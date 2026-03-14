# Cross-Platform Porting TODO

Goal: make the codebase cross-platform (Windows, macOS, Linux).
All changes must remain compatible with the existing MSVC/Windows build.
Primary dev environment: macOS ARM64 (Apple Silicon M1), CLion.

## ~~1. Fix backslash includes~~ DONE
## ~~2. Add missing compat stubs~~ DONE
## ~~3. Fix pointer-to-int casts (64-bit)~~ DONE
## ~~4. Rewrite x86 inline assembly~~ DONE (21 files, ~155 blocks)
## ~~8. Replace Win32 file I/O~~ DONE (compat_file_io.h)
## ~~9. Replace WinMain~~ DONE (platform_main.cpp)
## ~~Fix duplicate symbols~~ DONE
## ~~Fix custom memory allocator~~ DONE
## ~~Fix 32-bit pointer size assumptions~~ DONE (32 locations, `malloc(4*N)` → `malloc(sizeof(T*)*N)`)

## NEXT: Wire up SDL window and event loop
Game initializes fully — loads .gsc archives, reads game data, exits cleanly.
But nothing displays because Win32 window/event stubs are no-ops.
Need to connect the existing SDL2 code in Ddini.cpp to the game loop:
- `CreateWindow`/`RegisterClass` → SDL2 already creates `gWindow` in `CreateDDObjects()`
- `PeekMessage`/`GetMessage`/`DispatchMessage` → `SDL_PollEvent()`
- Keyboard input (VK_* codes) → SDL_SCANCODE/SDL_KEYCODE mapping
- Mouse input → SDL mouse events
- The rendering pipeline (FlipPages) already uses SDL2

## 5. Port CommCore to POSIX sockets
CommCore compiles and links. Remaining:
- Replace `SO_MAX_MSG_SIZE` with reasonable default
- Replace registry-based CCUID storage with config file

## 6. Tackle `__declspec(dllexport/dllimport)` pattern
Already linking statically on macOS. Future cleanup.

## 7. Replace Win32 threading with std::thread or SDL threads
- `CreateThread` → `std::thread` / `SDL_CreateThread`
- `CRITICAL_SECTION` → `std::mutex`
- `WaitForSingleObject` → `std::condition_variable` / SDL equivalents

## Build status on macOS ARM64
- **CommCore** — compiles and links (static lib)
- **IChat** — compiles and links (static lib)
- **IntExplorer** — compiles and links (static lib)
- **Main executable** — compiles, links, and runs. Native ARM64 binary (3.5MB)
- **Runtime** — initializes fully: loads .gsc archives, reads game data,
  exits cleanly (code 0). No window/display yet (Win32 window stubs are no-ops).

## Assembly rewrite status (ALL DONE)
21 files, ~155 blocks rewritten from x86 to portable C:
3DGraph.cpp, Fastdraw.cpp, GP_Draw.cpp (79 blocks), fog.cpp (13),
RealWater.cpp (15), path.cpp (15), Masks.cpp (7), Lines.cpp (6),
ShipTrace.cpp (4), mapa.cpp (4), 3DRandMap.cpp (3), NewMon.cpp (3),
Groups.cpp, ZBuffer.cpp, Multi.cpp, Stringshash.cpp, Megapolis.cpp,
Nation.cpp, AntiBug.cpp, MapDiscr.h, NewMon.h

## Notes

### Graphics pipeline (already mostly portable)
Software-based 8-bit rendering to `ScreenPtr`, then `FlipPages()` in
`Ddini.cpp` converts to SDL texture for display. Already uses SDL2.

### Audio (already portable)
`Cdirsnd.cpp` uses SDL2_mixer. No porting work needed.

### Key fixes applied
- Custom allocator (`_ExMalloc`/`_ExFree`) debug markers disabled on non-Win32
- `operator new`/`delete` override disabled on non-Win32
- 32-bit pointer assumptions (`malloc(4*N)`) fixed across 32 locations
- `std::ifstream` in static init replaced with `fopen()` to avoid crashes
- Wine detection bypassed on non-Win32
