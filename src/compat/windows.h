// ==============================================
// Windows API compatibility layer for non-Windows platforms
// Provides type definitions and stubs so the codebase can be
// parsed by CLion/Clang on macOS/Linux.
// This is NOT a full Windows API implementation.
// ==============================================
#ifndef _COMPAT_WINDOWS_H_
#define _COMPAT_WINDOWS_H_

#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <time.h>

// ---- Basic types ----
typedef int                 BOOL;
typedef unsigned char       BYTE;
typedef unsigned short      WORD;
typedef uint32_t            DWORD;
typedef int32_t             LONG;
typedef uint32_t            ULONG;
typedef int                 INT;
typedef unsigned int        UINT;
typedef short               SHORT;
typedef unsigned short      USHORT;
typedef char                CHAR;
typedef wchar_t             WCHAR;
typedef float               FLOAT;
typedef long long           LONGLONG;
typedef unsigned long long  ULONGLONG;
typedef BYTE                byte;
typedef void                VOID;
typedef unsigned int        u_int;
typedef unsigned long       u_long;
typedef unsigned short      u_short;
typedef unsigned char       u_char;

#ifndef TRUE
#define TRUE  1
#endif
#ifndef FALSE
#define FALSE 0
#endif

#ifndef NULL
#ifdef __cplusplus
#define NULL nullptr
#else
#define NULL ((void*)0)
#endif
#endif

// ---- Pointer types ----
typedef char*               LPSTR;
typedef const char*         LPCSTR;
typedef wchar_t*            LPWSTR;
typedef const wchar_t*      LPCWSTR;
typedef void*               LPVOID;
typedef const void*         LPCVOID;
typedef BYTE*               LPBYTE;
typedef DWORD*              LPDWORD;
typedef WORD*               LPWORD;
typedef LONG*               LPLONG;
typedef BOOL*               LPBOOL;

// ---- Handle types ----
typedef void*               HANDLE;
typedef void*               HWND;
typedef void*               HINSTANCE;
typedef void*               HMODULE;
typedef void*               HDC;
typedef void*               HBRUSH;
typedef void*               HPEN;
typedef void*               HFONT;
typedef void*               HBITMAP;
typedef void*               HICON;
typedef void*               HCURSOR;
typedef void*               HMENU;
typedef void*               HRGN;
typedef void*               HPALETTE;
typedef void*               HGLOBAL;
typedef void*               HLOCAL;
typedef void*               HKEY;

// ---- Message types ----
typedef uintptr_t           WPARAM;
typedef intptr_t            LPARAM;
typedef intptr_t            LRESULT;

// ---- COM/OLE types ----
typedef int32_t             HRESULT;
typedef uint32_t            COLORREF;
typedef DWORD               FOURCC;

#define S_OK                ((HRESULT)0)
#define S_FALSE             ((HRESULT)1)
#define E_FAIL              ((HRESULT)0x80004005L)
#define E_NOINTERFACE       ((HRESULT)0x80004002L)
#define E_OUTOFMEMORY       ((HRESULT)0x8007000EL)
#define E_INVALIDARG        ((HRESULT)0x80070057L)
#define E_NOTIMPL           ((HRESULT)0x80004001L)
#define SUCCEEDED(hr)       (((HRESULT)(hr)) >= 0)
#define FAILED(hr)          (((HRESULT)(hr)) < 0)
#define MAKE_HRESULT(sev,fac,code) \
    ((HRESULT)(((unsigned long)(sev)<<31)|((unsigned long)(fac)<<16)|((unsigned long)(code))))

// ---- GUID ----
typedef struct _GUID {
    uint32_t Data1;
    uint16_t Data2;
    uint16_t Data3;
    uint8_t  Data4[8];
} GUID, IID, CLSID;

typedef GUID*               LPGUID;
typedef const GUID&         REFGUID;
typedef const GUID&         REFIID;
typedef const GUID&         REFCLSID;

#ifdef __cplusplus
inline bool operator==(const GUID& a, const GUID& b) {
    return memcmp(&a, &b, sizeof(GUID)) == 0;
}
inline bool operator!=(const GUID& a, const GUID& b) {
    return !(a == b);
}
#endif

#define DEFINE_GUID(name, l, w1, w2, b1, b2, b3, b4, b5, b6, b7, b8) \
    static const GUID name = { l, w1, w2, { b1, b2, b3, b4, b5, b6, b7, b8 } }

// ---- COM interface macros ----
#define STDMETHOD(method)       virtual HRESULT method
#define STDMETHOD_(type,method) virtual type method
#define STDMETHODIMP            HRESULT
#define STDMETHODIMP_(type)     type
#define THIS_
#define THIS                    void
#define PURE                    = 0
#define DECLARE_INTERFACE(iface) struct iface
#define DECLARE_INTERFACE_(iface, baseiface) struct iface : public baseiface

struct IUnknown {
    virtual HRESULT QueryInterface(REFIID riid, void** ppvObject) = 0;
    virtual ULONG AddRef() = 0;
    virtual ULONG Release() = 0;
    virtual ~IUnknown() {}
};

// ---- Calling conventions (no-op on non-Windows) ----
#define WINAPI
#define APIENTRY
#define CALLBACK
#define PASCAL
#define FAR
#define NEAR
#define _cdecl
#define __cdecl
#define __stdcall
#define __fastcall

// ---- __declspec stubs ----
#define __declspec(x)
#define _declspec(x)

// ---- MSVC-specific ----
// Note: We do NOT define _MSC_VER here as it confuses system headers
// (e.g., inttypes.h checks for MSVC version). Code that needs to check
// for MSVC should use: #if defined(_MSC_VER) && _MSC_VER > 0

// MSVC type aliases
typedef char small;
#define __forceinline inline __attribute__((always_inline))

// ---- Structures ----
typedef struct tagRECT {
    LONG left;
    LONG top;
    LONG right;
    LONG bottom;
} RECT, *LPRECT;
typedef const RECT* LPCRECT;

typedef struct tagPOINT {
    LONG x;
    LONG y;
} POINT, *LPPOINT;

typedef struct tagSIZE {
    LONG cx;
    LONG cy;
} SIZE, *LPSIZE;

typedef struct tagMSG {
    HWND   hwnd;
    UINT   message;
    WPARAM wParam;
    LPARAM lParam;
    DWORD  time;
    POINT  pt;
} MSG, *LPMSG;

// ---- WAVEFORMATEX ----
typedef struct tWAVEFORMATEX {
    WORD  wFormatTag;
    WORD  nChannels;
    DWORD nSamplesPerSec;
    DWORD nAvgBytesPerSec;
    WORD  nBlockAlign;
    WORD  wBitsPerSample;
    WORD  cbSize;
} WAVEFORMATEX, *LPWAVEFORMATEX;
typedef const WAVEFORMATEX* LPCWAVEFORMATEX;

#define WAVE_FORMAT_PCM 1

// ---- Bitmap structures ----
#define BI_RGB 0L
#define BI_RLE8 1L
#define BI_RLE4 2L

#pragma pack(push, 2)
typedef struct tagBITMAPFILEHEADER {
    WORD  bfType;
    DWORD bfSize;
    WORD  bfReserved1;
    WORD  bfReserved2;
    DWORD bfOffBits;
} BITMAPFILEHEADER;

typedef struct tagBITMAPINFOHEADER {
    DWORD biSize;
    LONG  biWidth;
    LONG  biHeight;
    WORD  biPlanes;
    WORD  biBitCount;
    DWORD biCompression;
    DWORD biSizeImage;
    LONG  biXPelsPerMeter;
    LONG  biYPelsPerMeter;
    DWORD biClrUsed;
    DWORD biClrImportant;
} BITMAPINFOHEADER;

typedef struct tagRGBQUAD {
    BYTE rgbBlue;
    BYTE rgbGreen;
    BYTE rgbRed;
    BYTE rgbReserved;
} RGBQUAD;
#pragma pack(pop)

// ---- Color ----
#define RGB(r,g,b) ((COLORREF)(((BYTE)(r)|((WORD)((BYTE)(g))<<8))|(((DWORD)(BYTE)(b))<<16)))
#define GetRValue(rgb) ((BYTE)(rgb))
#define GetGValue(rgb) ((BYTE)(((WORD)(rgb)) >> 8))
#define GetBValue(rgb) ((BYTE)((rgb) >> 16))

// ---- Common macros ----
#define LOWORD(l) ((WORD)((DWORD)(l) & 0xffff))
#define HIWORD(l) ((WORD)((DWORD)(l) >> 16))
#define LOBYTE(w) ((BYTE)((DWORD)(w) & 0xff))
#define HIBYTE(w) ((BYTE)((DWORD)(w) >> 8))
#define MAKELONG(a, b) ((LONG)(((WORD)(a)) | ((DWORD)((WORD)(b))) << 16))
#define MAKEWORD(a, b) ((WORD)(((BYTE)(a)) | ((WORD)((BYTE)(b))) << 8))
#define max(a,b) (((a) > (b)) ? (a) : (b))
#define min(a,b) (((a) < (b)) ? (a) : (b))
#define ZeroMemory(dest, size) memset((dest), 0, (size))
#define CopyMemory(dest, src, size) memcpy((dest), (src), (size))
#define MoveMemory(dest, src, size) memmove((dest), (src), (size))
#define FillMemory(dest, size, fill) memset((dest), (fill), (size))

// ---- String macros ----
#define lstrlen strlen
#define lstrcpy strcpy
#define lstrcpyn strncpy
#define lstrcmp strcmp
#define lstrcmpi strcasecmp
#define lstrcat strcat
#define wsprintf sprintf
#define wvsprintf vsprintf
// ---- MSVC secure CRT ----
#ifndef _TRUNCATE
#define _TRUNCATE ((size_t)-1)
#endif
#ifndef strncpy_s
inline int strncpy_s(char* dest, size_t destsz, const char* src, size_t count) {
    if (!dest || destsz == 0) return -1;
    if (count == _TRUNCATE || count >= destsz) count = destsz - 1;
    strncpy(dest, src ? src : "", count);
    dest[count] = 0;
    return 0;
}
#endif
#ifndef sprintf_s
#define sprintf_s(buf, sz, ...) snprintf(buf, sz, __VA_ARGS__)
#endif
#ifndef _stprintf_s
#define _stprintf_s sprintf_s
#endif
#ifndef strcpy_s
inline int strcpy_s(char* dest, size_t destsz, const char* src) {
    if (!dest || destsz == 0) return -1;
    strncpy(dest, src ? src : "", destsz - 1);
    dest[destsz - 1] = 0;
    return 0;
}
#endif

// ---- MSVC CRT name mappings ----
#define _utimbuf utimbuf
#define _utime utime
#define stricmp strcasecmp
#define _stricmp strcasecmp
#define strnicmp strncasecmp
#define _strnicmp strncasecmp
#define _snprintf snprintf
#define _vsnprintf vsnprintf

// strupr / strlwr - MSVC extensions
#include <ctype.h>
inline char* strupr(char* s) { for (char* p = s; *p; p++) *p = toupper(*p); return s; }
inline char* strlwr(char* s) { for (char* p = s; *p; p++) *p = tolower(*p); return s; }
#define _strupr strupr
#define _strlwr strlwr
#define itoa(val, buf, radix) sprintf(buf, "%d", val)
#define _itoa(val, buf, radix) sprintf(buf, "%d", val)

// ---- File and system stubs ----
#define MAX_PATH 260
#define INVALID_HANDLE_VALUE ((HANDLE)(intptr_t)-1)

// ---- Window messages (minimal set for parsing) ----
#define WM_CREATE       0x0001
#define WM_DESTROY      0x0002
#define WM_MOVE         0x0003
#define WM_SIZE         0x0005
#define WM_ACTIVATE     0x0006
#define WM_SETFOCUS     0x0007
#define WM_KILLFOCUS    0x0008
#define WM_CLOSE        0x0010
#define WM_QUIT         0x0012
#define WM_PAINT        0x000F
#define WM_ERASEBKGND   0x0014
#define WM_KEYDOWN      0x0100
#define WM_KEYUP        0x0101
#define WM_CHAR         0x0102
#define WM_SYSKEYDOWN   0x0104
#define WM_SYSKEYUP     0x0105
#define WM_COMMAND      0x0111
#define WM_TIMER        0x0113
#define WM_MOUSEMOVE    0x0200
#define WM_LBUTTONDOWN  0x0201
#define WM_LBUTTONUP    0x0202
#define WM_RBUTTONDOWN  0x0204
#define WM_RBUTTONUP    0x0205
#define WM_MBUTTONDOWN  0x0207
#define WM_MBUTTONUP    0x0208
#define WM_MOUSEWHEEL   0x020A
#define WM_USER         0x0400

// ---- Window styles ----
#define WS_OVERLAPPED   0x00000000L
#define WS_POPUP        0x80000000L
#define WS_CHILD        0x40000000L
#define WS_VISIBLE      0x10000000L
#define WS_CAPTION      0x00C00000L
#define WS_BORDER       0x00800000L
#define WS_SYSMENU      0x00080000L
#define WS_MINIMIZEBOX  0x00020000L
#define WS_MAXIMIZEBOX  0x00010000L
#define WS_OVERLAPPEDWINDOW (WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX | WS_MAXIMIZEBOX)

// ---- Virtual key codes ----
#define VK_BACK         0x08
#define VK_TAB          0x09
#define VK_RETURN       0x0D
#define VK_SHIFT        0x10
#define VK_CONTROL      0x11
#define VK_MENU         0x12
#define VK_PAUSE        0x13
#define VK_ESCAPE       0x1B
#define VK_SPACE        0x20
#define VK_LEFT         0x25
#define VK_UP           0x26
#define VK_RIGHT        0x27
#define VK_DOWN         0x28
#define VK_DELETE       0x2E
#define VK_F1           0x70
#define VK_F2           0x71
#define VK_F3           0x72
#define VK_F4           0x73
#define VK_F5           0x74
#define VK_F6           0x75
#define VK_F7           0x76
#define VK_F8           0x77
#define VK_F9           0x78
#define VK_F10          0x79
#define VK_F11          0x7A
#define VK_F12          0x7B
#define VK_SCROLL       0x91
#define VK_NUMLOCK      0x90

// ---- MessageBox ----
#define MB_OK               0x00000000L
#define MB_OKCANCEL         0x00000001L
#define MB_YESNO            0x00000004L
#define MB_ICONERROR        0x00000010L
#define MB_ICONWARNING      0x00000030L
#define MB_ICONINFORMATION  0x00000040L
#define MB_TOPMOST          0x00040000L
#define IDOK     1
#define IDCANCEL 2
#define IDYES    6
#define IDNO     7

// ---- Registry stubs ----
#define HKEY_CURRENT_USER   ((HKEY)(intptr_t)0x80000001)
#define HKEY_LOCAL_MACHINE  ((HKEY)(intptr_t)0x80000002)
#define REG_SZ              1
#define REG_DWORD           4
#define KEY_READ            0x20019
#define KEY_WRITE           0x20006
#define ERROR_SUCCESS       0L

// ---- GetTickCount ----
#ifdef __cplusplus
extern "C" {
#endif

// These are stubs - they won't link but allow CLion to parse
inline DWORD GetTickCount() {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (DWORD)(ts.tv_sec * 1000 + ts.tv_nsec / 1000000);
}
inline void Sleep(DWORD ms) { usleep(ms * 1000); }
inline int MessageBoxA(HWND hw, LPCSTR text, LPCSTR caption, UINT type) {
    (void)hw; (void)text; (void)caption; (void)type; return 0;
}
#define MessageBox MessageBoxA

inline HMODULE GetModuleHandleA(LPCSTR name) { (void)name; return NULL; }
#define GetModuleHandle GetModuleHandleA

inline DWORD GetModuleFileNameA(HMODULE hm, LPSTR buf, DWORD size) {
    (void)hm; (void)buf; (void)size; return 0;
}
#define GetModuleFileName GetModuleFileNameA

inline void GetSystemDirectory(LPSTR buf, UINT size) { (void)buf; (void)size; }
inline DWORD GetLogicalDriveStringsA(DWORD size, LPSTR buf) { (void)size; (void)buf; return 0; }
#define GetLogicalDriveStrings GetLogicalDriveStringsA
// GetCurrentDirectory, SetCurrentDirectory, CreateFile, ReadFile, WriteFile,
// SetFilePointer, GetFileSize, CloseHandle, CreateFileMapping, MapViewOfFile,
// UnmapViewOfFile, FindFirstFile, FindNextFile, FindClose, GetFileAttributes
// are implemented in compat_file_io.h (included at end of this file)

// ---- Critical section stubs ----
// Critical sections implemented via pthread_mutex
#include <pthread.h>
typedef struct _CRITICAL_SECTION {
    pthread_mutex_t mutex;
} CRITICAL_SECTION, *LPCRITICAL_SECTION;

inline void InitializeCriticalSection(LPCRITICAL_SECTION cs) {
    pthread_mutexattr_t attr;
    pthread_mutexattr_init(&attr);
    pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE);
    pthread_mutex_init(&cs->mutex, &attr);
    pthread_mutexattr_destroy(&attr);
}
inline void DeleteCriticalSection(LPCRITICAL_SECTION cs) { pthread_mutex_destroy(&cs->mutex); }
inline void EnterCriticalSection(LPCRITICAL_SECTION cs) { pthread_mutex_lock(&cs->mutex); }
inline void LeaveCriticalSection(LPCRITICAL_SECTION cs) { pthread_mutex_unlock(&cs->mutex); }

// ---- Threading stubs ----
#ifndef WAIT_OBJECT_0
#define WAIT_OBJECT_0 0
#endif
#ifndef INFINITE
#define INFINITE 0xFFFFFFFF
#endif
// CreateThread using pthreads
typedef DWORD (*_compat_thread_proc)(LPVOID);
struct _compat_thread_info {
    _compat_thread_proc proc;
    LPVOID param;
    pthread_t thread;
};
inline void* _compat_thread_wrapper(void* arg) {
    _compat_thread_info* info = (_compat_thread_info*)arg;
    info->proc(info->param);
    return NULL;
}
inline HANDLE CreateThread(void* attr, size_t stack, void* func, void* param, DWORD flags, DWORD* id) {
    (void)attr; (void)stack; (void)flags;
    _compat_thread_info* info = new _compat_thread_info;
    info->proc = (_compat_thread_proc)func;
    info->param = param;
    if (pthread_create(&info->thread, NULL, _compat_thread_wrapper, info) != 0) {
        delete info;
        return NULL;
    }
    if (id) *id = (DWORD)(uintptr_t)info;
    return (HANDLE)info;
}
inline DWORD WaitForSingleObject(HANDLE h, DWORD ms) {
    (void)ms; // timeout not implemented
    if (!h) return WAIT_OBJECT_0;
    _compat_thread_info* info = (_compat_thread_info*)h;
    pthread_join(info->thread, NULL);
    return WAIT_OBJECT_0;
}
// CloseHandle — implemented in compat_file_io.h
#define INFINITE 0xFFFFFFFF
#define WAIT_OBJECT_0 0

// ---- File I/O ----
// CreateFileA is implemented in compat_file_io.h
#define GENERIC_READ    0x80000000L
#define GENERIC_WRITE   0x40000000L
#define OPEN_EXISTING   3
#define CREATE_ALWAYS   2
#define FILE_SHARE_READ  0x00000001
#define FILE_SHARE_WRITE 0x00000002
#define FILE_BEGIN       0
#define FILE_CURRENT     1
#define FILE_END         2
#define CREATE_NEW       1
#define OPEN_ALWAYS      4
#define TRUNCATE_EXISTING 5

// ReadFile, WriteFile, SetFilePointer, GetFileSize — implemented in compat_file_io.h

// ---- FILETIME ----
typedef struct _FILETIME {
    DWORD dwLowDateTime;
    DWORD dwHighDateTime;
} FILETIME;

// ---- FindFile stubs ----
typedef struct _WIN32_FIND_DATAA {
    DWORD    dwFileAttributes;
    FILETIME ftCreationTime;
    FILETIME ftLastAccessTime;
    FILETIME ftLastWriteTime;
    DWORD    nFileSizeHigh;
    DWORD    nFileSizeLow;
    CHAR     cFileName[MAX_PATH];
    CHAR     cAlternateFileName[14];
} WIN32_FIND_DATAA, *LPWIN32_FIND_DATAA;
#define WIN32_FIND_DATA WIN32_FIND_DATAA

// FindFirstFile, FindNextFile, FindClose — implemented in compat_file_io.h
#define FILE_ATTRIBUTE_DIRECTORY 0x00000010

// ---- Registry stubs ----
inline LONG RegOpenKeyExA(HKEY key, LPCSTR sub, DWORD opts, DWORD access, HKEY* result) {
    (void)key; (void)sub; (void)opts; (void)access; (void)result; return ERROR_SUCCESS;
}
#define RegOpenKeyEx RegOpenKeyExA
inline LONG RegQueryValueExA(HKEY key, LPCSTR name, LPDWORD reserved, LPDWORD type, LPBYTE data, LPDWORD size) {
    (void)key; (void)name; (void)reserved; (void)type; (void)data; (void)size; return ERROR_SUCCESS;
}
#define RegQueryValueEx RegQueryValueExA
inline LONG RegSetValueExA(HKEY key, LPCSTR name, DWORD reserved, DWORD type, const BYTE* data, DWORD size) {
    (void)key; (void)name; (void)reserved; (void)type; (void)data; (void)size; return ERROR_SUCCESS;
}
#define RegSetValueEx RegSetValueExA
inline LONG RegCloseKey(HKEY key) { (void)key; return ERROR_SUCCESS; }
inline LONG RegCreateKeyExA(HKEY key, LPCSTR sub, DWORD reserved, LPSTR cls, DWORD opts,
                            DWORD access, void* sa, HKEY* result, LPDWORD disp) {
    (void)key; (void)sub; (void)reserved; (void)cls; (void)opts;
    (void)access; (void)sa; (void)result; (void)disp; return ERROR_SUCCESS;
}
#define RegCreateKeyEx RegCreateKeyExA
#define KEY_ALL_ACCESS 0xF003F

// ---- Computer name ----
inline BOOL GetComputerNameA(LPSTR buf, void* size) {
    if (buf && size) { buf[0] = '\0'; }
    return TRUE;
}
#define GetComputerName GetComputerNameA

// ---- Process stubs ----
typedef struct _PROCESS_MEMORY_COUNTERS {
    DWORD cb;
    DWORD PageFaultCount;
    size_t PeakWorkingSetSize;
    size_t WorkingSetSize;
} PROCESS_MEMORY_COUNTERS;

inline HANDLE GetCurrentProcess() { return NULL; }
inline DWORD GetCurrentProcessId() { return (DWORD)getpid(); }
inline BOOL GetProcessMemoryInfo(HANDLE proc, PROCESS_MEMORY_COUNTERS* pmc, DWORD cb) {
    (void)proc; (void)pmc; (void)cb; return FALSE;
}

typedef struct _STARTUPINFOA {
    DWORD cb;
    LPSTR lpReserved;
    LPSTR lpDesktop;
    LPSTR lpTitle;
    DWORD dwX;
    DWORD dwY;
    DWORD dwXSize;
    DWORD dwYSize;
    DWORD dwXCountChars;
    DWORD dwYCountChars;
    DWORD dwFillAttribute;
    DWORD dwFlags;
    WORD  wShowWindow;
    WORD  cbReserved2;
    LPBYTE lpReserved2;
    HANDLE hStdInput;
    HANDLE hStdOutput;
    HANDLE hStdError;
} STARTUPINFOA;
#define STARTUPINFO STARTUPINFOA
#define STARTF_USESHOWWINDOW 0x00000001
#define SW_SHOWMINNOACTIVE 7

typedef struct _PROCESS_INFORMATION {
    HANDLE hProcess;
    HANDLE hThread;
    DWORD dwProcessId;
    DWORD dwThreadId;
} PROCESS_INFORMATION;

inline BOOL CreateProcessA(LPCSTR app, LPSTR cmd, void* pa, void* ta, BOOL inh, DWORD flags,
                           LPVOID env, LPCSTR dir, STARTUPINFOA* si, PROCESS_INFORMATION* pi) {
    (void)app; (void)cmd; (void)pa; (void)ta; (void)inh; (void)flags;
    (void)env; (void)dir; (void)si; (void)pi; return FALSE;
}
#define CreateProcess CreateProcessA

// ---- GDI stubs ----
inline HDC GetDC(HWND hw) { (void)hw; return NULL; }
inline int ReleaseDC(HWND hw, HDC dc) { (void)hw; (void)dc; return 0; }
inline BOOL SetWindowTextA(HWND hw, LPCSTR text) { (void)hw; (void)text; return TRUE; }
#define SetWindowText SetWindowTextA

// ---- Window function stubs ----
typedef LRESULT (*WNDPROC)(HWND, UINT, WPARAM, LPARAM);

typedef struct tagWNDCLASSA {
    UINT      style;
    WNDPROC   lpfnWndProc;
    int       cbClsExtra;
    int       cbWndExtra;
    HINSTANCE hInstance;
    HICON     hIcon;
    HCURSOR   hCursor;
    HBRUSH    hbrBackground;
    LPCSTR    lpszMenuName;
    LPCSTR    lpszClassName;
} WNDCLASSA;
#define WNDCLASS WNDCLASSA

inline BOOL ShowWindow(HWND hw, int cmd) { (void)hw; (void)cmd; return TRUE; }
inline BOOL UpdateWindow(HWND hw) { (void)hw; return TRUE; }
inline BOOL DestroyWindow(HWND hw) { (void)hw; return TRUE; }
inline BOOL PostMessageA(HWND hw, UINT msg, WPARAM wp, LPARAM lp) {
    (void)hw; (void)msg; (void)wp; (void)lp; return TRUE;
}
#define PostMessage PostMessageA
inline void PostQuitMessage(int code) { (void)code; }
inline LRESULT SendMessageA(HWND hw, UINT msg, WPARAM wp, LPARAM lp) {
    (void)hw; (void)msg; (void)wp; (void)lp; return 0;
}
#define SendMessage SendMessageA
inline LRESULT DefWindowProcA(HWND hw, UINT msg, WPARAM wp, LPARAM lp) {
    (void)hw; (void)msg; (void)wp; (void)lp; return 0;
}
#define DefWindowProc DefWindowProcA
inline BOOL GetMessageA(LPMSG msg, HWND hw, UINT min_val, UINT max_val) {
    (void)msg; (void)hw; (void)min_val; (void)max_val; return FALSE;
}
#define GetMessage GetMessageA
// PeekMessage, TranslateMessage, DispatchMessage — implemented in compat_sdl_events.h
// (included at end of this file, needs SDL)

#define SW_SHOW         5
#define SW_HIDE         0
#define SW_MAXIMIZE     3
#define SW_MINIMIZE     6
#define SW_RESTORE      9

// ---- SYSTEMTIME ----
typedef struct _SYSTEMTIME {
    WORD wYear;
    WORD wMonth;
    WORD wDayOfWeek;
    WORD wDay;
    WORD wHour;
    WORD wMinute;
    WORD wSecond;
    WORD wMilliseconds;
} SYSTEMTIME;
inline void GetLocalTime(SYSTEMTIME* st) {
    if (!st) return;
    time_t t = time(NULL);
    struct tm* tm = localtime(&t);
    st->wYear = (WORD)(tm->tm_year + 1900);
    st->wMonth = (WORD)(tm->tm_mon + 1);
    st->wDayOfWeek = (WORD)tm->tm_wday;
    st->wDay = (WORD)tm->tm_mday;
    st->wHour = (WORD)tm->tm_hour;
    st->wMinute = (WORD)tm->tm_min;
    st->wSecond = (WORD)tm->tm_sec;
    st->wMilliseconds = 0;
}
inline void GetSystemTime(SYSTEMTIME* st) {
    if (!st) return;
    time_t t = time(NULL);
    struct tm* tm = gmtime(&t);
    st->wYear = (WORD)(tm->tm_year + 1900);
    st->wMonth = (WORD)(tm->tm_mon + 1);
    st->wDayOfWeek = (WORD)tm->tm_wday;
    st->wDay = (WORD)tm->tm_mday;
    st->wHour = (WORD)tm->tm_hour;
    st->wMinute = (WORD)tm->tm_min;
    st->wSecond = (WORD)tm->tm_sec;
    st->wMilliseconds = 0;
}
inline BOOL SystemTimeToFileTime(const SYSTEMTIME* st, FILETIME* ft) { (void)st; (void)ft; return TRUE; }
inline BOOL FileTimeToSystemTime(const FILETIME* ft, SYSTEMTIME* st) { (void)ft; (void)st; return TRUE; }
inline BOOL GetFileTime(HANDLE hFile, FILETIME* lpCreate, FILETIME* lpAccess, FILETIME* lpWrite) {
    (void)hFile; (void)lpCreate; (void)lpAccess; (void)lpWrite; return TRUE;
}
inline BOOL SetFileTime(HANDLE hFile, const FILETIME* lpCreate, const FILETIME* lpAccess, const FILETIME* lpWrite) {
    (void)hFile; (void)lpCreate; (void)lpAccess; (void)lpWrite; return TRUE;
}
inline BOOL CompareFileTime(const FILETIME* ft1, const FILETIME* ft2) {
    (void)ft1; (void)ft2; return 0;
}

// ---- File/Process stubs ----
inline BOOL DeleteFileA(LPCSTR path) { return ::remove(path) == 0; }
#define DeleteFile DeleteFileA
inline BOOL CreateDirectoryA(LPCSTR path, void* sa) { (void)path; (void)sa; return TRUE; }
#define CreateDirectory CreateDirectoryA
inline void ExitProcess(UINT code) { exit(code); }

// ---- Keyboard state (tracked by compat_sdl_events.h) ----
extern unsigned char _compat_key_state[256]; // defined in platform_main.cpp
inline SHORT GetKeyState(int vkey) {
    if (vkey < 0 || vkey > 255) return 0;
    return _compat_key_state[vkey] ? (SHORT)0x8000 : 0;
}
inline SHORT GetAsyncKeyState(int vkey) { return GetKeyState(vkey); }

// ---- Cursor stubs ----
inline BOOL GetCursorPos(LPPOINT pt) { (void)pt; return TRUE; }
inline BOOL SetCursorPos(int x, int y) { (void)x; (void)y; return TRUE; }
inline HCURSOR SetCursor(HCURSOR c) { (void)c; return NULL; }
inline HCURSOR LoadCursorA(HINSTANCE hi, LPCSTR name) { (void)hi; (void)name; return NULL; }
#define LoadCursor LoadCursorA

// ---- _stat mapping ----
#include <sys/stat.h>
#define _stat stat
#define _access access

// ---- Shell stubs ----
inline HINSTANCE ShellExecuteA(HWND hw, LPCSTR op, LPCSTR file, LPCSTR params, LPCSTR dir, int show) {
    (void)hw; (void)op; (void)file; (void)params; (void)dir; (void)show;
    return NULL;
}
#define ShellExecute ShellExecuteA

// ---- Misc ----
inline void OutputDebugStringA(LPCSTR str) { (void)str; }
#define OutputDebugString OutputDebugStringA

inline int MulDiv(int a, int b, int c) { return (int)((long long)a * b / c); }
inline void* GetProcAddress(HMODULE hm, LPCSTR name) { (void)hm; (void)name; return NULL; }
inline HMODULE LoadLibraryA(LPCSTR name) { (void)name; return NULL; }
#define LoadLibrary LoadLibraryA
inline BOOL FreeLibrary(HMODULE hm) { (void)hm; return TRUE; }

// ---- DLL entry point constants ----
#define DLL_PROCESS_ATTACH 1
#define DLL_THREAD_ATTACH  2
#define DLL_THREAD_DETACH  3
#define DLL_PROCESS_DETACH 0

#define MAKEFOURCC(ch0, ch1, ch2, ch3)                              \
                ((DWORD)(BYTE)(ch0) | ((DWORD)(BYTE)(ch1) << 8) |   \
                ((DWORD)(BYTE)(ch2) << 16) | ((DWORD)(BYTE)(ch3) << 24 ))

// ---- Memory allocation ----
inline HGLOBAL GlobalAlloc(UINT flags, size_t bytes) { (void)flags; return malloc(bytes); }
inline HGLOBAL GlobalFree(HGLOBAL mem) { free(mem); return NULL; }
inline LPVOID GlobalLock(HGLOBAL mem) { return mem; }
inline BOOL GlobalUnlock(HGLOBAL mem) { (void)mem; return TRUE; }
#define GMEM_FIXED 0x0000
#define GMEM_ZEROINIT 0x0040
#define GPTR (GMEM_FIXED | GMEM_ZEROINIT)

// ---- Time ----
typedef union _LARGE_INTEGER {
    struct {
        DWORD LowPart;
        LONG  HighPart;
    };
    LONGLONG QuadPart;
} LARGE_INTEGER;
inline BOOL QueryPerformanceCounter(LARGE_INTEGER* counter) { (void)counter; return FALSE; }
inline BOOL QueryPerformanceFrequency(LARGE_INTEGER* freq) { (void)freq; return FALSE; }

// ---- Interlocked ----
inline LONG InterlockedIncrement(volatile LONG* val) { return ++(*val); }
inline LONG InterlockedDecrement(volatile LONG* val) { return --(*val); }

// ---- Memory-mapped file constants ----
#define PAGE_READONLY       0x02
#define PAGE_READWRITE      0x04
#define FILE_MAP_READ       0x0004
#define FILE_MAP_WRITE      0x0002
// CreateFileMapping, MapViewOfFile, UnmapViewOfFile — implemented in compat_file_io.h

// ---- Locale stubs ----
typedef DWORD LCID;
typedef WORD LANGID;
#define LOCALE_SISO639LANGNAME 0x00000059
#define LOCALE_SISO3166CTRYNAME 0x0000005A
#define MAKELCID(lgid, srtid) ((LCID)((((DWORD)((WORD)(srtid))) << 16) | ((DWORD)((WORD)(lgid)))))
#define SORT_DEFAULT 0x0
#define SUBLANG_DEFAULT 0x01
#define LANG_NEUTRAL 0x00
#define MAKELANGID(p, s) ((((WORD)(s)) << 10) | (WORD)(p))
inline LCID GetSystemDefaultLCID() { return 0; }
inline LANGID GetSystemDefaultUILanguage() { return 0; }
inline LANGID GetUserDefaultUILanguage() { return 0; }
inline int GetLocaleInfoA(LCID lcid, DWORD lctype, LPSTR lpData, int cchData) {
    (void)lcid; (void)lctype; (void)lpData; (void)cchData; return 0;
}

// ---- Command line ----
inline LPSTR GetCommandLineA() { return (LPSTR)""; }
#define GetCommandLine GetCommandLineA

// ---- Event stubs ----
inline HANDLE CreateEventA(void* lpAttr, BOOL bManual, BOOL bInitial, LPCSTR lpName) {
    (void)lpAttr; (void)bManual; (void)bInitial; (void)lpName; return NULL;
}
#define CreateEvent CreateEventA
inline BOOL SetEvent(HANDLE hEvent) { (void)hEvent; return TRUE; }
inline BOOL ResetEvent(HANDLE hEvent) { (void)hEvent; return TRUE; }

// ---- Waitable timer / misc threading ----
inline DWORD WaitForMultipleObjects(DWORD nCount, const HANDLE* lpHandles, BOOL bWaitAll, DWORD dwMs) {
    (void)nCount; (void)lpHandles; (void)bWaitAll; (void)dwMs; return 0;
}
inline DWORD ResumeThread(HANDLE hThread) { (void)hThread; return 0; }
inline DWORD SuspendThread(HANDLE hThread) { (void)hThread; return 0; }
inline BOOL TerminateThread(HANDLE hThread, DWORD dwExitCode) { (void)hThread; (void)dwExitCode; return TRUE; }
#define CREATE_SUSPENDED 0x00000004

// ---- GetPrivateProfileString / WritePrivateProfileString ----
inline DWORD GetPrivateProfileStringA(LPCSTR lpApp, LPCSTR lpKey, LPCSTR lpDef,
                                       LPSTR lpRet, DWORD nSize, LPCSTR lpFile) {
    (void)lpApp; (void)lpKey; (void)lpFile;
    if (lpDef && lpRet && nSize > 0) { strncpy(lpRet, lpDef, nSize); lpRet[nSize-1]=0; return (DWORD)strlen(lpRet); }
    return 0;
}
#define GetPrivateProfileString GetPrivateProfileStringA
inline DWORD GetPrivateProfileIntA(LPCSTR lpApp, LPCSTR lpKey, INT nDef, LPCSTR lpFile) {
    (void)lpApp; (void)lpKey; (void)lpFile; return nDef;
}
#define GetPrivateProfileInt GetPrivateProfileIntA
inline BOOL WritePrivateProfileStringA(LPCSTR lpApp, LPCSTR lpKey, LPCSTR lpVal, LPCSTR lpFile) {
    (void)lpApp; (void)lpKey; (void)lpVal; (void)lpFile; return TRUE;
}
#define WritePrivateProfileString WritePrivateProfileStringA

// ---- GetWindowRect / SetWindowPos / MoveWindow ----
inline BOOL GetWindowRect(HWND hw, LPRECT lpRect) { (void)hw; (void)lpRect; return TRUE; }
inline BOOL GetClientRect(HWND hw, LPRECT lpRect) { (void)hw; (void)lpRect; return TRUE; }
inline BOOL SetWindowPos(HWND hw, HWND hwAfter, int x, int y, int cx, int cy, UINT flags) {
    (void)hw; (void)hwAfter; (void)x; (void)y; (void)cx; (void)cy; (void)flags; return TRUE;
}
inline BOOL MoveWindow(HWND hw, int x, int y, int w, int h, BOOL repaint) {
    (void)hw; (void)x; (void)y; (void)w; (void)h; (void)repaint; return TRUE;
}
#define HWND_TOPMOST ((HWND)(intptr_t)-1)
#define HWND_NOTOPMOST ((HWND)(intptr_t)-2)
#define SWP_NOSIZE 0x0001
#define SWP_NOMOVE 0x0002
#define SWP_SHOWWINDOW   0x0040
#define SWP_NOZORDER     0x0004
#define SWP_NOACTIVATE   0x0010
#define SWP_FRAMECHANGED 0x0020
#define SWP_NOSENDCHANGING 0x0400

// ---- Screen metrics ----
// Returns 0 (caller should use SDL_GetCurrentDisplayMode for real values)
inline int GetSystemMetrics(int nIndex) { (void)nIndex; return 1024; }
#define SM_CXSCREEN 0
#define SM_CYSCREEN 1

// ---- Clipboard stubs ----
inline BOOL OpenClipboard(HWND hw) { (void)hw; return TRUE; }
inline BOOL CloseClipboard() { return TRUE; }
inline BOOL EmptyClipboard() { return TRUE; }
inline HANDLE SetClipboardData(UINT fmt, HANDLE hMem) { (void)fmt; (void)hMem; return NULL; }
inline HANDLE GetClipboardData(UINT fmt) { (void)fmt; return NULL; }
#define CF_TEXT 1

// ---- Misc Win32 ----
inline BOOL SetForegroundWindow(HWND hw) { (void)hw; return TRUE; }
inline HWND GetForegroundWindow() { return NULL; }
inline HWND SetFocus(HWND hw) { (void)hw; return NULL; }
inline HWND GetActiveWindow() { return NULL; }
inline BOOL InvalidateRect(HWND hw, const RECT* lpRect, BOOL bErase) { (void)hw; (void)lpRect; (void)bErase; return TRUE; }
inline int GetDeviceCaps(HDC hdc, int index) { (void)hdc; (void)index; return 0; }
#define HORZRES 8
#define VERTRES 10
#define BITSPIXEL 12

// ---- File attributes ----
// GetFileAttributes — implemented in compat_file_io.h
#define INVALID_FILE_ATTRIBUTES ((DWORD)-1)
#define FILE_ATTRIBUTE_NORMAL 0x00000080

// ---- Atom/String ----
typedef WORD ATOM;

// ---- Window class registration ----
inline ATOM RegisterClassA(const WNDCLASSA* lpWndClass) { (void)lpWndClass; return 1; }
#define RegisterClass RegisterClassA
inline HWND CreateWindowExA(DWORD dwExStyle, LPCSTR lpClass, LPCSTR lpName, DWORD dwStyle,
                             int x, int y, int w, int h, HWND hwParent, HMENU hMenu,
                             HINSTANCE hInst, LPVOID lpParam) {
    (void)dwExStyle; (void)lpClass; (void)lpName; (void)dwStyle;
    (void)x; (void)y; (void)w; (void)h; (void)hwParent; (void)hMenu;
    (void)hInst; (void)lpParam;
    return NULL;
}
#define CreateWindowEx CreateWindowExA
#define CreateWindowA(cls,name,style,x,y,w,h,parent,menu,inst,param) \
    CreateWindowExA(0,(cls),(name),(style),(x),(y),(w),(h),(parent),(menu),(inst),(param))
#define CreateWindow CreateWindowA
#define CW_USEDEFAULT ((int)0x80000000)

// ---- Timer ----
typedef uintptr_t UINT_PTR;
typedef intptr_t INT_PTR;
inline UINT_PTR SetTimer(HWND hw, UINT_PTR nID, UINT uElapse, void* lpTimerFunc) {
    (void)hw; (void)nID; (void)uElapse; (void)lpTimerFunc; return nID;
}
inline BOOL KillTimer(HWND hw, UINT_PTR nID) { (void)hw; (void)nID; return TRUE; }

// ---- Additional string/path ----
inline LPSTR CharLowerA(LPSTR lpsz) { for (char* p = lpsz; *p; p++) *p = tolower(*p); return lpsz; }
#define CharLower CharLowerA
inline LPSTR CharUpperA(LPSTR lpsz) { for (char* p = lpsz; *p; p++) *p = toupper(*p); return lpsz; }
#define CharUpper CharUpperA

// ---- _splitpath / _makepath ----
inline void _splitpath(const char* path, char* drive, char* dir, char* fname, char* ext) {
    if (drive) drive[0] = 0;
    if (dir) dir[0] = 0;
    if (fname) fname[0] = 0;
    if (ext) ext[0] = 0;
    if (!path) return;
    const char* p = strrchr(path, '/');
    const char* p2 = strrchr(path, '\\');
    if (p2 && (!p || p2 > p)) p = p2;
    if (p) {
        if (dir) { size_t n = p - path + 1; strncpy(dir, path, n); dir[n] = 0; }
        p++;
    } else { p = path; }
    const char* dot = strrchr(p, '.');
    if (dot) {
        if (fname) { size_t n = dot - p; strncpy(fname, p, n); fname[n] = 0; }
        if (ext) strcpy(ext, dot);
    } else {
        if (fname) strcpy(fname, p);
    }
}
inline void _makepath(char* path, const char* drive, const char* dir, const char* fname, const char* ext) {
    path[0] = 0;
    if (drive && drive[0]) { strcat(path, drive); strcat(path, ":"); }
    if (dir && dir[0]) strcat(path, dir);
    if (fname) strcat(path, fname);
    if (ext && ext[0]) { if (ext[0] != '.') strcat(path, "."); strcat(path, ext); }
}

// ---- WideChar / MultiByte conversion stubs ----
#define CP_ACP   0
#define CP_UTF8  65001
#define MB_PRECOMPOSED 0x00000001

inline int MultiByteToWideChar(UINT cp, DWORD flags, LPCSTR lpMB, int cbMB,
                                LPWSTR lpWC, int cchWC) {
    (void)cp; (void)flags;
    if (!lpMB) return 0;
    int len = (cbMB == -1) ? (int)strlen(lpMB) + 1 : cbMB;
    if (cchWC == 0) return len;
    int i;
    for (i = 0; i < len && i < cchWC; i++) lpWC[i] = (WCHAR)(unsigned char)lpMB[i];
    return i;
}

inline int WideCharToMultiByte(UINT cp, DWORD flags, LPCWSTR lpWC, int cchWC,
                                LPSTR lpMB, int cbMB, LPCSTR lpDef, LPBOOL lpUsedDef) {
    (void)cp; (void)flags; (void)lpDef; (void)lpUsedDef;
    if (!lpWC) return 0;
    int len = cchWC;
    if (cchWC == -1) { len = 0; while (lpWC[len]) len++; len++; }
    if (cbMB == 0) return len;
    int i;
    for (i = 0; i < len && i < cbMB; i++) lpMB[i] = (char)(lpWC[i] & 0xFF);
    return i;
}

// ---- Additional misc stubs ----
inline DWORD GetLastError() { return 0; }
inline void SetLastError(DWORD dwErr) { (void)dwErr; }
inline BOOL IsBadReadPtr(const void* lp, UINT_PTR ucb) { (void)lp; (void)ucb; return FALSE; }
inline BOOL IsBadWritePtr(void* lp, UINT_PTR ucb) { (void)lp; (void)ucb; return FALSE; }

// ---- Heap stubs ----
inline HANDLE GetProcessHeap() { return NULL; }
inline LPVOID HeapAlloc(HANDLE hHeap, DWORD dwFlags, size_t dwBytes) {
    (void)hHeap; (void)dwFlags; return malloc(dwBytes);
}
inline BOOL HeapFree(HANDLE hHeap, DWORD dwFlags, LPVOID lpMem) {
    (void)hHeap; (void)dwFlags; free(lpMem); return TRUE;
}
#define HEAP_ZERO_MEMORY 0x00000008

// ---- Handle types ----
typedef HANDLE* LPHANDLE;

// ---- Cursor/Window functions ----
inline BOOL ClipCursor(const RECT* lpRect) { (void)lpRect; return TRUE; }
inline int MapWindowPoints(HWND hwFrom, HWND hwTo, LPPOINT lpPoints, UINT cPoints) {
    (void)hwFrom; (void)hwTo; (void)lpPoints; (void)cPoints; return 0;
}
inline BOOL AdjustWindowRect(LPRECT lpRect, DWORD dwStyle, BOOL bMenu) {
    (void)lpRect; (void)dwStyle; (void)bMenu; return TRUE;
}
inline BOOL AdjustWindowRectEx(LPRECT lpRect, DWORD dwStyle, BOOL bMenu, DWORD dwExStyle) {
    (void)lpRect; (void)dwStyle; (void)bMenu; (void)dwExStyle; return TRUE;
}
inline BOOL ScreenToClient(HWND hw, LPPOINT lpPoint) { (void)hw; (void)lpPoint; return TRUE; }
inline BOOL ClientToScreen(HWND hw, LPPOINT lpPoint) { (void)hw; (void)lpPoint; return TRUE; }

// ---- Keyboard stubs ----
inline BOOL GetKeyboardState(BYTE* lpKeyState) {
    if (lpKeyState) memcpy(lpKeyState, _compat_key_state, 256);
    return TRUE;
}
inline int ToAscii(UINT uVirtKey, UINT uScanCode, const BYTE* lpKeyState, LPWORD lpChar, UINT uFlags) {
    (void)uScanCode; (void)uFlags;
    if (!lpChar) return 0;
    // Basic ASCII mapping: if key is printable, return it
    bool shift = lpKeyState && (lpKeyState[VK_SHIFT] & 0x80);
    if (uVirtKey >= 'A' && uVirtKey <= 'Z') {
        *lpChar = shift ? uVirtKey : (uVirtKey + 32); // uppercase or lowercase
        return 1;
    }
    if (uVirtKey >= '0' && uVirtKey <= '9') {
        *lpChar = (WORD)uVirtKey;
        return 1;
    }
    if (uVirtKey == VK_SPACE) { *lpChar = ' '; return 1; }
    if (uVirtKey == VK_RETURN) { *lpChar = '\r'; return 1; }
    if (uVirtKey == VK_TAB) { *lpChar = '\t'; return 1; }
    return 0;
}
inline int ToUnicode(UINT wVirtKey, UINT wScanCode, const BYTE* lpKeyState,
                      LPWSTR pwszBuff, int cchBuff, UINT wFlags) {
    (void)wScanCode; (void)wFlags;
    if (!pwszBuff || cchBuff < 1) return 0;
    bool shift = lpKeyState && (lpKeyState[VK_SHIFT] & 0x80);
    if (wVirtKey >= 'A' && wVirtKey <= 'Z') {
        pwszBuff[0] = shift ? (WCHAR)wVirtKey : (WCHAR)(wVirtKey + 32);
        return 1;
    }
    if (wVirtKey >= '0' && wVirtKey <= '9') {
        // Shift+number for common symbols
        if (shift) {
            const char* shifted = ")!@#$%^&*(";
            pwszBuff[0] = (WCHAR)shifted[wVirtKey - '0'];
        } else {
            pwszBuff[0] = (WCHAR)wVirtKey;
        }
        return 1;
    }
    switch (wVirtKey) {
        case VK_SPACE:  pwszBuff[0] = ' '; return 1;
        case VK_RETURN: pwszBuff[0] = '\r'; return 1;
        case VK_TAB:    pwszBuff[0] = '\t'; return 1;
        default: break;
    }
    // Punctuation (unshifted US layout)
    switch (wVirtKey) {
        case 0xBA: pwszBuff[0] = shift ? ':' : ';'; return 1;
        case 0xBB: pwszBuff[0] = shift ? '+' : '='; return 1;
        case 0xBC: pwszBuff[0] = shift ? '<' : ','; return 1;
        case 0xBD: pwszBuff[0] = shift ? '_' : '-'; return 1;
        case 0xBE: pwszBuff[0] = shift ? '>' : '.'; return 1;
        case 0xBF: pwszBuff[0] = shift ? '?' : '/'; return 1;
        default: break;
    }
    return 0;
}

// ---- More VK codes ----
#define VK_NUMPAD0      0x60
#define VK_NUMPAD1      0x61
#define VK_NUMPAD2      0x62
#define VK_NUMPAD3      0x63
#define VK_NUMPAD4      0x64
#define VK_NUMPAD5      0x65
#define VK_NUMPAD6      0x66
#define VK_NUMPAD7      0x67
#define VK_NUMPAD8      0x68
#define VK_NUMPAD9      0x69
#define VK_MULTIPLY     0x6A
#define VK_ADD          0x6B
#define VK_SUBTRACT     0x6D
#define VK_DECIMAL      0x6E
#define VK_DIVIDE       0x6F
#define VK_CAPITAL      0x14
#define VK_INSERT       0x2D
#define VK_PRIOR        0x21
#define VK_NEXT         0x22
#define VK_END          0x23
#define VK_HOME         0x24

// ---- Mouse key flags ----
#define MK_LBUTTON      0x0001
#define MK_RBUTTON      0x0002
#define MK_MBUTTON      0x0010

// ---- More window messages ----
#define WM_ACTIVATEAPP  0x001C
#define WM_SETCURSOR    0x0020
#define WM_EXITSIZEMOVE 0x0232
#define WM_SYSCOMMAND   0x0112
#define MM_MCINOTIFY    0x03B9
#define SIZE_RESTORED   0
#define SIZE_MINIMIZED  1
#define SIZE_MAXIMIZED  2

// ---- MessageBox extras ----
#define MB_ICONSTOP     0x00000010L
#define MB_ICONHAND     0x00000010L
#define MB_ICONQUESTION 0x00000020L
#define MB_SETFOREGROUND 0x00010000L

// ---- Class styles ----
#define CS_HREDRAW      0x0002
#define CS_VREDRAW      0x0001
#define CS_DBLCLKS      0x0008

// ---- FARPROC ----
typedef void* FARPROC;

// ---- WA_ constants ----
#define WA_INACTIVE     0
#define WA_ACTIVE       1
#define WA_CLICKACTIVE  2

// ---- SetActiveWindow ----
inline HWND SetActiveWindow(HWND hw) { (void)hw; return NULL; }

// ---- Window styles ----
#define WS_THICKFRAME   0x00040000L
#define WS_CLIPCHILDREN 0x02000000L
#define WS_CLIPSIBLINGS 0x04000000L

// ---- DEVMODE ----
typedef struct _DEVMODEA {
    CHAR  dmDeviceName[32];
    WORD  dmSpecVersion;
    WORD  dmDriverVersion;
    WORD  dmSize;
    WORD  dmDriverExtra;
    DWORD dmFields;
    LONG  dmPelsWidth;
    LONG  dmPelsHeight;
    DWORD dmBitsPerPel;
    DWORD dmDisplayFrequency;
} DEVMODEA;
#define DEVMODE DEVMODEA
#define DM_PELSWIDTH    0x00080000L
#define DM_PELSHEIGHT   0x00100000L
#define DM_BITSPERPEL   0x00040000L
#define DISP_CHANGE_SUCCESSFUL 0
#define CDS_FULLSCREEN  0x00000004
#define ENUM_CURRENT_SETTINGS ((DWORD)-1)
inline LONG ChangeDisplaySettingsA(DEVMODEA* lpDevMode, DWORD dwFlags) { (void)lpDevMode; (void)dwFlags; return 0; }
#define ChangeDisplaySettings ChangeDisplaySettingsA
inline BOOL EnumDisplaySettingsA(LPCSTR lpDevice, DWORD iModeNum, DEVMODEA* lpDevMode) {
    (void)lpDevice; (void)iModeNum; (void)lpDevMode; return FALSE;
}
#define EnumDisplaySettings EnumDisplaySettingsA

// ---- ShowCursor ----
inline int ShowCursor(BOOL bShow) { (void)bShow; return 0; }

// ---- IDC cursor constants ----
#define IDC_ARROW ((LPCSTR)(intptr_t)32512)
#define IDC_WAIT  ((LPCSTR)(intptr_t)32514)

// ---- Misc window ----
inline LONG GetWindowLongA(HWND hw, int nIndex) { (void)hw; (void)nIndex; return 0; }
#define GetWindowLong GetWindowLongA
inline LONG SetWindowLongA(HWND hw, int nIndex, LONG dwNewLong) { (void)hw; (void)nIndex; (void)dwNewLong; return 0; }
#define SetWindowLong SetWindowLongA
#define GWL_STYLE (-16)
#define GWL_EXSTYLE (-20)

// ---- SC_ commands ----
#define SC_MAXIMIZE 0xF030
#define SC_MONITORPOWER 0xF170
#define SC_SCREENSAVE 0xF140

// ---- Window extended styles ----
#define WS_EX_APPWINDOW 0x00040000L
#define WS_EX_TOPMOST   0x00000008L
#define WS_EX_WINDOWEDGE 0x00000100L

// ---- ShowWindow constants ----
#define SW_SHOWNORMAL   1

// ---- PeekMessage flags ----
#define PM_REMOVE  0x0001
#define PM_NOREMOVE 0x0000

// ---- LoadIcon / LoadImage ----
#define IDI_APPLICATION ((LPCSTR)(intptr_t)32512)
inline HICON LoadIconA(HINSTANCE hi, LPCSTR name) { (void)hi; (void)name; return NULL; }
#define LoadIcon LoadIconA

// ---- Misc ----
inline BOOL IsIconic(HWND hw) { (void)hw; return FALSE; }
inline BOOL BringWindowToTop(HWND hw) { (void)hw; return TRUE; }
inline HWND GetDesktopWindow() { return NULL; }
inline BOOL IsWindow(HWND hw) { (void)hw; return TRUE; }
inline int GetClassName(HWND hw, LPSTR lpClassName, int nMaxCount) { (void)hw; (void)lpClassName; (void)nMaxCount; return 0; }

// ---- Bitmap info ----
typedef struct tagBITMAPINFO {
    BITMAPINFOHEADER bmiHeader;
    RGBQUAD          bmiColors[1];
} BITMAPINFO;

// ---- GDI extras ----
inline HBITMAP CreateDIBSection(HDC hdc, const BITMAPINFO* pbmi, UINT usage,
                                 void** ppvBits, HANDLE hSection, DWORD offset) {
    (void)hdc; (void)pbmi; (void)usage; (void)ppvBits; (void)hSection; (void)offset;
    return NULL;
}
#define DIB_RGB_COLORS 0
inline BOOL BitBlt(HDC hdc, int x, int y, int cx, int cy, HDC hdcSrc, int x1, int y1, DWORD rop) {
    (void)hdc; (void)x; (void)y; (void)cx; (void)cy; (void)hdcSrc; (void)x1; (void)y1; (void)rop;
    return TRUE;
}
typedef void* HGDIOBJ;
inline HGDIOBJ SelectObject(HDC hdc, void* h) { (void)hdc; (void)h; return NULL; }
inline BOOL DeleteObject(void* ho) { (void)ho; return TRUE; }
inline HDC CreateCompatibleDC(HDC hdc) { (void)hdc; return NULL; }
inline BOOL DeleteDC(HDC hdc) { (void)hdc; return TRUE; }
#define SRCCOPY 0x00CC0020

// ---- Interlocked exchange ----
inline LONG InterlockedExchange(volatile LONG* target, LONG value) {
    LONG old = *target; *target = value; return old;
}

#ifdef __cplusplus
}
#endif

// ---- Real file I/O implementations (must come after all type definitions) ----
#ifdef __cplusplus
#include "compat_file_io.h"
#include "compat_sdl_events.h"
#endif

#endif // _COMPAT_WINDOWS_H_
