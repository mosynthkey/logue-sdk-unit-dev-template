#pragma once

/*
 * Small shared helpers. Header-only and libm-free (logue float_math only).
 */

#include "utils/float_math.h"
#include <stdint.h>

namespace fx
{

inline float clip01(float value)
{
  if (value < 0.f)
    return 0.f;
  if (value > 1.f)
    return 1.f;
  return value;
}

inline float clip(float value, float min_value, float max_value)
{
  if (value < min_value)
    return min_value;
  if (value > max_value)
    return max_value;
  return value;
}

inline float mix(float dry, float wet, float amount)
{
  return dry + (wet - dry) * amount;
}

inline float absf(float value)
{
  return si_fabsf(value);
}

inline float softclip(float value)
{
  return fastertanhf(value);
}

inline float onePoleCoeff(float hz, float sample_rate)
{
  const float clamped = clip(hz, 1.f, sample_rate * 0.45f);
  return 1.f - fasterexpf(-6.283185307179586f * clamped / sample_rate);
}

inline float noteToHz(float midi_note)
{
  // Use fastpow2f, not fasterpow2f: Mineiro's fasterpow2f(0) ≈ 0.971, so
  // A4 becomes ~427 Hz and intra-octave intervals warp (not 12-TET).
  return 440.f * fastpow2f((midi_note - 69.f) * (1.f / 12.f));
}

inline float noteToInc(float midi_note, float sample_rate)
{
  return noteToHz(midi_note) / sample_rate;
}

} // namespace fx
