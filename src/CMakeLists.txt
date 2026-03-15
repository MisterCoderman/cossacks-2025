set(CMAKE_CXX_STANDARD 17)
set(CMAKE_C_STANDARD 11)

# On non-Windows, add our compatibility headers FIRST so they shadow real Windows headers
if(NOT WIN32)
    set(COMPAT_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/compat")
    include_directories(BEFORE SYSTEM "${COMPAT_INCLUDE_DIR}")

    # Suppress warnings for the massive amount of compat-related issues
    add_compile_options(
        -Wno-pragma-once-outside-header
        -Wno-unused-parameter
        -Wno-unused-variable
        -Wno-unused-function
        -Wno-missing-field-initializers
        -Wno-sign-compare
        -Wno-writable-strings
        -Wno-reorder
        -Wno-invalid-offsetof
        -Wno-c++11-narrowing
        -Wno-microsoft-anon-tag
        -Wno-error
        -Wno-incompatible-pointer-types
        -fpermissive
        -fms-extensions
        -ferror-limit=0
    )
endif()

# Find SDL2 (use system SDL2 on macOS via Homebrew, or bundled on Windows)
if(WIN32)
    # On Windows, use the bundled SDL2
    set(SDL2_INCLUDE_DIRS "${CMAKE_CURRENT_SOURCE_DIR}/Main executable/sdl/include")
    set(SDL2_MIXER_INCLUDE_DIRS "${CMAKE_CURRENT_SOURCE_DIR}/Main executable/SDL2_mixer/include")
else()
    find_package(PkgConfig REQUIRED)
    pkg_check_modules(SDL2 REQUIRED sdl2)
    pkg_check_modules(SDL2_MIXER REQUIRED SDL2_mixer)
    # Add SDL2 include dirs globally so compat headers can find SDL.h
    include_directories(SYSTEM ${SDL2_INCLUDE_DIRS})
endif()

# Define NODDRAW as the Release config does
add_definitions(-DNODDRAW)

# Sub-projects
add_subdirectory("CommCore library")
add_subdirectory("IChat library")
add_subdirectory("IntExplorer library")
add_subdirectory("Main executable")
