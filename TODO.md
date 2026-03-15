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

## ~~PREV: Fix crash in Loading()~~ DONE (Nature.cpp `short**` alloc, GP_Draw.cpp `int`→`intptr_t`)
## ~~PREV: Fix GP_Header::Pack pointer size~~ DONE (changed `byte*` to `DWORD` to match file format)

## ~~PREV: Fix GP sprite rendering~~ DONE
Fixed CASHREF (DWORD→uintptr_t), INTV/PTRV macros, cash buffer header layout
(8→16 bytes on 64-bit), NO_PACK sentinel, `(int)(intptr_t)maskPtr` truncation
(15 instances), GP_Header::Pack (byte*→DWORD), comprehensive 32-bit sweep
(~95 instances across 34 files).

## ~~PREV: Wire up SDL input events~~ DONE
Created `compat_sdl_events.h` — translates SDL events to Win32 MSG structs.
`PeekMessage` polls SDL, fills MSG with WM_MOUSEMOVE/WM_LBUTTONDOWN/WM_KEYDOWN etc.
`DispatchMessage` calls the game's `WindowProc`. Mouse, keyboard, wheel all work.

## ~~PREV: Fix rendering artifacts~~ DONE (GP renderer lineScr save/restore)
## ~~PREV: Fix RLC OfsTable 64-bit~~ DONE (RLC_ADDR macro, LoadRLC relative offsets)
## ~~PREV: Fix AI DLL loading~~ DONE (skip assert on macOS)

## ~~PREV: Fix gameplay crashes~~ DONE
Fixed SmartLink (function pointer obfuscation), UParam2 (fog pointer storage),
Nation.cpp pointer arithmetic, SpRefs/WRefs/Obj3Map/TRIANG/SCINF allocations,
Masks.cpp RLC_ADDR, StrHash memset overflow, more memset pointer sizes.

## ~~PREV: Fix struct serialization 32/64-bit mismatch~~ DONE
Structs with pointer fields (`Area`, `ActiveZone`, `ActiveGroup`, `SelGroup`)
were serialized with `sizeof()` which differs between 32-bit (file format)
and 64-bit (macOS). Created on-disk structs (`Area_File`, `ActiveZone_File`,
`ActiveGroup_File`) with DWORD placeholders for pointers and conversion
helpers. Fixed 7 read/write sites + 3 size calculations in SaveNewMap.cpp
and LoadSave.cpp. Also fixed `SelGroup` buffer overflow
(`sizeof(SelGroup)-8` → `sizeof(SelGroup)-2*sizeof(word*)`).
This was causing Brigade.cpp crashes (corrupted topology `Link` pointers)
when loading designed maps.

## Wire up SDL window and event loop (partially done)
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
- **Runtime** — GAME IS FULLY PLAYABLE WITH AUDIO! Build, move, save/load,
  sound effects all work. Rendering correct. Built-in AI works.
  Keyboard (F12=menu, Escape=cancel) and mouse (click, drag select) work.
  Campaigns show immediate defeat — the campaign scripting engine needs
  investigation. cew.dll contains script data but the interpreter may
  depend on LoadLibrary to load AI DLLs for mission scripts.
  Known issues:
  - Campaigns: immediate defeat. Each mission has a `Mission.dll` containing
    compiled x86 Windows code for mission logic (victory/defeat conditions,
    triggers, scripted events). These can't run on macOS ARM64.
    Also `unrar.dll` needed to extract some campaign data from .rar files.
    Would need mission DLL source code or Wine/Rosetta to make campaigns work.
  - Multiplayer: TCP/IP LAN mode doesn't work (requires DirectPlay which
    is Windows-only). Direct IP mode uses CommCore UDP stack and should
    work — select "Direct IP" when creating/joining a game.
  - `Order1` struct serialization still uses `sizeof()` with pointers in
    save files — self-consistent on 64-bit but cross-platform saves won't work
  - Occasional crashes in gameplay — some may be pre-existing bugs,
    some may be remaining 64-bit issues
  - `#pragma pack(1)` from game headers can break system struct layouts —
    fixed for iphlpapi.h, may affect other system headers

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

### Build and test commands
```bash
# Build (from project root):
cd /Users/mazhnik/Codes/opensource/cossacks-1.52
cmake -B build -S .
cmake --build build

# Run (from game data directory):
cd "/Users/mazhnik/Codes/opensource/cossacks-1.52/Cossacks Back to War v1.52 (2025)"
"../build/src/Main executable/Cossacks" 2>&1

# CLion: Set working directory to the game data folder in Run > Edit Configurations
```

### Audio (already portable)
`Cdirsnd.cpp` uses SDL2_mixer. No porting work needed.

### Key fixes applied
- Custom allocator (`_ExMalloc`/`_ExFree`) debug markers disabled on non-Win32
- `operator new`/`delete` override disabled on non-Win32
- 32-bit pointer assumptions (`malloc(4*N)`) fixed across 32 locations
- `std::ifstream` in static init replaced with `fopen()` to avoid crashes
- Wine detection bypassed on non-Win32
