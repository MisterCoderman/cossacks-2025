// ==============================================
// Эмулятор DirectDraw на базе SDL2
// MR.CODERMAN 2025
// ==============================================
#ifndef __DDINI_H_
#define __DDINI_H_

#include <windows.h>
#include <windowsx.h>
#include <stdlib.h>
#include <stdarg.h>
#include "resource.h"
#include "lines.h"

#define free _ExFree
#define malloc _ExMalloc
void* _ExMalloc(int Size);
void _ExFree(void* Ptr);

inline void* _cdecl operator new(size_t size) {
    return malloc(size);
}

inline void __cdecl operator delete(void* ptr) {
    free(ptr);
}

#define MAKE_PTC

// Эмуляция DirectDraw типов для совместимости
#define LPDIRECTDRAW void*
#define LPDIRECTDRAWSURFACE void*

// Заглушка для DDSURFACEDESC с полем lpSurface для совместимости
struct DDSURFACEDESC {
    int dummy;      // Заглушка
    void* lpSurface; // Добавлено для совместимости с Fastdraw.cpp
};

// Объявления переменных и функций
#ifndef __ddini_cpp_
extern LPDIRECTDRAW lpDD;           // Эмуляция DirectDraw object
extern LPDIRECTDRAWSURFACE lpDDSPrimary; // Эмуляция первичной поверхности
extern LPDIRECTDRAWSURFACE lpDDSBack;   // Эмуляция заднего буфера
extern BOOL bActive;        // Приложение активно?
extern BOOL CurrentSurface; // FALSE: backbuffer активен, TRUE: primary активен
extern BOOL DDError;        // FALSE: всё работает нормально
extern DDSURFACEDESC ddsd;  // Заглушка для совместимости
extern HWND hwnd;
extern bool window_mode;

// Разрешения экрана
extern int ModeLX[32];
extern int ModeLY[32];
extern int NModes;

// Переменные объявлены с dllexport, как в MapDiscr.h
extern __declspec(dllexport) int RealLx;
extern __declspec(dllexport) int RealLy;
extern __declspec(dllexport) int SCRSizeX;
extern __declspec(dllexport) int SCRSizeY;
extern __declspec(dllexport) int RSCRSizeX;
extern __declspec(dllexport) int RSCRSizeY;
extern __declspec(dllexport) int COPYSizeX;
extern __declspec(dllexport) int Pitch;
extern int SCRSZY; // Добавлено для совместимости

// Функции
bool CreateDDObjects(HWND hwnd);
int CreateRGBDDObjects(HWND hwnd);
int CreateRGB640DDObjects(HWND hwnd);
void FreeDDObjects(void);
__declspec(dllexport) void FlipPages(void);
void LockSurface(void);
void UnlockSurface(void);
void LoadPalette(LPCSTR lpFileName);
__declspec(dllexport) void SlowLoadPalette(LPCSTR lpFileName);
__declspec(dllexport) void SlowUnLoadPalette(LPCSTR lpFileName);
void SetDarkPalette(void);
void SetDebugMode();
void NoDebugMode();
__declspec(dllexport) byte GetPaletteColor(int r, int g, int b);
__declspec(dllexport) void GetPalColor(byte idx, byte* r, byte* g, byte* b); // Добавлено как в старом коде
#endif

#endif //__DDINI_H_
