// Stub for <crtdbg.h> on non-Windows platforms
#ifndef _COMPAT_CRTDBG_H_
#define _COMPAT_CRTDBG_H_

#define _CrtSetDbgFlag(x) ((void)0)
#define _CrtDumpMemoryLeaks() ((void)0)
#define _CRTDBG_ALLOC_MEM_DF 0
#define _CRTDBG_LEAK_CHECK_DF 0
#define _CrtSetReportMode(a,b) ((void)0)
#define _CRT_WARN 0
#define _CRTDBG_MODE_DEBUG 0

#endif
