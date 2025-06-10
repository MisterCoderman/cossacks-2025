// ==============================================
// Ёмул€тор DirectSound на базе SDL2 Mixer
// MR.CODERMAN 2025
// ==============================================
#ifndef __CDIRSND_H
#define __CDIRSND_H

#include <windows.h>
#include "cwave.h"
#include <mmsystem.h>
#include <SDL.h>
#include <SDL_mixer.h>
#include <string>

#define MAXSND 600
#define MAXSND1 601
#define MaxSlider 1000
#define SDL_MAX 128

class CDirSound {
protected:
    void* m_pDirectSoundObj; // SDL audio context (opaque)
    HWND m_hWindow;
    Mix_Chunk* m_bufferPointers[MAXSND1]; // SDL_mixer sound chunks
    DWORD m_bufferSizes[MAXSND1];
    int m_channels[MAXSND1];

public:
    short Volume[MAXSND1];
    short SrcX[MAXSND1];
    short SrcY[MAXSND1];
    byte BufIsRun[MAXSND1];
    UINT m_currentBufferNum;
    void CreateDirSound(HWND hWnd); // Reverted to single argument
    ~CDirSound();
    UINT CreateSoundBuffer(CWave* pWave);
    UINT DuplicateSoundBuffer(UINT bufferNum);
    BOOL DirectSoundOK();
    void SetLastVolume(short Vol);
    BOOL CopyWaveToBuffer(CWave* pWave, UINT bufferNum);
    void SetVolume(UINT bufferNum, int vol);
    void SetPan(UINT bufferNum, int pan);
    BOOL PlaySound(UINT bufferNum);
    BOOL StopSound(UINT bufferNum);
    BOOL PlayCoorSound(UINT bufferNum, int x, int vx);
    void ControlPan(UINT bufferNum);
    bool IsPlaying(UINT bufferNum);
    int GetPos(UINT bufferNum);
    void ProcessSoundSystem();
    void SetGlobalSoundVolume(int vol);
    void SetGlobalMusicVolume(int vol);
    void UpdateMusicVolume(int pos);
    void InitAudio(const std::string& iniPath);

protected:
    void ReleaseAll();
    std::string m_iniPath;
};

#endif

#define MaxSnd 1024