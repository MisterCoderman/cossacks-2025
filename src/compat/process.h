// Stub for <process.h> on non-Windows platforms
#ifndef _COMPAT_PROCESS_H_
#define _COMPAT_PROCESS_H_

#include <unistd.h>

#define _getpid getpid

inline unsigned long _beginthreadex(void* security, unsigned stack_size,
    unsigned (*start_address)(void*), void* arglist,
    unsigned initflag, unsigned* thrdaddr) {
    (void)security; (void)stack_size; (void)start_address;
    (void)arglist; (void)initflag; (void)thrdaddr;
    return 0;
}

#endif
