# Cross-Platform Porting TODO

Goal: make the codebase cross-platform (Windows, macOS, Linux).
All changes must remain compatible with the existing MSVC/Windows build.
Primary dev environment: macOS ARM64 (Apple Silicon M1), CLion.

## ~~1. Fix backslash includes~~ DONE
~~Replace Windows-style `#include "arc\gscarch.h"` paths with forward slashes.~~

## ~~2. Add missing compat stubs~~ DONE
~~Extend `src/compat/windows.h` and other compat headers incrementally.~~
CommCore, IChat, IntExplorer all compile on macOS.

## ~~3. Fix pointer-to-int casts (64-bit)~~ DONE
~~Original code stores pointers in `int`/`DWORD` (32-bit) — truncates on
ARM64 where pointers are 8 bytes. Fixed with `intptr_t`/`uintptr_t`.~~

## ~~4. Rewrite x86 inline assembly~~ DONE (21 files, ~155 blocks)
~~x86 `__asm` blocks cannot compile on ARM64. Rewrote all to portable C:
`Fastdraw.cpp` (RLC sprite decoding), `3DGraph.cpp` (textured triangle
rasterization), `GP_Draw.cpp` (79 blocks), `fog.cpp`, `RealWater.cpp`,
`path.cpp`, `Masks.cpp`, `Lines.cpp`, and 13 more files.~~

## ~~8. Replace Win32 file I/O~~ DONE (compat_file_io.h)
~~`CreateFile`/`ReadFile`/`WriteFile` → `open`/`read`/`write`.
`CreateFileMapping`/`MapViewOfFile` → `mmap`.
`FindFirstFile`/`FindNextFile` → `opendir`/`readdir` with `fnmatch`.~~

## ~~9. Replace WinMain~~ DONE (platform_main.cpp)
~~`int main()` entry point on non-Windows, calls `WinMain()`.
DirectPlayLobby stubs, CPinger stub implementation.~~

## ~~Fix duplicate symbols~~ DONE
~~25 symbols defined in both exe and libraries. Fixed with `extern`/`static`.~~

## ~~Fix custom memory allocator~~ DONE
~~`_ExMalloc` writes `0xCAFEBABE` to first 4 bytes of every allocation,
corrupting C++ standard library objects during static init. Disabled on
non-Win32. `operator new`/`delete` override also disabled.~~

## ~~Fix 32-bit pointer size assumptions~~ DONE (32 locations, `malloc(4*N)` → `malloc(sizeof(T*)*N)`)

## ~~PREV: Fix crash in Loading()~~ DONE
~~Nature.cpp `short**` alloc, GP_Draw.cpp `int`→`intptr_t`.~~

## ~~PREV: Fix GP_Header::Pack pointer size~~ DONE
~~Changed `byte*` to `DWORD` to match 4-byte on-disk format.~~

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
Structs with pointer fields were serialized with `sizeof()` which differs
between 32-bit (file format) and 64-bit (macOS). Created on-disk structs
with DWORD placeholders for pointers and conversion helpers.
**Map files** (SaveNewMap.cpp — must match 32-bit format for original game data):
- `Area` → `Area_File` (TopoGraf.h) — was causing Brigade.cpp crashes
- `ActiveZone` → `ActiveZone_File` (Activezone.h)
- `ActiveGroup` → `ActiveGroup_File` (Activezone.h)
**Save files** (LoadSave.cpp):
- `Order1` → `Order1_File` (MapDiscr.h) — portable
- `SelGroup` — fixed buffer overflow (`sizeof-8` → `sizeof-2*sizeof(word*)`)
- `AnmObject` — fixed buffer overflow (`sizeof-4` → `sizeof-sizeof(NewAnimation*)`)
- `ExtendedBrigade` → `ExtendedBrigade_File` (Megapolis.h) — portable
- `int()` pointer truncations → `intptr_t` (AI_Army field offset calculations)

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
  - Occasional crashes in gameplay — some may be pre-existing bugs,
    some may be remaining 64-bit issues

## Make save files fully portable across 32-bit and 64-bit
Most save-file structs are now portable. Remaining issues:
- `City` (Megapolis.h) — enormous struct with many embedded `Brigade` arrays
  (each Brigade has 7 pointers), `AI_Army` arrays, and its own pointer fields.
  Serialized whole with `xBlockWrite(SB, CT, sizeof(City))`. Creating an
  on-disk format would require handling hundreds of fields. Currently
  self-consistent on 64-bit (same sizeof for write/read).
- `BrigadeOrder` / `ArmyOrder` — variable-size allocations (`BOR->Size`
  includes `sizeof(BrigadeOrder)` which differs per platform). The extra-data
  scheme (`BOR + 1` for bytes beyond the struct) is tightly coupled to
  platform-specific sizeof. On-disk format structs defined but not yet wired
  into save/load due to variable-size complexity.
Note: these only affect cross-platform save portability (macOS↔Windows).
Saves created and loaded on the same platform work correctly.
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
- `memcpy` → `memmove` for overlapping array shifts (~70 instances).
  The original code uses `memcpy(arr+i, arr+i+1, ...)` to shift array
  elements when removing an item. Source and destination overlap, which is
  undefined behavior per the C standard. On Windows/MSVC x86, `memcpy`
  happened to copy forward (low→high), which is safe for this pattern.
  On macOS ARM64/Clang, `memcpy` may use optimized SIMD that copies in
  arbitrary order, corrupting the data. `memmove` handles overlap correctly
  on all platforms. ASan catches this as `memcpy-param-overlap`.
- Buffer overflow fixes found via ASan (all pre-existing UB, harmless on
  Windows 32-bit due to struct padding/alignment, crashes on macOS 64-bit):
  - `xBlockRead/Write(&byteVar, 4)` — reading/writing 4 bytes from 1-byte
    globals (`BalloonState`, `CannonState`, etc.) and 2-byte globals
    (`MAXOBJECT`). Overwrites adjacent globals. Fixed with temp `int`.
  - `DEFPLNAMES[8][0]` — off-by-one, should be `DEFPLNAMES[i][0]`
  - `SAVMES` length check `> 10` should be `> 16`
  - `Dialogs.cpp` `malloc(4)` for `char**` — 4 bytes for an 8-byte pointer
  - `GET_INT/GET_BYTE` in 3DRandMap.cpp — no bounds check on SaveBuf
  - `HostID == -1` used as array index without guard
  - `GetGPWidth/Height/Shift` — no `n < 0` guard
  - `CDirSound` class — `#pragma pack(1)` corrupted `std::string` layout
  - `minimap` off-by-one: `MaxMLX - MiniLx - 1` → `-1` when equal

### Known issue: terrain artifacts on first save load after restart
SectMap (terrain section data) gets corrupted during the first save load
after app restart. Subsequent loads in the same session work correctly.
The corruption produces invalid section values (should be 0-2, gets values
like 109, 201, 235) causing wrong terrain blending. Guarded in
`CopyMaskedBitmap` and `PrepareIntersection1/2` to prevent crashes.
Root cause: likely an initialization order issue — first load allocates
new arrays via `SetupArrays()` (ADDSH≠ACTUAL_ADDSH), subsequent loads
reuse arrays via `ClearArrays()` (ADDSH==ACTUAL_ADDSH). The difference
in code paths may leave SectMap partially uninitialized or corrupted
by the random map regeneration code.


## Check
- archives unarchiving location: currently work root (Cossacks Back to War v1.52 (2025)), not sure that is how it was on Windows. 
Need to check if Windows unarchived to the archive location.
