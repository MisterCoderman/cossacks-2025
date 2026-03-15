This is an ongoing effort to make the engine run natively on macOS (with the help of Claude Code).

### Current Status

The game is playable on macOS in Random Map mode. Campaigns are not yet supported (see limitations below). 
Direct IP multiplayer is supported but not well-tested yet.

### Building on macOS

```bash
# Prerequisites
brew install sdl2 sdl2_mixer cmake

# Build
cd /path/to/cossacks-1.52
cmake -B build -S .
cmake --build build

# Run (from game data directory)
cd "Cossacks Back to War v1.52 (2025)"
"../build/src/Main executable/Cossacks"
```

Intel Mac untested.

#### Compatibility updates
- CMake build system for macOS/Linux alongside existing MSVC/Windows
- Windows API compatibility layer (in `src/compat/`)
- x86 inline assembly rewritten to portable C (21 files, ~155 blocks) *
- 32-bit → 64-bit pointer fixes (`intptr_t` casts, `sizeof(T*)` allocations) *
- `memcpy -> memmove` for overlapping array shifts (~70 fixes) *

\* These changes improve correctness on all platforms, and I'll probably include them in separate PRs.

### Current limitations

- Campaigns: immediate defeat — Mission DLLs contain compiled x86 Windows code that needs to be ported
- Multiplayer: only Direct IP mode works
- Cross-platform saves: saves work within the same platform; macOS ↔ Windows save portability is partial (City struct not yet ported)

### All changes remain cross-platform compatible

The existing MSVC/Windows build should be unaffected — all non-Windows code is guarded with `#ifndef _WIN32` or `#if defined(_MSC_VER) && defined(_M_IX86)`.
The buffer overflow and memmove fixes improve correctness on Windows too.
