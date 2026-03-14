# Cross-Platform Porting TODO

## ~~1. Fix backslash includes~~ DONE
~~Replace Windows-style `#include "arc\gscarch.h"` paths with forward slashes.~~
~~Forward slashes work on Windows too, so this is a safe no-op change.~~
~~Unblocks CLion from parsing most of the Main executable.~~

## ~~2. Add missing compat stubs as needed~~ DONE (initial pass)
~~Extend `src/compat/windows.h` and other compat headers incrementally~~
~~as new missing types or functions are encountered while browsing in CLion.~~
CommCore, IChat, and IntExplorer (except FnDefine.cpp) compile on macOS.
Remaining: pointer-to-int casts in FnDefine.cpp (64-bit porting issue),
and Main executable not yet attempted. Compat stubs may need further
extension as the Main executable is tackled.

## 3. Rewrite x86 inline assembly in Fastdraw.cpp
`Fastdraw.cpp` uses MSVC inline assembly (`__asm`) for RLC sprite decoding
and blitting. This is a **major blocker** for:
- Non-MSVC compilers (Clang, GCC)
- ARM platforms (Apple Silicon)

The assembly handles clipped RLC decompression with `rep movsb`/`rep movsd`.
Needs rewriting in portable C/C++. The logic is self-contained — decode
run-length data and copy palette-indexed bytes to `ScreenPtr`.

## 4. Port CommCore to POSIX sockets
CommCore already compiles on macOS. Remaining work:
- Replace `SO_MAX_MSG_SIZE` usage with a reasonable default
- Replace registry-based CCUID storage with a config file
- Goal: working cross-platform networking library

## 5. Tackle `__declspec(dllexport/dllimport)` pattern
The project uses DLL exports between the main exe and the libraries. Options:
- Link everything statically (simplest)
- Replace `__declspec` with `__attribute__((visibility("default")))` via a macro

## 6. Replace Win32 threading with std::thread or SDL threads
- `CreateThread` → `std::thread` / `SDL_CreateThread`
- `CRITICAL_SECTION` → `std::mutex`
- `WaitForSingleObject` → `std::condition_variable` / SDL equivalents

## 7. Replace Win32 file I/O with standard C/C++
- `CreateFile`/`ReadFile`/`WriteFile` → `fopen`/`fread`/`fwrite`
- `FindFirstFile`/`FindNextFile` → `std::filesystem` or `dirent.h`
- Registry access → config file or `SDL_GetPrefPath`

## 8. Replace WinMain with SDL_main
SDL2 provides a cross-platform entry point mechanism.
Change in `Ddex1.cpp` — replace `WinMain(HINSTANCE, ...)` with
`int main(int argc, char* argv[])` using `SDL_main.h`.

## 9. Replace Win32 window/message loop with SDL events
The game currently uses `CreateWindow`, `PeekMessage`, `DispatchMessage` etc.
SDL2 already creates the window (`gWindow` in Ddini.cpp). The Win32 message
loop in `Ddex1.cpp` needs to be replaced with `SDL_PollEvent()`.
This includes keyboard/mouse input handling in `Interface.cpp` and `Mouse_X.cpp`.

## Notes

### Graphics pipeline (already mostly portable)
The rendering is software-based: all drawing goes to an 8-bit `ScreenPtr`
buffer, then `FlipPages()` in `Ddini.cpp` converts it to an SDL texture
for display. This part already works cross-platform via SDL2.

### 3D rendering (portable except assembly)
The 3D terrain renderer (`3DGraph.cpp`) uses software triangle rasterization.
It also contains some x86 assembly for textured triangle rendering that
would need C/C++ rewrites.

### Audio (already portable)
`Cdirsnd.cpp` already uses SDL2_mixer. No porting work needed.
