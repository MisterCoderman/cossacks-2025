set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_C_STANDARD 11)

# Platform detection
if(EMSCRIPTEN)
    message(STATUS "Building for WebAssembly (Emscripten)")
elseif(WIN32)
    message(STATUS "Building for Windows")
    if(CMAKE_SIZEOF_VOID_P EQUAL 8)
        message(STATUS "  Architecture: x64")
    else()
        message(STATUS "  Architecture: x86")
    endif()
elseif(APPLE)
    message(STATUS "Building for macOS (${CMAKE_SYSTEM_PROCESSOR})")
elseif(UNIX)
    message(STATUS "Building for Linux (${CMAKE_SYSTEM_PROCESSOR})")
endif()

# On non-Windows, add our compatibility headers FIRST so they shadow real Windows headers
if(NOT WIN32)
    set(COMPAT_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/compat")
    include_directories(BEFORE SYSTEM "${COMPAT_INCLUDE_DIR}")

    # Suppress warnings from original codebase that can't be fixed without
    # massive changes or are intentional patterns in the game code
    add_compile_options(
        -Wno-writable-strings          # char* from string literals (pervasive)
        -Wno-c++11-narrowing           # narrowing conversions in initializers
        -Wno-microsoft-anon-tag        # anonymous structs/unions (MSVC extension)
        -Wno-pragma-once-outside-header # pragma once in .cpp files
        -Wno-incompatible-pointer-types # C pointer type mismatches
        -Wno-nonportable-include-path  # Windows backslash #include paths
        -Wno-deprecated-declarations   # sprintf etc
        -Wno-pragma-pack              # #pragma pack(1) usage
        -Wno-multichar                 # multi-char constants like 'SJBO' (by design)
        -fpermissive
        -fms-extensions
        -ferror-limit=0
    )

    # AddressSanitizer: catches heap corruption, buffer overflows, use-after-free.
    # Enable with: cmake -B build -DENABLE_ASAN=ON
    option(ENABLE_ASAN "Enable AddressSanitizer" OFF)
    if(ENABLE_ASAN)
        message(STATUS "AddressSanitizer enabled")
        add_compile_options(-fsanitize=address -fno-omit-frame-pointer)
        add_link_options(-fsanitize=address)
    endif()
endif()

# Find SDL2
if(EMSCRIPTEN)
    # Emscripten provides SDL2 built-in via -s USE_SDL=2
    # Include dirs come from emscripten's sysroot automatically
    add_compile_options(-sUSE_SDL=2 -sUSE_SDL_MIXER=2 -pthread -fexceptions)
    add_link_options(-sUSE_SDL=2 -sUSE_SDL_MIXER=2)
elseif(WIN32)
    # On Windows, use the bundled SDL2
    set(SDL2_INCLUDE_DIRS "${CMAKE_CURRENT_SOURCE_DIR}/Main executable/sdl/include")
    set(SDL2_MIXER_INCLUDE_DIRS "${CMAKE_CURRENT_SOURCE_DIR}/Main executable/SDL2_mixer/include")
else()
    # macOS/Linux: use system SDL2 via pkg-config
    find_package(PkgConfig REQUIRED)
    pkg_check_modules(SDL2 REQUIRED sdl2)
    pkg_check_modules(SDL2_MIXER REQUIRED SDL2_mixer)
    include_directories(SYSTEM ${SDL2_INCLUDE_DIRS})
endif()

# Define NODDRAW — use SDL2 instead of DirectDraw
add_definitions(-DNODDRAW)

# Sub-projects (build order matches dependency chain)
add_subdirectory("CommCore library")
add_subdirectory("IChat library")
add_subdirectory("IntExplorer library")
add_subdirectory("Main executable")
