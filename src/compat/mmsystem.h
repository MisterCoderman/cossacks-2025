// Stub for <mmsystem.h> on non-Windows platforms
#ifndef _COMPAT_MMSYSTEM_H_
#define _COMPAT_MMSYSTEM_H_

#include "windows.h"

typedef UINT MMRESULT;
#define TIMERR_NOERROR 0
#define TIME_PERIODIC  0x0001

inline MMRESULT timeSetEvent(UINT delay, UINT res, void* proc, DWORD user, UINT event) {
    (void)delay; (void)res; (void)proc; (void)user; (void)event; return 0;
}
inline MMRESULT timeKillEvent(UINT id) { (void)id; return 0; }
inline MMRESULT timeBeginPeriod(UINT period) { (void)period; return 0; }
inline MMRESULT timeEndPeriod(UINT period) { (void)period; return 0; }
inline DWORD timeGetTime() { return 0; }

#endif
