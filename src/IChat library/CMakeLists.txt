# IChat - Chat/networking library (shared lib)
add_library(IChat SHARED
    Chat/chatCallbacks.c
    Chat/chatChannel.c
    Chat/chatHandlers.c
    Chat/chatMain.c
    Chat/chatSocket.c
    cs_chat.cpp
    darray.c
    GSC_ChatWindow.cpp
    hashtable.c
    IChat.cpp
    md5c.c
    nonport.c
    StdAfx.cpp
    ../IntExplorer\ library/ParseRQ.cpp
)

target_include_directories(IChat PUBLIC
    "${CMAKE_CURRENT_SOURCE_DIR}"
    "${CMAKE_CURRENT_SOURCE_DIR}/Chat"
)

target_include_directories(IChat SYSTEM PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/../Main executable"
    "${CMAKE_CURRENT_SOURCE_DIR}/../IntExplorer library"
)

target_link_libraries(IChat PRIVATE Cossacks)
