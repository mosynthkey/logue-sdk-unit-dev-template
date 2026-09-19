# Agent Guide

This repository is a **single logue SDK unit**. Do not add a second plugin
directory. When the user asks for a new sound, change this unit in place.

## Layout

```text
plugin.json                 # name, description, targets, params
dsp/                        # shared DSP, device-independent
common/                     # helpers (fx_dsp.h, touch_phase.h, dev_id.h)
targets/nts-1_mkii/         # NTS-1 mkII adapter (osc by default)
targets/nts-3_kaoss/        # NTS-3 adapter (genericfx only)
targets/microkorg2/         # microKORG2 adapter (osc by default)
web/index.html              # single page: target dropdown, download, send, simulator iframe
web/osc.html                # Emscripten shell for the mkII simulator (embedded, not linked)
web/xypad.html              # Emscripten shell for the NTS-3 simulator (embedded, not linked)
```

DSP lives once. Each target is a thin adapter: `header.c`, `unit.cc`, and
`wasm.cc` where the platform supports a browser simulator.

## What to edit when the user describes a unit

1. Rewrite `dsp/*.h` (keep one processor class). For microKORG2, also keep a
   thin `*_mk2.h` multi-voice wrapper (see `dsp/sine_mk2.h`, same pattern as
   my-logue-sdk-units `plugins/*/dsp/*_mk2.h`).
2. Update `targets/*/header.c` parameter lists so they match the DSP.
   microKORG2 headers typically declare `num_params = 13` with unused slots
   left empty (see `targets/microkorg2/header.c`).
3. Update `plugin.json` `name`, `description`, and `params`.
4. If the display name changes, update `.name` in every `header.c`.
5. If the project id changes, update `plugin.json` `id` and every `config.mk`
   `PROJECT` value. They must stay the same short lowercase token.

Do not introduce Vue, npm, CSS frameworks, or extra user-facing HTML pages.
Keep `web/` plain HTML. The only page users open is `web/index.html` (target
dropdown + download + send + simulator iframe). `osc.html` and `xypad.html`
are Emscripten build shells embedded in that iframe; put user-facing copy in
`web/index.html` and `plugin.json`.

## Targets

| Target | File | Module | Notes |
| --- | --- | --- | --- |
| `nts-1_mkii` | `.nts1mkiiunit` | default `osc` | Keyboard. Can be `modfx` / `delfx` / `revfx`. Wasm yes. Web MIDI yes. |
| `nts-3_kaoss` | `.nts3unit` | `genericfx` only | XY pad. `unit_touch_event` required. Wasm yes. Web MIDI yes. |
| `microkorg2` | `.mk2unit` | default `osc` | Multi-voice. No wasm. No Web MIDI — USB mass storage only. |

Module types are not interchangeable. An `osc` binary will not load as NTS-3
`genericfx`. If the user wants a delay / reverb / modulator on mkII or
microKORG2, change `PROJECT_TYPE` and the header include in that target only.
NTS-3 stays `genericfx`.

## Build

```bash
git submodule update --init
git -C third_party/logue-sdk submodule update --init platform/ext/CMSIS

make unit GCC_BIN_PATH=/path/to/gcc-arm-none-eabi-10.3-2021.10/bin
make wasm EMCC_BIN_PATH=/path/to/emsdk/upstream/emscripten
make site
python3 scripts/serve.py dist
```

microKORG2 needs the drumlogue / microKORG2 A7 toolchain under
`third_party/logue-sdk/tools/microkorg2-toolchain`. CI downloads it. The
target Makefile picks it up automatically when present (same as
my-logue-sdk-units). Do not export an empty `CROSS_COMPILE` from the root
Makefile — that blocks the auto-detect.

Do not run `git submodule update --recursive` (it pulls the huge emsdk tree).

Hardware units land in `targets/<platform>/<id>.<suffix>`. Wasm files land in
`targets/<platform>/sim/` (mkII and NTS-3 only). `make site` copies both into
`dist/` for GitHub Pages.

## NTS-3 pitfalls

NTS-3 does not export `sinf` / `cosf` / `expf` / `powf` / `exp2f` to unit ELFs.
Prefer `common/fx_dsp.h` and `utils/float_math.h` (`fast*`, `si_fabsf`). If a
true libm call is unavoidable, keep `ULIBS = -lm` in `config.mk`.

`fasterexpf` is unusable for per-sample envelope coefficients near 1.
`fasterexpf(0) ≈ 0.971`, so a supposed 80 ms decay dies in a few milliseconds.
Use `level = fasterexpf(-age / tau)` with `|age/tau|` not tiny, or `1.f + x`
when `|x|` is tiny.

Never use `fasterpow2f` for MIDI note to Hz. `fasterpow2f(0) ≈ 0.971` makes A4
about 427 Hz and warps 12-TET. Use `fx::noteToHz` / `fx::noteToInc`.

After `make unit`, if the NTS-3 unit is silent or click-only:

```bash
arm-none-eabi-nm -u targets/nts-3_kaoss/build/sine.elf
```

Undefined `sinf` / `expf` / … means a link / libm problem. No undefined libm
plus a body shorter than ~20 ms on a host render means an envelope-length bug.

## Web MIDI send

`web/nts1-midi.js` talks to NTS-1 mkII (`F0 42 3g 00 01 73`) and NTS-3
(`F0 42 3g 00 01 72`). Chrome / Edge only. microKORG2 is download-only. Do not
replace this with a new protocol unless the user is debugging SysEx.

## Comments and UI copy

User-facing strings and code comments are English. Comment only logic that is
hard to see from the code. Iterator names must say what they iterate
(`sampleIndex`, `paramIndex`), not `i` / `j`.
