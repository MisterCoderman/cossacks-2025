# CommCore - UDP networking library (static lib)
add_library(CommCore STATIC
    CommCore.cpp
    CommInet.cpp
    CommInit.cpp
    CommPeers.cpp
    CommPing.cpp
    CommQueue.cpp
    CommRaw.cpp
    CommRecv.cpp
    CommServerList.cpp
)

target_include_directories(CommCore PUBLIC
    "${CMAKE_CURRENT_SOURCE_DIR}"
)
