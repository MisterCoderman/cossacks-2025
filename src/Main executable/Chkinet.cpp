#include "ddini.h"
#ifdef _WIN32
#include <winsock2.h>
#else
#include <netdb.h>
#endif
bool CheckInternet(){
	return gethostbyname("peerchat.gamespy.com")!=NULL;
};