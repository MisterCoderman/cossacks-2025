# Cossacks - Main game executable
add_executable(Cossacks
    3DBars.cpp
    3DGraph.cpp
    3DMapEd.cpp
    3DRandMap.cpp
    3DSurf.cpp
    ActiveScenary.cpp
    ActiveZone.cpp
    AntiBug.cpp
    ArchTool.cpp
    Arc/GSCarch.cpp
    Arc/GSCset.cpp
    Arc/isiMasks.cpp
    bmptool.cpp
    Brigade.cpp
    Build.cpp
    Cdirsnd.cpp
    Chkinet.cpp
    ConstStr.cpp
    Cwave.cpp
    Danger.cpp
    Ddex1.cpp
    Ddini.cpp
    DeviceCD.cpp
    Dialogs.cpp
    DrawForms.cpp
    Economy.cpp
    EinfoClass.cpp
    ExConst.cpp
    Fastdraw.cpp
    fog.cpp
    Fractal.cpp
    GameSound.cpp
    GP_Draw.cpp
    Graphs.cpp
    Groups.cpp
    gText.cpp
    GVClass.cpp
    Hint.cpp
    HTTP/HttpComm.cpp
    Icontool.cpp
    Interface.cpp
    IR_Main.cpp
    Lines.cpp
    LoadSave.cpp
    mapa.cpp
    MapSprites.cpp
    Masks.cpp
    Megapolis.cpp
    menu.cpp
    Mine.cpp
    Motion.cpp
    Mouse_X.cpp
    MousZone.cpp
    MPlayer.cpp
    Multi.cpp
    Nation.cpp
    Nature.cpp
    Navorots.cpp
    NewAI.cpp
    NewCode/MapOptionsEncoder.cpp
    NewCode/QueueManagement.cpp
    NewCode/SecureDivision.cpp
    NewCode/UdpHolePuncher.cpp
    NewILogin.cpp
    NewMon.cpp
    NewUpgrade.cpp
    Nuclear.cpp
    OnWater.cpp
    path.cpp
    RealWater.cpp
    Recorder.cpp
    Report.cpp
    Resfile.cpp
    River.cpp
    SafeNet.cpp
    SaveNewMap.cpp
    Select.cpp
    SelProp.cpp
    ShipTrace.cpp
    Source.cpp
    StrategyResearch.cpp
    stRecog.cpp
    Stringshash.cpp
    testserver.cpp
    Timing.cpp
    Tmixer.cpp
    TopoGraf.cpp
    Transport.cpp
    UnRar.cpp
    UnSyncro.cpp
    Upgrade.cpp
    VirtScr.cpp
    walls.cpp
    Waves.cpp
    Weapon.cpp
    ZBuffer.cpp
    platform_main.cpp
)

target_include_directories(Cossacks PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}"
    "${CMAKE_CURRENT_SOURCE_DIR}/Arc"
    "${CMAKE_CURRENT_SOURCE_DIR}/CEngine"
    "${CMAKE_CURRENT_SOURCE_DIR}/Chat"
    "${CMAKE_CURRENT_SOURCE_DIR}/Dialogs"
    "${CMAKE_CURRENT_SOURCE_DIR}/HTTP"
    "${CMAKE_CURRENT_SOURCE_DIR}/NewCode"
    "${CMAKE_CURRENT_SOURCE_DIR}/queryreporting"
    "${SDL2_INCLUDE_DIRS}"
    "${SDL2_MIXER_INCLUDE_DIRS}"
)

target_link_libraries(Cossacks PRIVATE CommCore)
# On non-Windows, link IChat and IntExplorer as static libraries.
if(NOT WIN32)
    target_link_libraries(Cossacks PRIVATE IChat IntExplorer)
endif()

if(EMSCRIPTEN)
    # Emscripten: SDL2 linked via -s flags (set in parent CMakeLists)
    # Game data directory — set with: cmake -DGAME_DATA_DIR=/path/to/game/data
    set(GAME_DATA_DIR "" CACHE PATH "Path to game data directory for WASM preloading")

    target_link_options(Cossacks PRIVATE
        -sALLOW_MEMORY_GROWTH=1
        -sINITIAL_MEMORY=536870912    # 512MB (game needs ~400MB for all.gsc + runtime)
        -sEXIT_RUNTIME=0
        -sFORCE_FILESYSTEM=1          # enable virtual filesystem
        -sASYNCIFY=1                  # allow sync C code to yield to browser
        -sASYNCIFY_STACK_SIZE=1048576  # 1MB — game has deep call stacks
        -pthread                       # enable pthreads / std::thread support
        -sPTHREAD_POOL_SIZE=16
        # -sMALLOC=emmalloc              # lightweight allocator — avoids mutex deadlock with asyncify
        -fexceptions                   # needed for std::thread error handling
        -g                              # debug symbols for readable stack traces
        -sASSERTIONS=2                  # extra runtime checks
    )

    # Preload game data if path provided
    if(GAME_DATA_DIR)
        message(STATUS "WASM: preloading game data from ${GAME_DATA_DIR}")
        # Essential startup files — mapped to virtual filesystem root
        # Use SHELL: prefix to prevent CMake from splitting on @
        target_link_options(Cossacks PRIVATE
            "SHELL:--preload-file ${GAME_DATA_DIR}/cew.dll@cew.dll"
            "SHELL:--preload-file ${GAME_DATA_DIR}/agew_1.pal@agew_1.pal"
            "SHELL:--preload-file ${GAME_DATA_DIR}/all.gsc@all.gsc"
            "SHELL:--preload-file ${GAME_DATA_DIR}/sound@Sound"
            "SHELL:--preload-file ${GAME_DATA_DIR}/sounds@Sounds"
            "SHELL:--preload-file ${GAME_DATA_DIR}/single.opt@single.opt"
            "SHELL:--preload-file ${GAME_DATA_DIR}/players@players"
            "SHELL:--preload-file ${GAME_DATA_DIR}/ai@ai"
        )
    else()
        message(WARNING "WASM: no GAME_DATA_DIR set. Build will succeed but game won't find data files at runtime. Set with: cmake -DGAME_DATA_DIR=/path/to/data")
    endif()

    set_target_properties(Cossacks PROPERTIES SUFFIX ".html")
elseif(WIN32)
    target_link_libraries(Cossacks PRIVATE
        kernel32 user32 gdi32 advapi32 shell32
        ole32 oleaut32 uuid ws2_32
        SDL2 SDL2_mixer
    )
else()
    # macOS/Linux
    target_link_directories(Cossacks PRIVATE ${SDL2_LIBRARY_DIRS} ${SDL2_MIXER_LIBRARY_DIRS})
    target_link_libraries(Cossacks PRIVATE ${SDL2_LIBRARIES} ${SDL2_MIXER_LIBRARIES})
endif()
