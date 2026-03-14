// Stub for <windowsx.h> on non-Windows platforms
#ifndef _COMPAT_WINDOWSX_H_
#define _COMPAT_WINDOWSX_H_

#include "windows.h"

#define GET_X_LPARAM(lp) ((int)(short)LOWORD(lp))
#define GET_Y_LPARAM(lp) ((int)(short)HIWORD(lp))
#define GET_WHEEL_DELTA_WPARAM(wParam) ((short)HIWORD(wParam))

#endif // _COMPAT_WINDOWSX_H_
