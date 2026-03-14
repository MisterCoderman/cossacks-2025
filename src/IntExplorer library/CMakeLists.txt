# IntExplorer - Internet explorer UI library (shared lib)
add_library(IntExplorer SHARED
    Bmptool.cpp
    DataXchange.cpp
    DString.cpp
    Explore.cpp
    FnDefine.cpp
    GameOnMap.cpp
    IntExplorer.cpp
    ParseRQ.cpp
    PingEngine.cpp
    StdAfx.cpp
)

target_include_directories(IntExplorer PUBLIC
    "${CMAKE_CURRENT_SOURCE_DIR}"
)

target_include_directories(IntExplorer SYSTEM PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/../Main executable"
)

target_link_libraries(IntExplorer PRIVATE Cossacks IChat)
