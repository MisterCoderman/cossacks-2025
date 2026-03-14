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
#ifndef _MSC_VER
#define _MSC_VER 0
#endif

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

// ---- MessageBox ----
#define MB_OK               0x00000000L
#define MB_OKCANCEL         0x00000001L
#define MB_YESNO            0x00000004L
#define MB_ICONERROR        0x00000010L
#define MB_ICONWARNING      0x00000030L
#define MB_ICONINFORMATION  0x00000040L
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
inline DWORD GetTickCount() { return 0; }
inline void Sleep(DWORD ms) { (void)ms; }
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
inline DWORD GetCurrentDirectoryA(DWORD size, LPSTR buf) { (void)size; (void)buf; return 0; }
#define GetCurrentDirectory GetCurrentDirectoryA
inline DWORD GetLogicalDriveStringsA(DWORD size, LPSTR buf) { (void)size; (void)buf; return 0; }
#define GetLogicalDriveStrings GetLogicalDriveStringsA
inline BOOL SetCurrentDirectoryA(LPCSTR path) { (void)path; return TRUE; }
#define SetCurrentDirectory SetCurrentDirectoryA

// ---- Critical section stubs ----
typedef struct _CRITICAL_SECTION {
    void* dummy;
} CRITICAL_SECTION, *LPCRITICAL_SECTION;

inline void InitializeCriticalSection(LPCRITICAL_SECTION cs) { (void)cs; }
inline void DeleteCriticalSection(LPCRITICAL_SECTION cs) { (void)cs; }
inline void EnterCriticalSection(LPCRITICAL_SECTION cs) { (void)cs; }
inline void LeaveCriticalSection(LPCRITICAL_SECTION cs) { (void)cs; }

// ---- Threading stubs ----
inline HANDLE CreateThread(void* attr, size_t stack, void* func, void* param, DWORD flags, DWORD* id) {
    (void)attr; (void)stack; (void)func; (void)param; (void)flags; (void)id;
    return NULL;
}
inline DWORD WaitForSingleObject(HANDLE h, DWORD ms) { (void)h; (void)ms; return 0; }
inline BOOL CloseHandle(HANDLE h) { (void)h; return TRUE; }
#define INFINITE 0xFFFFFFFF
#define WAIT_OBJECT_0 0

// ---- File I/O stubs ----
inline HANDLE CreateFileA(LPCSTR name, DWORD access, DWORD share, void* sa, DWORD disp, DWORD flags, HANDLE templ) {
    (void)name; (void)access; (void)share; (void)sa; (void)disp; (void)flags; (void)templ;
    return INVALID_HANDLE_VALUE;
}
#define CreateFile CreateFileA
#define GENERIC_READ    0x80000000L
#define GENERIC_WRITE   0x40000000L
#define OPEN_EXISTING   3
#define CREATE_ALWAYS   2
#define FILE_SHARE_READ 0x00000001

inline BOOL ReadFile(HANDLE h, LPVOID buf, DWORD bytes, LPDWORD read, void* overlapped) {
    (void)h; (void)buf; (void)bytes; (void)read; (void)overlapped; return FALSE;
}
inline BOOL WriteFile(HANDLE h, LPCVOID buf, DWORD bytes, LPDWORD written, void* overlapped) {
    (void)h; (void)buf; (void)bytes; (void)written; (void)overlapped; return FALSE;
}
inline DWORD SetFilePointer(HANDLE h, LONG dist, LPLONG high, DWORD method) {
    (void)h; (void)dist; (void)high; (void)method; return 0;
}
inline DWORD GetFileSize(HANDLE h, LPDWORD high) { (void)h; (void)high; return 0; }

// ---- FindFile stubs ----
typedef struct _WIN32_FIND_DATAA {
    DWORD    dwFileAttributes;
    CHAR     cFileName[MAX_PATH];
} WIN32_FIND_DATAA, *LPWIN32_FIND_DATAA;
#define WIN32_FIND_DATA WIN32_FIND_DATAA

inline HANDLE FindFirstFileA(LPCSTR path, LPWIN32_FIND_DATAA data) {
    (void)path; (void)data; return INVALID_HANDLE_VALUE;
}
#define FindFirstFile FindFirstFileA
inline BOOL FindNextFileA(HANDLE h, LPWIN32_FIND_DATAA data) { (void)h; (void)data; return FALSE; }
#define FindNextFile FindNextFileA
inline BOOL FindClose(HANDLE h) { (void)h; return TRUE; }
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
inline BOOL GetComputerNameA(LPSTR buf, LPDWORD size) {
    if (buf && size && *size > 0) { buf[0] = '\0'; }
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
inline BOOL PeekMessageA(LPMSG msg, HWND hw, UINT min_val, UINT max_val, UINT remove) {
    (void)msg; (void)hw; (void)min_val; (void)max_val; (void)remove; return FALSE;
}
#define PeekMessage PeekMessageA
inline BOOL TranslateMessage(const MSG* msg) { (void)msg; return FALSE; }
inline LRESULT DispatchMessageA(const MSG* msg) { (void)msg; return 0; }
#define DispatchMessage DispatchMessageA

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
inline void GetLocalTime(SYSTEMTIME* st) { (void)st; }
inline void GetSystemTime(SYSTEMTIME* st) { (void)st; }

// ---- File/Process stubs ----
inline BOOL DeleteFileA(LPCSTR path) { return ::remove(path) == 0; }
#define DeleteFile DeleteFileA
inline BOOL CreateDirectoryA(LPCSTR path, void* sa) { (void)path; (void)sa; return TRUE; }
#define CreateDirectory CreateDirectoryA
inline void ExitProcess(UINT code) { exit(code); }

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
inline BOOL QueryPerformanceCounter(LONGLONG* counter) { (void)counter; return FALSE; }
inline BOOL QueryPerformanceFrequency(LONGLONG* freq) { (void)freq; return FALSE; }
typedef struct _LARGE_INTEGER {
    LONGLONG QuadPart;
} LARGE_INTEGER;

// ---- Interlocked ----
inline LONG InterlockedIncrement(volatile LONG* val) { return ++(*val); }
inline LONG InterlockedDecrement(volatile LONG* val) { return --(*val); }

#ifdef __cplusplus
}
#endif

#endif // _COMPAT_WINDOWS_H_
