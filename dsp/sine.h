#pragma once

#include "fx_dsp.h"
#include "macros.h"
#include "processor.h"
#include "touch_phase.h"

class Sine : public Processor
{
public:
  uint32_t getBufferSize() const override final { return 0; }

  enum
  {
    WAVE = 0U,
    SHAPE,
    MIX,
    NUM_PARAMS
  };

  enum
  {
    WAVE_SINE = 0,
    WAVE_TRI,
    WAVE_SQUARE,
    WAVE_SAW,
    NUM_WAVES
  };

  void setStereoMix(bool enabled) { stereo_mix_ = enabled; }

  void setParameter(uint8_t index, int32_t value) override final
  {
    switch (index)
    {
    case WAVE:
      wave_ = static_cast<uint32_t>(value);
      if (wave_ >= NUM_WAVES)
        wave_ = NUM_WAVES - 1;
      break;
    case SHAPE:
      shape_ = param_10bit_to_f32(value);
      break;
    case MIX:
      mix_ = fx::clip01(value / 1000.f);
      break;
    default:
      break;
    }
  }

  const char *getParameterStrValue(uint8_t index, int32_t value) const override final
  {
    static const char *wave_names[NUM_WAVES] = {"SINE", "TRI", "SQR", "SAW"};
    if (index == WAVE && value >= 0 && value < static_cast<int32_t>(NUM_WAVES))
      return wave_names[value];
    return nullptr;
  }

  void init(float *) override final
  {
    wave_ = WAVE_SINE;
    shape_ = 0.f;
    mix_ = 1.f;
    phasor_ = 0.f;
    gate_ = stereo_mix_ ? 0.f : 1.f;
    setPitch(fx::noteToInc(60.f, getSampleRate()));
  }

  void reset() override final
  {
    phasor_ = 0.f;
    if (stereo_mix_)
      gate_ = 0.f;
  }

  void setPitch(float w0)
  {
    w0_ = fx::clip(w0, 0.f, 0.49f);
  }

  void noteOn(uint8_t note, uint8_t velo) override final
  {
    (void)velo;
    setPitch(fx::noteToInc(static_cast<float>(note), getSampleRate()));
    gate_ = 1.f;
  }

  void noteOff(uint8_t note) override final
  {
    (void)note;
    if (stereo_mix_)
      gate_ = 0.f;
  }

  void allNoteOff() override final
  {
    if (stereo_mix_)
      gate_ = 0.f;
  }

  void touchEvent(uint8_t id, uint8_t phase, uint32_t x, uint32_t y) override final
  {
    (void)id;
    const float note = 36.f + (static_cast<float>(x) * (48.f / 1023.f));
    const float level = static_cast<float>(y) * (1.f / 1023.f);

    if (phase == kLogueTouchBegan || phase == kLogueTouchMoved || phase == kLogueTouchStationary)
    {
      setPitch(fx::noteToInc(note, getSampleRate()));
      gate_ = level;
      return;
    }

    if (phase == kLogueTouchEnded || phase == kLogueTouchCancelled)
      gate_ = 0.f;
  }

  void process(const float *__restrict in, float *__restrict out, uint32_t frames) override final
  {
    for (uint32_t sampleIndex = 0; sampleIndex < frames; ++sampleIndex)
    {
      const float wet = renderSample();
      if (stereo_mix_)
      {
        out[0] = fx::mix(in[0], wet, mix_);
        out[1] = fx::mix(in[1], wet, mix_);
        in += 2;
        out += 2;
      }
      else
      {
        out[0] = wet;
        ++out;
        in += 2;
      }
    }
  }

  float renderSample()
  {
    return tick() * gate_;
  }

private:
  float tick()
  {
    phasor_ += w0_;
    if (phasor_ >= 1.f)
      phasor_ -= 1.f;

    float sample = 0.f;
    switch (wave_)
    {
    case WAVE_TRI:
      sample = 1.f - 4.f * fx::absf(phasor_ - 0.5f);
      break;
    case WAVE_SQUARE:
      sample = (phasor_ < (0.5f - 0.45f * shape_)) ? 1.f : -1.f;
      break;
    case WAVE_SAW:
      sample = (2.f * phasor_) - 1.f;
      break;
    case WAVE_SINE:
    default:
      sample = fastsinfullf(phasor_ * 6.283185307179586f);
      break;
    }

    if (wave_ != WAVE_SQUARE)
      sample += fastsinfullf(phasor_ * 12.566370614359172f) * (shape_ * 0.35f);

    return sample;
  }

  bool stereo_mix_ = false;
  uint32_t wave_ = WAVE_SINE;
  float shape_ = 0.f;
  float mix_ = 1.f;
  float w0_ = 0.f;
  float phasor_ = 0.f;
  float gate_ = 1.f;
};
