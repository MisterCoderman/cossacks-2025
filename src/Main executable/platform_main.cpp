// Cross-platform entry point and stubs for non-Windows platforms
#if !defined(_WIN32)

#include "ddini.h"
#include "Dplay.h"
#include "Dplobby.h"

// Entry point: on non-Windows, provide main() that calls WinMain()
extern int PASCAL WinMain(HINSTANCE, HINSTANCE, LPSTR, int);

int main(int argc, char* argv[])
{
    // Combine argv into a single command line string for WinMain
    static char cmdLine[4096] = "";
    for (int i = 1; i < argc; i++) {
        if (i > 1) strcat(cmdLine, " ");
        strcat(cmdLine, argv[i]);
    }
    return WinMain(NULL, NULL, cmdLine, 0);
}

// Stub for DirectPlay lobby creation (DirectPlay is not available on non-Windows)
HRESULT WINAPI DirectPlayLobbyCreateA(LPGUID lpGuid, LPDIRECTPLAYLOBBYA* lplpDPLobby,
                                       IUnknown* lpUnk, LPVOID lpData, DWORD dwDataSize)
{
    (void)lpGuid; (void)lplpDPLobby; (void)lpUnk; (void)lpData; (void)dwDataSize;
    return E_NOTIMPL;
}

HRESULT WINAPI DirectPlayLobbyCreateW(LPGUID lpGuid, LPDIRECTPLAYLOBBY* lplpDPLobby,
                                       IUnknown* lpUnk, LPVOID lpData, DWORD dwDataSize)
{
    (void)lpGuid; (void)lplpDPLobby; (void)lpUnk; (void)lpData; (void)dwDataSize;
    return E_NOTIMPL;
}

// Stub implementation of CPinger (ICMP ping utility, precompiled .lib on Windows)
#include "Pinger.h"

CPinger::CPinger() : m_IcmpRawSocket(INVALID_SOCKET), m_bNetworkInitialized(FALSE),
    m_bTargetSet(FALSE), m_dwLastSendTime(0), m_iSeqCounter(0),
    cReplies(0), cTimeOuts(0), cTotal(0) {
    memset(&m_TargetAddr, 0, sizeof(m_TargetAddr));
    memset(&m_IcmpHeader, 0, sizeof(m_IcmpHeader));
}
CPinger::~CPinger() { DoneNetwork(); }
BOOL CPinger::InitNetwork() { m_bNetworkInitialized = TRUE; return TRUE; }
BOOL CPinger::DoneNetwork() { m_bNetworkInitialized = FALSE; return TRUE; }
BOOL CPinger::SetTargetName(LPCSTR lpcszTargetName) { (void)lpcszTargetName; return FALSE; }
BOOL CPinger::GetStatistic(BYTE* lpcReplies, BYTE* lpcTotal) {
    if (lpcReplies) *lpcReplies = cReplies;
    if (lpcTotal) *lpcTotal = cTotal;
    return TRUE;
}
BOOL CPinger::SendEcho() { cTotal++; return FALSE; }
BOOL CPinger::RecvEcho() { return FALSE; }
USHORT CPinger::checksum(USHORT* buffer, int size) { (void)buffer; (void)size; return 0; }

#endif // !_WIN32
