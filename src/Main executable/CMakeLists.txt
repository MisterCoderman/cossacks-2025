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
# Use -ld_classic linker to handle duplicate symbols (the exe and DLLs
# shared symbol definitions via dllexport/dllimport on Windows).
if(NOT WIN32)
    target_link_libraries(Cossacks PRIVATE IChat IntExplorer)
endif()

if(NOT WIN32)
    target_link_directories(Cossacks PRIVATE ${SDL2_LIBRARY_DIRS} ${SDL2_MIXER_LIBRARY_DIRS})
    target_link_libraries(Cossacks PRIVATE ${SDL2_LIBRARIES} ${SDL2_MIXER_LIBRARIES})
    # Allow duplicate symbols from static libs (were separate DLLs on Windows)
    if(APPLE)
        # No special linker flags needed
    endif()
endif()

# On Windows, link Windows system libraries
if(WIN32)
    target_link_libraries(Cossacks PRIVATE
        kernel32 user32 gdi32 advapi32 shell32
        ole32 oleaut32 uuid ws2_32
        SDL2 SDL2_mixer
    )
endif()
