# IChat - Chat/networking library
# On Windows this is a DLL; on other platforms use STATIC to avoid
# circular link dependencies between the exe and its DLLs.
set(ICHAT_SOURCES
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

if(WIN32)
    add_library(IChat SHARED ${ICHAT_SOURCES})
    target_link_libraries(IChat PRIVATE Cossacks)
else()
    add_library(IChat STATIC ${ICHAT_SOURCES})
endif()

target_include_directories(IChat PUBLIC
    "${CMAKE_CURRENT_SOURCE_DIR}"
    "${CMAKE_CURRENT_SOURCE_DIR}/Chat"
)

target_include_directories(IChat SYSTEM PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/../Main executable"
    "${CMAKE_CURRENT_SOURCE_DIR}/../IntExplorer library"
)
