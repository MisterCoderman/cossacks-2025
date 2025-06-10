// ==============================================
// Эмулятор DirectDraw на базе SDL2
// MR.CODERMAN 2025
// ==============================================
#include <algorithm>
#include <cstring>
#include <vector>
#include <thread>
#include <mutex>
#include <windows.h>
#include <psapi.h>
#define __ddini_cpp_
#include "ddini.h"
#include "ResFile.h"
#include "FastDraw.h"
#include "mode.h"
#include "MapDiscr.h"
#include "fog.h"
#include "GSound.h"
#include "fonts.h"
#include "VirtScreen.h"
#include <SDL.h>
extern byte PlayGameMode;
void Rept(LPSTR sz, ...);
__declspec(dllexport) int ModeLX[32];
__declspec(dllexport) int ModeLY[32];
__declspec(dllexport) int NModes = 0;
__declspec(dllexport) int RealLx = 800;
__declspec(dllexport) int RealLy = 600;
__declspec(dllexport) int SCRSizeX = 800;
__declspec(dllexport) int SCRSizeY = 600;
__declspec(dllexport) int RSCRSizeX = 800;
__declspec(dllexport) int RSCRSizeY = 600;
__declspec(dllexport) int COPYSizeX = 800;
__declspec(dllexport) int Pitch = 800;
int SCRSZY = 600;
extern bool window_mode;
extern HWND hwnd;
LPDIRECTDRAW lpDD = nullptr;
LPDIRECTDRAWSURFACE lpDDSPrimary = nullptr;
LPDIRECTDRAWSURFACE lpDDSBack = nullptr;
BOOL bActive = FALSE;
BOOL CurrentSurface = TRUE;
BOOL DDError = FALSE;
DDSURFACEDESC ddsd;
void SERROR();
void SERROR1();
void SERROR2();
void InitRLCWindows();
const int InitLx = 1024;
const int InitLy = 768;
SDL_Window* gWindow = nullptr;
SDL_Renderer* gRenderer = nullptr;
SDL_Texture* gPrimaryTexture = nullptr;
SDL_Texture* gBackTexture = nullptr;
SDL_Palette* gPalette = nullptr;
SDL_Color GPal[256];
void* offScreenPtr = nullptr;
Uint32* rgbBuffer = nullptr;
Uint32 lutPalette[256];
std::mutex renderMutex;
extern bool PalDone;
extern word PlayerMenuMode;

// Проверка использования памяти процесса (в байтах)
size_t GetProcessMemoryUsage() {
    PROCESS_MEMORY_COUNTERS pmc;
    if (GetProcessMemoryInfo(GetCurrentProcess(), &pmc, sizeof(pmc))) {
        return pmc.WorkingSetSize;
    }
    return 0;
}

// Функция перезапуска текущего процесса
bool RestartProcess() {
    char exePath[MAX_PATH];
    if (!GetModuleFileNameA(NULL, exePath, MAX_PATH)) {
        return false;
    }
    STARTUPINFOA si = { sizeof(si) };
    PROCESS_INFORMATION pi = { 0 };
    BOOL success = CreateProcessA(exePath, GetCommandLineA(), NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi);
    if (!success) {
        return false;
    }
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
    ExitProcess(0);
    return true;
}

static void ConvertUTF8ToWindows1251(const char* utf8Str, char* outBuf, int outBufSize) {
    WCHAR wideBuf[256];
    int wideLen = MultiByteToWideChar(CP_UTF8, 0, utf8Str, -1, wideBuf, 256);
    if (wideLen == 0) {
        strncpy(outBuf, utf8Str, outBufSize - 1);
        outBuf[outBufSize - 1] = '\0';
        return;
    }
    WideCharToMultiByte(1251, 0, wideBuf, -1, outBuf, outBufSize, NULL, NULL);
}

__declspec(dllexport) byte GetPaletteColor(int r, int g, int b) {
    int dmax = 10000;
    int bestc = 0;
    for (int i = 0; i < 256; i++) {
        int d = abs(r - GPal[i].r) + abs(g - GPal[i].g) + abs(b - GPal[i].b);
        if (d < dmax) {
            dmax = d;
            bestc = i;
        }
    }
    return bestc;
}

__declspec(dllexport) void GetPalColor(byte idx, byte* r, byte* g, byte* b) {
    if (idx < 256) {
        *r = GPal[idx].r;
        *g = GPal[idx].g;
        *b = GPal[idx].b;
    }
    else {
        *r = 0;
        *g = 0;
        *b = 0;
    }
}

void ClearRGB() {
    std::lock_guard<std::mutex> lock(renderMutex);
    if (!bActive || !offScreenPtr) return;
    memset(offScreenPtr, 0, RSCRSizeX * SCRSZY);
}

extern bool InGame;
extern bool InEditor;

static void UpdatePaletteLUT() {
    for (int i = 0; i < 256; i++) {
        lutPalette[i] = (GPal[i].a << 24) | (GPal[i].r << 16) | (GPal[i].g << 8) | GPal[i].b;
    }
}


__declspec(dllexport) void FlipPages(void) {
    std::lock_guard<std::mutex> lock(renderMutex);
    if (!bActive || DDError) return;
    if (!gRenderer || !gPrimaryTexture || !gBackTexture) {
        char err[256], msg[256];
        sprintf(err, "FlipPages: Invalid renderer or textures");
        ConvertUTF8ToWindows1251(err, msg, 256);
        MessageBoxA(NULL, msg, "SDL Error", MB_OK | MB_ICONERROR);
        return;
    }
    if (!rgbBuffer) {
        rgbBuffer = (Uint32*)malloc(RealLx * RealLy * sizeof(Uint32));
        if (!rgbBuffer) {
            char err[256], msg[256];
            sprintf(err, "Failed to allocate rgbBuffer");
            ConvertUTF8ToWindows1251(err, msg, 256);
            MessageBoxA(NULL, msg, "SDL Error", MB_OK | MB_ICONERROR);
            return;
        }
    }
    if (!InGame) {
        memset(rgbBuffer, 0, RealLx * RealLy * sizeof(Uint32));
    }
    byte* src = (byte*)offScreenPtr + MaxSizeX * 32;
    Uint32* dst = rgbBuffer;
    unsigned int threadCount = std::thread::hardware_concurrency();
    if (threadCount == 0) threadCount = 1; // На случай, если hardware_concurrency вернёт 0

    // Определяем количество потоков в зависимости от разрешения
    if (threadCount == 1) {
        threadCount = 1; // Одно ядро для одноядерных систем
    }
    else if (RealLx <= 1920 && RealLy <= 1080) {
        threadCount = (threadCount < 4) ? threadCount : 4; // До 4 ядер для 1920x1080
    } // Для разрешений выше 1920x1080 используем все доступные ядра

    int linesPerThread = RealLy / threadCount;

    // Многопоточная обработка пикселей
    std::vector<std::thread> threads;
    threads.reserve(threadCount);
    for (unsigned int i = 0; i < threadCount; ++i) {
        int startY = i * linesPerThread;
        int endY = (i == threadCount - 1) ? RealLy : startY + linesPerThread;
        threads.emplace_back([=, &src, &dst]() {
            for (int y = startY; y < endY; ++y) {
                for (int x = 0; x < RealLx; ++x) {
                    int p = y * RealLx + x;
                    int idx = y * MaxSizeX + x;
                    dst[p] = lutPalette[src[idx]];
                }
            }
            });
    }
    for (auto& t : threads) t.join(); // Ожидаем завершения всех потоков

    SDL_Texture* target = window_mode ? gBackTexture : gPrimaryTexture;
    if (SDL_UpdateTexture(target, NULL, rgbBuffer, RealLx * sizeof(Uint32)) != 0) {
        char err[256], msg[256];
        sprintf(err, "SDL_UpdateTexture failed: %s", SDL_GetError());
        ConvertUTF8ToWindows1251(err, msg, 256);
        MessageBoxA(NULL, msg, "SDL Error", MB_OK | MB_ICONERROR);
        return;
    }
    if (SDL_SetRenderDrawColor(gRenderer, 0, 0, 0, 255) != 0 ||
        SDL_RenderClear(gRenderer) != 0) {
        char err[256], msg[256];
        sprintf(err, "SDL render setup failed: %s", SDL_GetError());
        ConvertUTF8ToWindows1251(err, msg, 256);
        MessageBoxA(NULL, msg, "SDL Error", MB_OK | MB_ICONERROR);
        return;
    }
    SDL_Rect dstRect = { 0, 0, RealLx, RealLy };
    if (SDL_RenderCopy(gRenderer, target, NULL, &dstRect) != 0) {
        char err[256], msg[256];
        sprintf(err, "SDL_RenderCopy failed: %s", SDL_GetError());
        ConvertUTF8ToWindows1251(err, msg, 256);
        MessageBoxA(NULL, msg, "SDL Error", MB_OK | MB_ICONERROR);
        return;
    }
    printf("FlipPages: Rendering with RealLx=%d, RealLy=%d, InGame=%d, Threads=%u\n", RealLx, RealLy, InGame, threadCount);
    SDL_RenderPresent(gRenderer);
}

void LockSurface(void) {
    std::lock_guard<std::mutex> lock(renderMutex);
    if (DDError) return;
    if (!offScreenPtr || SCRSizeX <= 0 || SCRSZY <= 0) {
        DDError = TRUE;
        char errorMsg[256], convertedMsg[256];
        sprintf(errorMsg, "LockSurface: Invalid offScreenPtr or dimensions (SCRSizeX=%d, SCRSZY=%d)", SCRSizeX, SCRSZY);
        ConvertUTF8ToWindows1251(errorMsg, convertedMsg, 256);
        MessageBoxA(NULL, convertedMsg, "Error", MB_OK | MB_ICONERROR);
        return;
    }
    ScreenPtr = (byte*)offScreenPtr + MaxSizeX * 32;
    RealScreenPtr = ScreenPtr;
    size_t bufferSize = SCRSizeX * SCRSZY;
    if (bufferSize > 0) {
        memset(ScreenPtr, 0, bufferSize);
    }
    else {
        DDError = TRUE;
        char errorMsg[256], convertedMsg[256];
        sprintf(errorMsg, "LockSurface: Invalid buffer size (SCRSizeX=%d, SCRSZY=%d)", SCRSizeX, SCRSZY);
        ConvertUTF8ToWindows1251(errorMsg, convertedMsg, 256);
        MessageBoxA(NULL, convertedMsg, "Error", MB_OK | MB_ICONERROR);
    }
}

void UnlockSurface(void) {
    std::lock_guard<std::mutex> lock(renderMutex);
    if (DDError) return;
}

bool EnumModesOnly() {
    NModes = 0;
    DEVMODE devMode;
    devMode.dmSize = sizeof(DEVMODE);
    for (int iMode = 0; EnumDisplaySettings(NULL, iMode, &devMode) != 0 && NModes < 32; ++iMode) {
        if (devMode.dmBitsPerPel == 32 &&
            devMode.dmPelsWidth >= 1024 && devMode.dmPelsHeight >= 768) {
            bool exists = false;
            for (int j = 0; j < NModes; j++) {
                if (ModeLX[j] == devMode.dmPelsWidth && ModeLY[j] == devMode.dmPelsHeight) {
                    exists = true;
                    break;
                }
            }
            if (!exists) {
                ModeLX[NModes] = devMode.dmPelsWidth;
                ModeLY[NModes] = devMode.dmPelsHeight;
                NModes++;
            }
        }
    }
    if (NModes == 0) {
        const int fallbackModes[][2] = {
            {1024, 768}, {1280, 720}, {1366, 768}, {1600, 900}, {1920, 1080}
        };
        for (int i = 0; i < 5 && NModes < 32; i++) {
            ModeLX[NModes] = fallbackModes[i][0];
            ModeLY[NModes] = fallbackModes[i][1];
            NModes++;
        }
    }
    return NModes > 0;
}

bool IsRunningUnderWine_ByNtDll() {
    HMODULE ntdll = GetModuleHandleA("ntdll.dll");
    if (!ntdll) return false;
    FARPROC wineVer = GetProcAddress(ntdll, "wine_get_version");
    return wineVer != nullptr;
}

bool CreateDDObjects(HWND hwnd_param) {
    std::lock_guard<std::mutex> lock(renderMutex);
    static int callCount = 0;
    static bool wasInGameOrEditor = false;
    callCount++;

    // Сброс wasInGameOrEditor при выходе в меню
    if (!InGame && !InEditor && wasInGameOrEditor) {
        wasInGameOrEditor = false;
    }

    // Проверка использования памяти перед входом в игру или редактор
    if ((InGame || InEditor) && !wasInGameOrEditor) {
        size_t memoryUsage = GetProcessMemoryUsage();
        if (memoryUsage > 1ULL * 1024 * 1024 * 1024) {
            RestartProcess();
        }
    }

    wasInGameOrEditor = InGame || InEditor;

    bool isWine = IsRunningUnderWine_ByNtDll();
    bool needRecreateWindow = (callCount > 1 && (wasInGameOrEditor && !InGame && !InEditor) && !isWine);

    DDError = FALSE;
    CurrentSurface = TRUE;

    // Освобождение существующих ресурсов
    free(offScreenPtr);
    free(rgbBuffer);
    offScreenPtr = nullptr;
    rgbBuffer = nullptr;
    if (gPrimaryTexture) SDL_DestroyTexture(gPrimaryTexture);
    if (gBackTexture) SDL_DestroyTexture(gBackTexture);
    gPrimaryTexture = nullptr;
    gBackTexture = nullptr;
    if (gRenderer) SDL_DestroyRenderer(gRenderer);
    gRenderer = nullptr;
    if (gWindow && needRecreateWindow) {
        SDL_DestroyWindow(gWindow);
        gWindow = nullptr;
    }
    if (gPalette) SDL_FreePalette(gPalette);
    gPalette = nullptr;

    if (!hwnd_param) {
        char errorMsg[256], convertedMsg[256];
        sprintf(errorMsg, "Invalid HWND: hwnd is NULL");
        ConvertUTF8ToWindows1251(errorMsg, convertedMsg, 256);
        MessageBoxA(NULL, convertedMsg, "SDL Error", MB_OK | MB_ICONERROR);
        return false;
    }

    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        char errorMsg[256], convertedMsg[256];
        sprintf(errorMsg, "SDL_Init failed: %s", SDL_GetError());
        ConvertUTF8ToWindows1251(errorMsg, convertedMsg, 256);
        MessageBoxA(NULL, convertedMsg, "SDL Error", MB_OK | MB_ICONERROR);
        return false;
    }

    bool validResolution = false;
    for (int i = 0; i < NModes; i++) {
        if (RealLx == ModeLX[i] && RealLy == ModeLY[i]) {
            validResolution = true;
            break;
        }
    }
    if (!validResolution && NModes > 0) {
        RealLx = ModeLX[0];
        RealLy = ModeLY[0];
    }
    if (RealLx <= 0) RealLx = 800;
    if (RealLy <= 0) RealLy = 600;

    if (!gWindow) {
        gWindow = SDL_CreateWindowFrom(hwnd_param);
        if (!gWindow) {
            char errorMsg[256], convertedMsg[256];
            sprintf(errorMsg, "SDL_CreateWindowFrom failed: %s", SDL_GetError());
            ConvertUTF8ToWindows1251(errorMsg, convertedMsg, 256);
            MessageBoxA(NULL, convertedMsg, "SDL Error", MB_OK | MB_ICONERROR);
            SDL_Quit();
            return false;
        }
    }

    hwnd = hwnd_param;

    if (!InGame && !InEditor) {
        window_mode = true;
        RealLx = 1024;
        RealLy = 768;
        SDL_SetWindowFullscreen(gWindow, 0);
        SDL_SetWindowBordered(gWindow, SDL_TRUE);
        SDL_SetWindowSize(gWindow, RealLx, RealLy);
        SDL_SetWindowPosition(gWindow, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);
        SDL_SetWindowGrab(gWindow, SDL_FALSE);
        SDL_ShowCursor(SDL_ENABLE);
        SDL_SetWindowResizable(gWindow, SDL_FALSE);
        LONG style = GetWindowLong(hwnd_param, GWL_STYLE);
        style &= ~WS_MAXIMIZEBOX;
        style |= WS_MINIMIZEBOX;
        style |= WS_CAPTION;
        style |= WS_SYSMENU;
        SetWindowLong(hwnd_param, GWL_STYLE, style);
        SetWindowPos(hwnd_param, NULL, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_FRAMECHANGED);
    }
    if ((InGame || InEditor) && window_mode) {
        if (isWine) {
            SDL_SetWindowFullscreen(gWindow, SDL_WINDOW_FULLSCREEN_DESKTOP);
            SDL_Delay(50);
        }
        else {
            SDL_SetWindowFullscreen(gWindow, SDL_WINDOW_FULLSCREEN);
        }
        SDL_SetWindowSize(gWindow, RealLx, RealLy);
        SDL_SetWindowPosition(gWindow, 0, 0);
        SDL_SetWindowGrab(gWindow, SDL_TRUE);
        SDL_ShowCursor(SDL_DISABLE);
        SDL_SetWindowResizable(gWindow, SDL_FALSE);
        if (!isWine) {
            LONG style = GetWindowLong(hwnd_param, GWL_STYLE);
            style &= ~WS_CAPTION;
            style &= ~WS_SYSMENU;
            style &= ~WS_MINIMIZEBOX;
            style &= ~WS_MAXIMIZEBOX;
            style &= ~WS_THICKFRAME;
            SetWindowLong(hwnd_param, GWL_STYLE, style);
            SetWindowPos(hwnd_param, NULL, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_FRAMECHANGED);
        }
    }
    else if (!window_mode) {
        SDL_SetWindowSize(gWindow, RealLx, RealLy);
        if (SDL_SetWindowFullscreen(gWindow, SDL_WINDOW_FULLSCREEN_DESKTOP) < 0) {
            char errorMsg[256], convertedMsg[256];
            sprintf(errorMsg, "SDL_SetWindowFullscreen_DESKTOP failed: %s", SDL_GetError());
            ConvertUTF8ToWindows1251(errorMsg, convertedMsg, 256);
            MessageBoxA(NULL, convertedMsg, "SDL Error", MB_OK | MB_ICONERROR);
            window_mode = true;
            SDL_SetWindowFullscreen(gWindow, 0);
            SDL_SetWindowSize(gWindow, RealLx, RealLy);
            SDL_SetWindowPosition(gWindow, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);
        }
        else {
            int w, h;
            SDL_GetWindowSize(gWindow, &w, &h);
            RealLx = w;
            RealLy = h;
            SDL_SetWindowGrab(gWindow, SDL_TRUE);
            SDL_ShowCursor(SDL_DISABLE);
        }
    }
    else {
        SDL_SetWindowSize(gWindow, RealLx, RealLy);
        SDL_SetWindowPosition(gWindow, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);
        SDL_SetWindowBordered(gWindow, SDL_TRUE);
        SDL_SetWindowGrab(gWindow, SDL_FALSE);
        SDL_ShowCursor(SDL_ENABLE);
        SDL_SetWindowResizable(gWindow, SDL_FALSE);
        LONG style = GetWindowLong(hwnd_param, GWL_STYLE);
        style |= WS_MINIMIZEBOX;
        style |= WS_MAXIMIZEBOX;
        style |= WS_CAPTION;
        style |= WS_SYSMENU;
        SetWindowLong(hwnd_param, GWL_STYLE, style);
        SetWindowPos(hwnd_param, NULL, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_FRAMECHANGED);
    }

    Uint32 rendererFlags = SDL_RENDERER_ACCELERATED;
    if (InGame || InEditor) {
        rendererFlags |= SDL_RENDERER_PRESENTVSYNC;
    }
    gRenderer = SDL_CreateRenderer(gWindow, -1, rendererFlags);
    if (!gRenderer) {
        char errorMsg[256], convertedMsg[256];
        sprintf(errorMsg, "SDL_CreateRenderer failed: %s", SDL_GetError());
        ConvertUTF8ToWindows1251(errorMsg, convertedMsg, 256);
        MessageBoxA(NULL, convertedMsg, "SDL Error", MB_OK | MB_ICONERROR);
        SDL_DestroyWindow(gWindow);
        SDL_Quit();
        return false;
    }

    if (!InGame && !InEditor) {
        SDL_RenderSetLogicalSize(gRenderer, 1024, 768);
    }

    if (!InGame && !InEditor) {
        RealLx = 1024;
        RealLy = 768;
        SCRSizeX = 1024;
        SCRSizeY = 768;
        RSCRSizeX = 1024;
        RSCRSizeY = 768;
        COPYSizeX = 1024;
        SCRSZY = 768;
        Pitch = 1024;
        ScrWidth = 1024;
        ScrHeight = 768;
        WindLx = 1024;
        WindLy = 768;
        WindX = 0;
        WindY = 0;
        WindX1 = 1023;
        WindY1 = 767;
        SVSC.SetSize(1024, 768);
    }
    else if (window_mode) {
        SCRSizeX = RealLx;
        SCRSizeY = RealLy;
        RSCRSizeX = RealLx;
        RSCRSizeY = RealLy;
        COPYSizeX = RealLx;
        SCRSZY = RealLy;
        Pitch = RealLx;
        ScrWidth = RealLx;
        ScrHeight = RealLy;
        WindLx = RealLx;
        WindLy = RealLy;
        WindX = 0;
        WindY = 0;
        WindX1 = RealLx - 1;
        WindY1 = RealLy - 1;
        SVSC.SetSize(RealLx, RealLy);
    }
    else {
        SCRSizeX = RealLx;
        SCRSizeY = RealLy;
        RSCRSizeX = RealLx;
        RSCRSizeY = RealLy;
        COPYSizeX = RealLx;
        SCRSZY = RealLy;
        Pitch = RealLx;
        ScrWidth = RealLx;
        ScrHeight = RealLy;
        WindLx = RealLx;
        WindLy = RealLy;
        WindX = 0;
        WindY = 0;
        WindX1 = RealLx - 1;
        WindY1 = RealLy - 1;
        SVSC.SetSize(RealLx, RealLy);
    }

    gPrimaryTexture = SDL_CreateTexture(gRenderer, SDL_PIXELFORMAT_ARGB8888,
        SDL_TEXTUREACCESS_STREAMING, RSCRSizeX, RSCRSizeY);
    gBackTexture = SDL_CreateTexture(gRenderer, SDL_PIXELFORMAT_ARGB8888,
        SDL_TEXTUREACCESS_STREAMING, RSCRSizeX, RSCRSizeY);
    if (!gPrimaryTexture || !gBackTexture) {
        char errorMsg[256], convertedMsg[256];
        sprintf(errorMsg, "SDL_CreateTexture failed: %s", SDL_GetError());
        ConvertUTF8ToWindows1251(errorMsg, convertedMsg, 256);
        MessageBoxA(NULL, convertedMsg, "SDL Error", MB_OK | MB_ICONERROR);
        SDL_DestroyRenderer(gRenderer);
        SDL_DestroyWindow(gWindow);
        SDL_Quit();
        return false;
    }

    // Явная очистка текстур при возвращении в меню
    if (!InGame && !InEditor) {
        SDL_SetRenderTarget(gRenderer, gPrimaryTexture);
        SDL_SetRenderDrawColor(gRenderer, 0, 0, 0, 255);
        SDL_RenderClear(gRenderer);
        SDL_SetRenderTarget(gRenderer, gBackTexture);
        SDL_SetRenderDrawColor(gRenderer, 0, 0, 0, 255);
        SDL_RenderClear(gRenderer);
        SDL_SetRenderTarget(gRenderer, nullptr);
    }

    gPalette = SDL_AllocPalette(256);
    if (!gPalette) {
        char errorMsg[256], convertedMsg[256];
        sprintf(errorMsg, "SDL_AllocPalette failed: %s", SDL_GetError());
        ConvertUTF8ToWindows1251(errorMsg, convertedMsg, 256);
        MessageBoxA(NULL, convertedMsg, "SDL Error", MB_OK | MB_ICONERROR);
        SDL_DestroyTexture(gBackTexture);
        SDL_DestroyTexture(gPrimaryTexture);
        SDL_DestroyRenderer(gRenderer);
        SDL_DestroyWindow(gWindow);
        SDL_Quit();
        return false;
    }

    SCRSizeX = MaxSizeX;
    SCRSizeY = MaxSizeY;
    Pitch = SCRSizeX;
    BytesPerPixel = 1;
    size_t bufferSize = (size_t)SCRSizeX * (size_t)SCRSizeY;
    if (bufferSize == 0) {
        char errorMsg[256], convertedMsg[256];
        sprintf(errorMsg, "Invalid buffer size: SCRSizeX=%d, SCRSizeY=%d", SCRSizeX, SCRSizeY);
        ConvertUTF8ToWindows1251(errorMsg, convertedMsg, 256);
        MessageBoxA(NULL, convertedMsg, "SDL Error", MB_OK | MB_ICONERROR);
        SDL_DestroyTexture(gBackTexture);
        SDL_DestroyTexture(gPrimaryTexture);
        SDL_DestroyRenderer(gRenderer);
        SDL_DestroyWindow(gWindow);
        SDL_Quit();
        return false;
    }

    offScreenPtr = malloc(bufferSize);
    if (!offScreenPtr) {
        char errorMsg[256], convertedMsg[256];
        sprintf(errorMsg, "Failed to allocate offScreenPtr (size: %zu bytes)", bufferSize);
        ConvertUTF8ToWindows1251(errorMsg, convertedMsg, 256);
        MessageBoxA(NULL, convertedMsg, "SDL Error", MB_OK | MB_ICONERROR);
        SDL_DestroyTexture(gBackTexture);
        SDL_DestroyTexture(gPrimaryTexture);
        SDL_DestroyRenderer(gRenderer);
        SDL_DestroyWindow(gWindow);
        SDL_FreePalette(gPalette);
        SDL_Quit();
        return false;
    }

    InitRLCWindows();
    UpdatePaletteLUT();
    return true;
}

int CreateRGBDDObjects(HWND hwnd) {
    RealLx = 800;
    RealLy = 600;
    SCRSizeX = 800;
    SCRSizeY = 600;
    RSCRSizeX = 800;
    RSCRSizeY = 600;
    COPYSizeX = 800;
    SCRSZY = SCRSizeY;
    return CreateDDObjects(hwnd) ? 1 : 0;
}

int CreateRGB640DDObjects(HWND hwnd) {
    RealLx = 640;
    RealLy = 480;
    SCRSizeX = 640;
    SCRSizeY = 480;
    RSCRSizeX = 640;
    RSCRSizeY = 480;
    COPYSizeX = 640;
    SCRSZY = SCRSizeY;
    return CreateDDObjects(hwnd) ? 1 : 0;
}

void FreeDDObjects(void) {
    std::lock_guard<std::mutex> lock(renderMutex);
    if (offScreenPtr) {
        free(offScreenPtr);
        offScreenPtr = nullptr;
    }
    if (rgbBuffer) {
        free(rgbBuffer);
        rgbBuffer = nullptr;
    }
    if (gPrimaryTexture) {
        SDL_DestroyTexture(gPrimaryTexture);
        gPrimaryTexture = nullptr;
    }
    if (gBackTexture) {
        SDL_DestroyTexture(gBackTexture);
        gBackTexture = nullptr;
    }
    if (gRenderer) {
        SDL_DestroyRenderer(gRenderer);
        gRenderer = nullptr;
    }
    if (gWindow) {
        SDL_DestroyWindow(gWindow);
        gWindow = nullptr;
    }
    if (gPalette) {
        SDL_FreePalette(gPalette);
        gPalette = nullptr;
    }
    SDL_Quit();
}

void LoadPalette(LPCSTR lpFileName) {
    if (DDError) return;
    ResFile pf = RReset(lpFileName);
    if (pf != INVALID_HANDLE_VALUE) {
        for (int i = 0; i < 256; i++) {
            RBlockRead(pf, &GPal[i].r, 1);
            RBlockRead(pf, &GPal[i].g, 1);
            RBlockRead(pf, &GPal[i].b, 1);
            GPal[i].a = 255;
        }
        RClose(pf);
        if (!strcmp(lpFileName, "agew_1.pal")) {
            int C0 = 65;
            for (int i = 0; i < 12; i++) {
                int gray = 0;
                if (i > 2) gray = (i - 2) * 2;
                if (i > 7) gray += (i - 7) * 8;
                if (i > 9) gray += (i - 10) * 10;
                if (i > 10) gray += 50;
                gray = gray * 6 / 3;
                int rr = 0 * C0 / 150 + gray * 8 / 2;
                int gg = 80 * C0 / 150 + gray * 6 / 2;
                int bb = 132 * C0 / 150 + gray * 4 / 2;
                if (rr > 255) rr = 255;
                if (gg > 255) gg = 255;
                if (bb > 255) bb = 255;
                if (i < 5) {
                    rr -= (rr * (5 - i)) / 6;
                    gg -= (gg * (5 - i)) / 6;
                    bb -= (bb * (5 - i)) / 6;
                }
                if (i < 3) {
                    rr -= (rr * (3 - i)) / 4;
                    gg -= (gg * (3 - i)) / 4;
                    bb -= (bb * (3 - i)) / 4;
                }
                if (i < 2) {
                    rr -= (rr * (2 - i)) / 3;
                    gg -= (gg * (2 - i)) / 3;
                    bb -= (bb * (2 - i)) / 3;
                }
                GPal[0xB0 + i].r = rr;
                GPal[0xB0 + i].g = gg;
                GPal[0xB0 + i].b = bb;
                C0 += 5;
            }
            ResFile pf = RRewrite(lpFileName);
            for (int i = 0; i < 256; i++) {
                RBlockWrite(pf, &GPal[i].r, 1);
                RBlockWrite(pf, &GPal[i].g, 1);
                RBlockWrite(pf, &GPal[i].b, 1);
            }
            RClose(pf);
        }
        UpdatePaletteLUT();
    }
}

__declspec(dllexport) void SlowLoadPalette(LPCSTR lpFileName) {
    if (DDError) return;
    SDL_Color tempPal[256];
    ResFile pf = RReset(lpFileName);
    if (pf != INVALID_HANDLE_VALUE) {
        for (int i = 0; i < 256; i++) {
            RBlockRead(pf, &tempPal[i].r, 1);
            RBlockRead(pf, &tempPal[i].g, 1);
            RBlockRead(pf, &tempPal[i].b, 1);
            tempPal[i].a = 255;
        }
        RClose(pf);
        const int steps = 20;
        Uint32 startTime = SDL_GetTicks();
        for (int step = 0; step <= steps; step++) {
            Uint32 currentTime = SDL_GetTicks();
            float t = (float)(currentTime - startTime) / 400.0f;
            if (t > 1.0f) t = 1.0f;
            for (int i = 0; i < 256; i++) {
                GPal[i].r = GPal[i].r + (tempPal[i].r - GPal[i].r) * t;
                GPal[i].g = GPal[i].g + (tempPal[i].g - GPal[i].g) * t;
                GPal[i].b = GPal[i].b + (tempPal[i].b - GPal[i].b) * t;
            }
            UpdatePaletteLUT();
            FlipPages();
            if (t >= 1.0f) break;
            SDL_Delay(10);
        }
    }
}

__declspec(dllexport) void SlowUnLoadPalette(LPCSTR lpFileName) {
    if (DDError) return;
    SDL_Color tempPal[256];
    for (int i = 0; i < 256; i++) {
        tempPal[i].r = 0;
        tempPal[i].g = 0;
        tempPal[i].b = 0;
        tempPal[i].a = 255;
    }
    const int steps = 20;
    Uint32 startTime = SDL_GetTicks();
    for (int step = 0; step <= steps; step++) {
        Uint32 currentTime = SDL_GetTicks();
        float t = (float)(currentTime - startTime) / 400.0f;
        if (t > 1.0f) t = 1.0f;
        for (int i = 0; i < 256; i++) {
            GPal[i].r = GPal[i].r + (tempPal[i].r - GPal[i].r) * t;
            GPal[i].g = GPal[i].g + (tempPal[i].g - GPal[i].g) * t;
            GPal[i].b = GPal[i].b + (tempPal[i].b - GPal[i].b) * t;
        }
        UpdatePaletteLUT();
        FlipPages();
        if (t >= 1.0f) break;
        SDL_Delay(10);
    }
    LoadPalette(lpFileName);
}

void SetDarkPalette(void) {
    if (DDError) return;
    for (int i = 0; i < 256; i++) {
        GPal[i].r = GPal[i].r * 2 / 3;
        GPal[i].g = GPal[i].g * 2 / 3;
        GPal[i].b = GPal[i].b * 2 / 3;
    }
    UpdatePaletteLUT();
}

void SetDebugMode() {}
void NoDebugMode() {}