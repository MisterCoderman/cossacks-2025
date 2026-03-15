This is an ongoing effort to make the engine run natively on macOS & web (with the help of Claude Code).

### Current Status

The game is playable on **macOS** and in **web browsers (WASM)** in Random Map mode.
Campaigns are not yet supported (see limitations below).
Direct IP multiplayer is supported on macOS but not well-tested yet.

### Building on macOS

```bash
# Prerequisites
brew install sdl2 sdl2_mixer cmake

# Build
cmake -B build -S .
cmake --build build

# Run (from game data directory)
cd "Cossacks Back to War v1.52 (2025)"
"../build/src/Main executable/Cossacks"

# Optional: enable AddressSanitizer for debugging
cmake -B build -S . -DENABLE_ASAN=ON
```

Intel Mac: compiles without code changes, untested.

### Building for WebAssembly (browser)

```bash
# Prerequisites
brew install emscripten cmake

# Build (point GAME_DATA_DIR to your game data — path must not contain spaces)
ln -sf "/path/to/Cossacks Back to War v1.52 (2025)" /tmp/cossacks-data
emcmake cmake -B build-wasm -S . -DGAME_DATA_DIR=/tmp/cossacks-data
cmake --build build-wasm

# Serve (requires special headers for threading support)
cd build-wasm/src/Main\ executable/
python3 -c "
from http.server import HTTPServer, SimpleHTTPRequestHandler
class H(SimpleHTTPRequestHandler):
    def end_headers(self):
        self.send_header('Cross-Origin-Opener-Policy', 'same-origin')
        self.send_header('Cross-Origin-Embedder-Policy', 'require-corp')
        super().end_headers()
HTTPServer(('', 8080), H).serve_forever()
"
# Open http://localhost:8080/Cossacks.html
```

Note: the WASM build preloads ~293MB of game data. The browser will download this on first load.

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
