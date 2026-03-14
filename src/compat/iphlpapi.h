// Stub for <iphlpapi.h> on non-Windows platforms
#ifndef _COMPAT_IPHLPAPI_H_
#define _COMPAT_IPHLPAPI_H_

#include "windows.h"
#include <sys/socket.h>

// IP_ADAPTER_INFO stub
typedef struct _IP_ADDR_STRING {
    struct _IP_ADDR_STRING* Next;
    char IpAddress[16];
    char IpMask[16];
    DWORD Context;
} IP_ADDR_STRING, *PIP_ADDR_STRING;

typedef struct _IP_ADAPTER_INFO {
    struct _IP_ADAPTER_INFO* Next;
    DWORD ComboIndex;
    char AdapterName[260];
    char Description[132];
    UINT AddressLength;
    BYTE Address[8];
    DWORD Index;
    UINT Type;
    UINT DhcpEnabled;
    PIP_ADDR_STRING CurrentIpAddress;
    IP_ADDR_STRING IpAddressList;
    IP_ADDR_STRING GatewayList;
    IP_ADDR_STRING DhcpServer;
} IP_ADAPTER_INFO, *PIP_ADAPTER_INFO;

// IP_ADAPTER_UNICAST_ADDRESS stub
typedef struct _IP_ADAPTER_UNICAST_ADDRESS {
    struct _IP_ADAPTER_UNICAST_ADDRESS* Next;
    struct sockaddr* Address_lpSockaddr;
    int Address_iSockaddrLength;
    // Simplified accessor
    struct {
        struct sockaddr* lpSockaddr;
        int iSockaddrLength;
    } Address;
} IP_ADAPTER_UNICAST_ADDRESS, *PIP_ADAPTER_UNICAST_ADDRESS;

// IP_ADAPTER_ADDRESSES stub
typedef struct _IP_ADAPTER_ADDRESSES {
    struct _IP_ADAPTER_ADDRESSES* Next;
    char AdapterName[260];
    PIP_ADAPTER_UNICAST_ADDRESS FirstUnicastAddress;
    DWORD IfType;
    DWORD OperStatus;
    DWORD Flags;
} IP_ADAPTER_ADDRESSES, *PIP_ADAPTER_ADDRESSES;

#define ERROR_BUFFER_OVERFLOW 111L
#define GAA_FLAG_SKIP_ANYCAST       0x0002
#define GAA_FLAG_SKIP_MULTICAST     0x0004
#define GAA_FLAG_SKIP_DNS_SERVER    0x0008
#define GAA_FLAG_SKIP_FRIENDLY_NAME 0x0020
#define IF_TYPE_SOFTWARE_LOOPBACK   24
#define IfOperStatusUp              1

// AF_INET is provided by sys/socket.h

inline DWORD GetAdaptersInfo(PIP_ADAPTER_INFO info, ULONG* size) {
    (void)info; (void)size; return ERROR_BUFFER_OVERFLOW;
}

inline DWORD GetAdaptersAddresses(ULONG family, ULONG flags, void* reserved,
                                  PIP_ADAPTER_ADDRESSES addrs, ULONG* size) {
    (void)family; (void)flags; (void)reserved; (void)addrs; (void)size;
    return ERROR_BUFFER_OVERFLOW;
}

#endif
