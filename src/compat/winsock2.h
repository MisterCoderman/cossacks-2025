// Stub for <Winsock2.h> on non-Windows platforms
// Maps WinSock types to BSD sockets
#ifndef _COMPAT_WINSOCK2_H_
#define _COMPAT_WINSOCK2_H_

#include "windows.h"

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>

// WinSock types mapped to BSD
typedef int SOCKET;
#define INVALID_SOCKET  ((SOCKET)-1)
#define SOCKET_ERROR    (-1)

// WinSock compatibility
#define closesocket close
#define ioctlsocket ioctl

typedef struct WSAData {
    WORD wVersion;
    WORD wHighVersion;
    char szDescription[257];
    char szSystemStatus[129];
} WSADATA, *LPWSADATA;

typedef unsigned char UCHAR;

// WSASocket stub — map to regular socket()
#define WSASocket(af, type, proto, info, group, flags) socket(af, type, proto)

inline int WSAStartup(WORD version, LPWSADATA data) { (void)version; (void)data; return 0; }
inline int WSACleanup() { return 0; }
inline int WSAGetLastError() { return errno; }

#define InetPton inet_pton
#define InetNtop inet_ntop
#define NO_ERROR 0
#define SD_BOTH SHUT_RDWR
#define WSAEWOULDBLOCK EWOULDBLOCK
#define WSAEINPROGRESS EINPROGRESS
typedef struct hostent HOSTENT;

#ifndef FIONBIO
#include <sys/ioctl.h>
#endif

// SO_MAX_MSG_SIZE is Windows-only. Define a value that getsockopt won't error on.
// On POSIX, we define it as SO_SNDBUF as a rough approximation for parsing purposes.
#ifndef SO_MAX_MSG_SIZE
#define SO_MAX_MSG_SIZE SO_SNDBUF
#endif

// On POSIX, getsockopt uses socklen_t* for the length parameter.
// Provide a wrapper to accept int* as Windows code expects.
#include <sys/ioctl.h>

// On POSIX, getsockopt uses socklen_t* but Windows code passes int*.
// Provide an overload that accepts int* for the last parameter.
#ifdef __cplusplus
inline int _compat_getsockopt(int sockfd, int level, int optname, void* optval, int* optlen) {
    socklen_t len = (socklen_t)*optlen;
    int result = ::getsockopt(sockfd, level, optname, optval, &len);
    *optlen = (int)len;
    return result;
}
#define getsockopt _compat_getsockopt

inline int _compat_recvfrom(int sockfd, void* buf, size_t len, int flags,
                           struct sockaddr* src_addr, int* addrlen) {
    socklen_t slen = addrlen ? (socklen_t)*addrlen : 0;
    int result = ::recvfrom(sockfd, buf, len, flags, src_addr, addrlen ? &slen : nullptr);
    if (addrlen) *addrlen = (int)slen;
    return (int)result;
}
#define recvfrom _compat_recvfrom

inline int _compat_accept(int sockfd, struct sockaddr* addr, int* addrlen) {
    socklen_t slen = addrlen ? (socklen_t)*addrlen : 0;
    int result = ::accept(sockfd, addr, addrlen ? &slen : nullptr);
    if (addrlen) *addrlen = (int)slen;
    return result;
}
#define accept _compat_accept

inline int _compat_ioctlsocket(int fd, unsigned long cmd, u_long* argp) {
    return ioctl(fd, (int)cmd, argp);
}
#undef ioctlsocket
#define ioctlsocket _compat_ioctlsocket
#endif

// u_* types (may already be defined on some systems)
#ifndef _U_SHORT_DEFINED
typedef unsigned short u_short;
#define _U_SHORT_DEFINED
#endif

#ifndef _U_LONG_DEFINED
typedef unsigned long u_long;
#define _U_LONG_DEFINED
#endif

#ifndef _U_CHAR_DEFINED
typedef unsigned char u_char;
#define _U_CHAR_DEFINED
#endif

#endif // _COMPAT_WINSOCK2_H_
