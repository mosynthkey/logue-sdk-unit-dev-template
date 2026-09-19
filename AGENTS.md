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
web/index.html              # landing page + Web MIDI send
web/osc.html                # Emscripten shell for the mkII simulator
web/xypad.html              # Emscripten shell for the NTS-3 simulator
```

DSP lives once. Each target is a thin adapter: `header.c`, `unit.cc`, `wasm.cc`.

## What to edit when the user describes a unit

1. Rewrite `dsp/*.h` (keep one processor class).
2. Update `targets/*/header.c` parameter lists so they match the DSP.
3. Update `plugin.json` `name`, `description`, and `params`.
4. If the display name changes, update `.name` in both `header.c` files.
5. If the project id changes, update `plugin.json` `id` and both `config.mk`
   `PROJECT` values. They must stay the same short lowercase token.

Do not introduce Vue, npm, CSS frameworks, or extra HTML pages. Keep `web/`
plain HTML. `osc.html` and `xypad.html` are Emscripten shells; put user-facing
copy in `web/index.html` and `plugin.json`.

## Targets

| Target | File | Module | Notes |
| --- | --- | --- | --- |
| `nts-1_mkii` | `.nts1mkiiunit` | default `osc` | Keyboard instrument. Can be `modfx` / `delfx` / `revfx` instead. |
| `nts-3_kaoss` | `.nts3unit` | `genericfx` only | XY pad. `unit_touch_event` is required. |

Module types are not interchangeable. An `osc` binary will not load as NTS-3
`genericfx`. If the user wants a delay / reverb / modulator on mkII, change
`PROJECT_TYPE` and the header include (`unit_delfx.h` / `unit_revfx.h` /
`unit_modfx.h`) in that target only. NTS-3 stays `genericfx`.

## Build

```bash
git submodule update --init
git -C third_party/logue-sdk submodule update --init platform/ext/CMSIS

make unit GCC_BIN_PATH=/path/to/gcc-arm-none-eabi-10.3-2021.10/bin
make wasm EMCC_BIN_PATH=/path/to/emsdk/upstream/emscripten
make site
python3 scripts/serve.py dist
```

Do not run `git submodule update --recursive` (it pulls the huge emsdk tree).

Hardware units land in `targets/<platform>/<id>.<suffix>`. Wasm files land in
`targets/<platform>/sim/`. `make site` copies both into `dist/` for GitHub Pages.

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
(`F0 42 3g 00 01 72`). Chrome / Edge only. Do not replace this with a new
protocol unless the user is debugging SysEx.

## Comments and UI copy

User-facing strings and code comments are English. Comment only logic that is
hard to see from the code. Iterator names must say what they iterate
(`sampleIndex`, `paramIndex`), not `i` / `j`.
