# Cross-Platform Porting TODO

Goal: make the codebase cross-platform (Windows, macOS, Linux).
All changes must remain compatible with the existing MSVC/Windows build.
Primary dev environment: macOS ARM64 (Apple Silicon M1), CLion.

## ~~1. Fix backslash includes~~ DONE
~~Replace Windows-style `#include "arc\gscarch.h"` paths with forward slashes.~~

## ~~2. Add missing compat stubs as needed~~ DONE (initial pass)
~~Extend `src/compat/windows.h` and other compat headers incrementally.~~
CommCore, IChat, and IntExplorer (except FnDefine.cpp) compile on macOS.
Remaining: pointer-to-int casts in FnDefine.cpp (64-bit porting issue),
and Main executable not yet attempted. Compat stubs may need further
extension as the Main executable is tackled.

## ~~3. Fix pointer-to-int casts (64-bit porting)~~ DONE
~~FnDefine.cpp (IntExplorer) — 17 casts blocking compilation.~~
Fixed by changing `UserParam`, `ComplexBox::param`, `CustomBox::param`
from `int` to `intptr_t` in dialogs.h, and updating cast sites.
Also replaced inline assembly in MapDiscr.h (`DistTo`) and NewMon.h
(`Norma`) with portable C equivalents. Fixed `WallSystem::Show` extra
qualification in walls.h. More pointer-to-int casts likely in Main executable.

## ~~4. Rewrite x86 inline assembly~~ DONE
All 21 files with x86 assembly now have portable C implementations.
Original assembly preserved in `#if defined(_MSC_VER) && defined(_M_IX86)`
guards for Windows builds.

## 5. Port CommCore to POSIX sockets
CommCore already compiles and links on macOS. Remaining work:
- Replace `SO_MAX_MSG_SIZE` usage with a reasonable default
- Replace registry-based CCUID storage with a config file
- Goal: working cross-platform networking library

## 6. Tackle `__declspec(dllexport/dllimport)` pattern
The project uses DLL exports between the main exe and the libraries. Options:
- Link everything statically (simplest — already done for macOS CMake build)
- Replace `__declspec` with `__attribute__((visibility("default")))` via a macro

## 7. Replace Win32 threading with std::thread or SDL threads
- `CreateThread` → `std::thread` / `SDL_CreateThread`
- `CRITICAL_SECTION` → `std::mutex`
- `WaitForSingleObject` → `std::condition_variable` / SDL equivalents

## 8. Replace Win32 file I/O with standard C/C++
- `CreateFile`/`ReadFile`/`WriteFile` → `fopen`/`fread`/`fwrite`
- `FindFirstFile`/`FindNextFile` → `std::filesystem` or `dirent.h`
- Registry access → config file or `SDL_GetPrefPath`

## ~~9. Replace WinMain with SDL_main~~ DONE
Added `platform_main.cpp` with `main()` that calls `WinMain()`.
Also contains stubs for DirectPlayLobby and CPinger.

## 10. Replace Win32 window/message loop with SDL events
The game currently uses `CreateWindow`, `PeekMessage`, `DispatchMessage` etc.
SDL2 already creates the window (`gWindow` in Ddini.cpp). The Win32 message
loop in `Ddex1.cpp` needs to be replaced with `SDL_PollEvent()`.
This includes keyboard/mouse input handling in `Interface.cpp` and `Mouse_X.cpp`.

## Notes

## NEXT: Fix duplicate symbol definitions
24 symbols are defined in multiple .cpp files (MSVC merges these, macOS
linker doesn't). Need to make one definition the primary and others `extern`.
Examples: `HiMap`, `TerrMap`, `SelCenter`, `CurPalette`, `ChatMess`, etc.
This is the **only remaining blocker** for linking.

### Assembly rewrite status by file

**All done (real C implementations):**
- `3DGraph.cpp` — full C rewrite (13 functions: triangle rendering, fog, texture mapping)
- `Fastdraw.cpp` — 8 RLC sprite functions (full clipped renderer with palette variants)
- `Masks.cpp` — 7 blocks (RLE mask decode, transparency blending, diamond shapes)
- `Lines.cpp` — 6 blocks (Hline, Vline, Bresenham line drawing)
- `ShipTrace.cpp` — 4 blocks (blob rendering, update loop, slot search)
- `mapa.cpp` — 4 blocks (minimap square rendering)
- `3DRandMap.cpp` — 3 blocks (grass rendering, dead code removal)
- `NewMon.cpp` — 3 blocks (memory zeroing)
- `Groups.cpp` — 1 block (bubble sort)
- `ZBuffer.cpp` — 1 block (bubble sort)
- `Multi.cpp` — 1 block (memset)
- `Stringshash.cpp` — 1 block (byte hash sum)
- `Megapolis.cpp` — 1 block (5-pixel cross)
- `Nation.cpp` — 1 block (struct fill)
- `fog.cpp` — 13 blocks (fog blur, darkfog bilinear interpolation, minimap fog)
- `path.cpp` — 15 blocks (bitmap bit ops, Bresenham line rasterization)
- `RealWater.cpp` — 15 blocks (wave simulation, water rendering, gradient lookup)
- `GP_Draw.cpp` — 79 blocks (sprite rendering, pixel ops, clipping, decompression)
- `AntiBug.cpp` — 1 block (checksum)
- `MapDiscr.h` (`DistTo`) — Chebyshev distance
- `NewMon.h` (`Norma`) — approximate distance

### Build status on macOS ARM64
- **CommCore** — compiles and links (static lib)
- **IChat** — compiles and links (static lib)
- **IntExplorer** — compiles and links (static lib)
- **Main executable** — ALL 97 source files compile, linking blocked by
  24 duplicate symbol definitions across translation units

### Graphics pipeline (already mostly portable)
The rendering is software-based: all drawing goes to an 8-bit `ScreenPtr`
buffer, then `FlipPages()` in `Ddini.cpp` converts it to an SDL texture
for display. This part already works cross-platform via SDL2.

### 3D rendering (portable except assembly)
The 3D terrain renderer (`3DGraph.cpp`) uses software triangle rasterization.
Contains x86 assembly for textured triangle rendering — must be rewritten
in C/C++ for ARM64.

### Audio (already portable)
`Cdirsnd.cpp` already uses SDL2_mixer. No porting work needed.

### sizeof without parentheses
MSVC accepts `sizeof TypeName` without parentheses but Clang doesn't.
Already fixed in: cs_chat.cpp, GSC_ChatWindow.cpp, ParseRQ.cpp,
Bmptool.cpp, DataXchange.cpp, Explore.cpp, FnDefine.cpp.
May appear in more Main executable files.
