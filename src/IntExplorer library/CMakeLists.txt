# IntExplorer - Internet explorer UI library
set(INTEXPLORER_SOURCES
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

if(WIN32)
    add_library(IntExplorer SHARED ${INTEXPLORER_SOURCES})
    target_link_libraries(IntExplorer PRIVATE Cossacks IChat)
else()
    add_library(IntExplorer STATIC ${INTEXPLORER_SOURCES})
endif()

target_include_directories(IntExplorer PUBLIC
    "${CMAKE_CURRENT_SOURCE_DIR}"
)

target_include_directories(IntExplorer SYSTEM PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/../Main executable"
)
