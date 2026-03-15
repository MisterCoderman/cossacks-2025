// Network initialization part
//
#include "CommCore.h"
#include <iphlpapi.h>
#pragma comment(lib, "Iphlpapi.lib")

// ---------------------------------------------------------------------------------------------

BOOL CCommCore::InitNetwork()
{
	_log_message("InitNetwork()");
	fprintf(stderr, "[NET] InitNetwork start\n"); fflush(stderr);

	WSADATA WSAData;

	if(WSAStartup(MAKEWORD(2,2),&WSAData)!=0) {
		fprintf(stderr, "[NET] WSAStartup failed\n"); fflush(stderr);
		return FALSE;
	}

	if(!InitSocket()) {
		fprintf(stderr, "[NET] InitSocket failed\n"); fflush(stderr);
		return FALSE;
	}
	fprintf(stderr, "[NET] InitSocket OK\n"); fflush(stderr);

	if(!InitHost()) {
		fprintf(stderr, "[NET] InitHost failed\n"); fflush(stderr);
		return FALSE;
	}
	fprintf(stderr, "[NET] InitHost OK, addrs=%d\n", m_uAddrCount); fflush(stderr);

	return TRUE;
}

// ---------------------------------------------------------------------------------------------

BOOL CCommCore::InitHost()
{
	_log_message("InitHost()");

	m_uAddrCount = 0;

	ULONG bufLen = 15000;
	PIP_ADAPTER_ADDRESSES pAddresses = NULL;
	ULONG ret;

	do {
		pAddresses = (PIP_ADAPTER_ADDRESSES)malloc(bufLen);
		if (!pAddresses)
			return FALSE;

		ret = GetAdaptersAddresses(AF_INET,
			GAA_FLAG_SKIP_ANYCAST | GAA_FLAG_SKIP_MULTICAST |
			GAA_FLAG_SKIP_DNS_SERVER | GAA_FLAG_SKIP_FRIENDLY_NAME,
			NULL, pAddresses, &bufLen);

		if (ret == ERROR_BUFFER_OVERFLOW) {
			free(pAddresses);
			pAddresses = NULL;
		}
	} while (ret == ERROR_BUFFER_OVERFLOW);

	if (ret != NO_ERROR) {
		if (pAddresses) free(pAddresses);
		return FALSE;
	}

	for (PIP_ADAPTER_ADDRESSES pCurr = pAddresses;
		pCurr && m_uAddrCount < 8;
		pCurr = pCurr->Next)
	{
		if (pCurr->IfType == IF_TYPE_SOFTWARE_LOOPBACK) continue;
		if (pCurr->OperStatus != IfOperStatusUp) continue;

		for (PIP_ADAPTER_UNICAST_ADDRESS pUnicast = pCurr->FirstUnicastAddress;
			pUnicast && m_uAddrCount < 8;
			pUnicast = pUnicast->Next)
		{
			sockaddr_in* sa = (sockaddr_in*)pUnicast->Address.lpSockaddr;
			m_dwAddrList[m_uAddrCount++] = sa->sin_addr.s_addr;
		}
	}

	free(pAddresses);
	return (m_uAddrCount > 0);
}

// ---------------------------------------------------------------------------------------------

BOOL CCommCore::CloseNetwork()
{
	_log_message("CloseNetwork()");

	if(!CloseSocket())
		return FALSE;

	if(WSACleanup()==SOCKET_ERROR)
		return FALSE;

	return TRUE;
}

// ---------------------------------------------------------------------------------------------

BOOL CCommCore::InitSocket()
{
	_log_message("InitSocket()");

	m_DataSocket=socket(AF_INET,SOCK_DGRAM,IPPROTO_UDP);
	if(m_DataSocket==INVALID_SOCKET) {
		fprintf(stderr, "[NET] socket() failed\n"); fflush(stderr);
		return FALSE;
	}

	u_long	lArgP=0x01;
#ifdef _WIN32
	if(ioctlsocket(m_DataSocket,FIONBIO,&lArgP)==SOCKET_ERROR)
		return FALSE;
#else
	// On POSIX, use fcntl to set non-blocking
	int flags = fcntl(m_DataSocket, F_GETFL, 0);
	if (flags < 0 || fcntl(m_DataSocket, F_SETFL, flags | O_NONBLOCK) < 0) {
		fprintf(stderr, "[NET] fcntl non-blocking failed\n"); fflush(stderr);
		return FALSE;
	}
#endif

#ifdef _WIN32
	u_long	lMaxSize;
	int		iSizeOfMaxSize=sizeof(u_long);
	if(getsockopt(m_DataSocket,SOL_SOCKET,SO_MAX_MSG_SIZE,(char *)&lMaxSize,&iSizeOfMaxSize)==SOCKET_ERROR)
		return FALSE;
	m_uMaxMsgSize=(u_short)lMaxSize;
#else
	// SO_MAX_MSG_SIZE doesn't exist on POSIX; use a reasonable default for UDP
	m_uMaxMsgSize = 8192;
#endif

	sockaddr_in locaddr;

	locaddr.sin_family=AF_INET;
	locaddr.sin_addr.s_addr=htonl(INADDR_ANY);
	locaddr.sin_port=htons(DATA_PORT);

	if(bind(m_DataSocket,(sockaddr *)&locaddr,sizeof(sockaddr_in))==SOCKET_ERROR)
		return FALSE;

	return TRUE;
}

// ---------------------------------------------------------------------------------------------

BOOL CCommCore::CloseSocket()
{
	_log_message("CloseSocket()");

	if(closesocket(m_DataSocket)==SOCKET_ERROR)
		return FALSE;

	return TRUE;
}

// ---------------------------------------------------------------------------------------------
