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

// Real implementation using getifaddrs on macOS/Linux
// IMPORTANT: Reset packing to default — game headers use #pragma pack(1)
// which would break system struct layouts (e.g., struct ifaddrs offsets)
#pragma pack(push, 8)
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <ifaddrs.h>
#include <net/if.h>
#pragma pack(pop)

// Static storage for adapter data (game only calls this once)
static struct {
    IP_ADAPTER_ADDRESSES adapters[16];
    IP_ADAPTER_UNICAST_ADDRESS unicast[16];
    struct sockaddr_in addrs[16];
    int count;
    bool initialized;
} _compat_adapters = {};

inline DWORD GetAdaptersAddresses(ULONG family, ULONG flags, void* reserved,
                                  PIP_ADAPTER_ADDRESSES addrs, ULONG* size) {
    (void)flags; (void)reserved;
    if (!addrs || !size) return ERROR_BUFFER_OVERFLOW;

    if (!_compat_adapters.initialized) {
        _compat_adapters.initialized = true;
        _compat_adapters.count = 0;

        struct ifaddrs* ifap = NULL;
        int gret = getifaddrs(&ifap);
        fprintf(stderr, "[NET] getifaddrs returned %d, ifap=%p\n", gret, (void*)ifap); fflush(stderr);
        if (gret == 0 && ifap) {
            int idx = 0;
            for (struct ifaddrs* ifa = ifap; ifa; ifa = ifa->ifa_next) {
                idx++;
                fprintf(stderr, "[NET]   if[%d] name=%s flags=0x%x addr=%p",
                    idx, ifa->ifa_name ? ifa->ifa_name : "?", ifa->ifa_flags, (void*)ifa->ifa_addr);
                if (ifa->ifa_addr) {
                    fprintf(stderr, " sa_family=%d", ifa->ifa_addr->sa_family);
                    if (ifa->ifa_addr->sa_family == AF_INET) {
                        struct sockaddr_in* sin = (struct sockaddr_in*)ifa->ifa_addr;
                        fprintf(stderr, " ip=%s", inet_ntoa(sin->sin_addr));
                    }
                }
                fprintf(stderr, "\n"); fflush(stderr);

                if (!ifa->ifa_addr) continue;
                if (ifa->ifa_addr->sa_family != AF_INET) continue;
                if (!(ifa->ifa_flags & IFF_UP)) continue;
                if (ifa->ifa_flags & IFF_LOOPBACK) continue;

                if (_compat_adapters.count < 16) {
                    int i = _compat_adapters.count;
                    memset(&_compat_adapters.adapters[i], 0, sizeof(IP_ADAPTER_ADDRESSES));
                    memset(&_compat_adapters.unicast[i], 0, sizeof(IP_ADAPTER_UNICAST_ADDRESS));
                    memcpy(&_compat_adapters.addrs[i], ifa->ifa_addr, sizeof(struct sockaddr_in));

                    _compat_adapters.adapters[i].IfType = 0;
                    _compat_adapters.adapters[i].OperStatus = IfOperStatusUp;
                    _compat_adapters.adapters[i].FirstUnicastAddress = &_compat_adapters.unicast[i];
                    _compat_adapters.unicast[i].Address.lpSockaddr = (struct sockaddr*)&_compat_adapters.addrs[i];
                    _compat_adapters.unicast[i].Address.iSockaddrLength = sizeof(struct sockaddr_in);
                    _compat_adapters.unicast[i].Next = NULL;

                    if (i > 0) _compat_adapters.adapters[i-1].Next = &_compat_adapters.adapters[i];
                    _compat_adapters.adapters[i].Next = NULL;
                    _compat_adapters.count++;
                }
            }
            freeifaddrs(ifap);
        }
        fprintf(stderr, "[NET] Found %d adapter addresses\n", _compat_adapters.count); fflush(stderr);
    }

    if (_compat_adapters.count == 0) return ERROR_BUFFER_OVERFLOW;

    size_t needed = _compat_adapters.count * sizeof(IP_ADAPTER_ADDRESSES);
    if (*size < needed) {
        *size = (ULONG)needed;
        return ERROR_BUFFER_OVERFLOW;
    }

    memcpy(addrs, &_compat_adapters.adapters[0], sizeof(IP_ADAPTER_ADDRESSES));
    // Fix pointers — they point into static storage, caller uses them directly
    *addrs = _compat_adapters.adapters[0];
    return NO_ERROR;
}

#endif
