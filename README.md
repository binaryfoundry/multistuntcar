# Multi Stunt Car

This is a fork of *StuntCarRemake* with two player and reworked physics for uncapped frame rate.

Play: https://binaryfoundry.github.io/stuntcarhd/

## Current Project Layout

- `src/` - game and platform source code
- `data/` - runtime assets (tracks, sounds, bitmaps, font)
- `build/` - out-of-source native build directory
- `build-web/` - out-of-source Emscripten build directory (recommended)

## Build System

Desktop/native rendering and audio use SDL2 + OpenGL + SDL_ttf.
Web builds use Emscripten SDL ports.

## Native Build (Windows/Linux/macOS)

```bash
cmake -S . -B build
cmake --build build
```

Windows Release:

```powershell
cmake -S . -B build
cmake --build build --config Release
```

## Web Build (Emscripten)

```bash
emcmake cmake -S . -B build-web
cmake --build build-web
```

## Notes

- Original project: http://sourceforge.net/projects/stuntcarremake/
- Forked from: https://github.com/ptitSeb/stuntcarremake
- Some original sound-loading code came from Forsaken/ProjectX port work by chino.
