// ==============================================
// Эмулятор DirectSound на базе SDL2 Mixer
// MR.CODERMAN 2025
// ==============================================
#include "cdirsnd.h"
#include <stdio.h>
#include <math.h>
#include <windows.h>
#include <string>

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

static void CreateWAVHeader(LPWAVEFORMATEX format, DWORD dataSize, char* headerBuf, DWORD* headerSize) {
    *headerSize = 44;
    char* buf = headerBuf;

    memcpy(buf, "RIFF", 4); buf += 4;
    DWORD chunkSize = 36 + dataSize;
    memcpy(buf, &chunkSize, 4); buf += 4;
    memcpy(buf, "WAVE", 4); buf += 4;

    memcpy(buf, "fmt ", 4); buf += 4;
    DWORD subchunk1Size = 16;
    memcpy(buf, &subchunk1Size, 4); buf += 4;
    memcpy(buf, &format->wFormatTag, 2); buf += 2;
    memcpy(buf, &format->nChannels, 2); buf += 2;
    memcpy(buf, &format->nSamplesPerSec, 4); buf += 4;
    memcpy(buf, &format->nAvgBytesPerSec, 4); buf += 4;
    memcpy(buf, &format->nBlockAlign, 2); buf += 2;
    memcpy(buf, &format->wBitsPerSample, 2); buf += 2;

    memcpy(buf, "data", 4); buf += 4;
    memcpy(buf, &dataSize, 4); buf += 4;
}

void CDirSound::CreateDirSound(HWND hWnd) {
    m_hWindow = hWnd;
    m_pDirectSoundObj = NULL;
    m_currentBufferNum = 0;
    m_iniPath = "dd.ini";

    for (UINT x = 0; x < MAXSND1; ++x) {
        m_bufferPointers[x] = NULL;
        m_bufferSizes[x] = 0;
        BufIsRun[x] = 0;
        m_channels[x] = -1;
        Volume[x] = 0;
        SrcX[x] = 0;
        SrcY[x] = 0;
    }

    if (SDL_Init(SDL_INIT_AUDIO) < 0) {
        return;
    }

    if (Mix_OpenAudio(22050, AUDIO_S16SYS, 2, 2048) < 0) {
        SDL_Quit();
        return;
    }

    m_pDirectSoundObj = (void*)1;
    Mix_AllocateChannels(MAXSND1);
    InitAudio(m_iniPath);
}

void CDirSound::InitAudio(const std::string& iniPath) {
    int pos = GetPrivateProfileIntA("Audio Options", "MusicVolume", MaxSlider, iniPath.c_str());
    UpdateMusicVolume(pos);
}

void CDirSound::UpdateMusicVolume(int pos) {
    if (pos < 0) pos = 0;
    if (pos > MaxSlider) pos = MaxSlider;

    float f = static_cast<float>(pos) / MaxSlider;
    int sdlVol = static_cast<int>(f * SDL_MAX);
    Mix_VolumeMusic(sdlVol);
}

CDirSound::~CDirSound() {
    if (m_pDirectSoundObj != NULL)
        ReleaseAll();
}

void CDirSound::ReleaseAll() {
    for (UINT x = 1; x <= m_currentBufferNum; ++x) {
        if (m_bufferPointers[x]) {
            Mix_FreeChunk(m_bufferPointers[x]);
            m_bufferPointers[x] = NULL;
        }
        m_channels[x] = -1;
    }
    if (m_pDirectSoundObj) {
        Mix_CloseAudio();
        SDL_Quit();
        m_pDirectSoundObj = NULL;
    }
    memset(BufIsRun, 0, sizeof(BufIsRun));
}

UINT CDirSound::CreateSoundBuffer(CWave* pWave) {
    if (m_currentBufferNum == MAXSND)
        return 0;

    ++m_currentBufferNum;

    char* waveData = pWave->GetWaveDataPtr();
    DWORD waveSize = pWave->GetWaveSize();
    LPWAVEFORMATEX waveFormat = pWave->GetWaveFormatPtr();
    if (!waveData || waveSize == 0 || !waveFormat) {
        return 0;
    }

    char headerBuf[44];
    DWORD headerSize;
    CreateWAVHeader(waveFormat, waveSize, headerBuf, &headerSize);
    char* wavData = new char[headerSize + waveSize];
    memcpy(wavData, headerBuf, headerSize);
    memcpy(wavData + headerSize, waveData, waveSize);

    SDL_RWops* rw = SDL_RWFromMem(wavData, headerSize + waveSize);
    if (!rw) {
        delete[] wavData;
        return 0;
    }

    Mix_Chunk* chunk = Mix_LoadWAV_RW(rw, 0);
    SDL_RWclose(rw);
    delete[] wavData;
    if (!chunk) {
        return 0;
    }

    m_bufferPointers[m_currentBufferNum] = chunk;
    m_bufferSizes[m_currentBufferNum] = waveSize;
    Volume[m_currentBufferNum] = 0;
    return m_currentBufferNum;
}

UINT CDirSound::DuplicateSoundBuffer(UINT bufferNum) {
    if (m_currentBufferNum == MAXSND || bufferNum == 0 || bufferNum > m_currentBufferNum)
        return 0;

    ++m_currentBufferNum;
    Mix_Chunk* srcChunk = m_bufferPointers[bufferNum];
    if (!srcChunk)
        return 0;

    char* waveData = (char*)srcChunk->abuf;
    DWORD waveSize = srcChunk->alen;
    LPWAVEFORMATEX waveFormat = ((CWave*)srcChunk->allocated)->GetWaveFormatPtr();
    if (!waveFormat) {
        return 0;
    }

    char headerBuf[44];
    DWORD headerSize;
    CreateWAVHeader(waveFormat, waveSize, headerBuf, &headerSize);
    char* wavData = new char[headerSize + waveSize];
    memcpy(wavData, headerBuf, headerSize);
    memcpy(wavData + headerSize, waveData, waveSize);

    SDL_RWops* rw = SDL_RWFromMem(wavData, headerSize + waveSize);
    if (!rw) {
        delete[] wavData;
        return 0;
    }

    Mix_Chunk* chunk = Mix_LoadWAV_RW(rw, 0);
    SDL_RWclose(rw);
    delete[] wavData;
    if (!chunk) {
        return 0;
    }

    m_bufferPointers[m_currentBufferNum] = chunk;
    m_bufferSizes[m_currentBufferNum] = waveSize;
    Volume[m_currentBufferNum] = Volume[bufferNum];
    return m_currentBufferNum;
}

BOOL CDirSound::CopyWaveToBuffer(CWave* pWave, UINT bufferNum) {
    if (bufferNum > m_currentBufferNum || bufferNum == 0)
        return FALSE;

    if (m_bufferPointers[bufferNum])
        Mix_FreeChunk(m_bufferPointers[bufferNum]);

    char* waveData = pWave->GetWaveDataPtr();
    DWORD waveSize = pWave->GetWaveSize();
    LPWAVEFORMATEX waveFormat = pWave->GetWaveFormatPtr();
    if (!waveData || waveSize == 0 || !waveFormat) {
        return FALSE;
    }

    char headerBuf[44];
    DWORD headerSize;
    CreateWAVHeader(waveFormat, waveSize, headerBuf, &headerSize);
    char* wavData = new char[headerSize + waveSize];
    memcpy(wavData, headerBuf, headerSize);
    memcpy(wavData + headerSize, waveData, waveSize);

    SDL_RWops* rw = SDL_RWFromMem(wavData, headerSize + waveSize);
    if (!rw) {
        delete[] wavData;
        return FALSE;
    }

    Mix_Chunk* chunk = Mix_LoadWAV_RW(rw, 0);
    SDL_RWclose(rw);
    delete[] wavData;
    if (!chunk) {
        return FALSE;
    }

    m_bufferPointers[bufferNum] = chunk;
    m_bufferSizes[bufferNum] = waveSize;
    return TRUE;
}

BOOL CDirSound::DirectSoundOK() {
    return m_pDirectSoundObj != NULL;
}

BOOL CDirSound::PlaySound(UINT bufferNum) {
    if (bufferNum > m_currentBufferNum || bufferNum == 0)
        return FALSE;

    Mix_Chunk* chunk = m_bufferPointers[bufferNum];
    if (!chunk)
        return FALSE;

    int channel = Mix_PlayChannel(-1, chunk, 0);
    if (channel < 0) {
        return FALSE;
    }

    m_channels[bufferNum] = channel;
    BufIsRun[bufferNum] = 0;
    if (bufferNum != 1) {
        SetVolume(bufferNum, Volume[bufferNum]);
    }
    return TRUE;
}

BOOL CDirSound::PlayCoorSound(UINT bufferNum, int x, int vx) {
    if (bufferNum > m_currentBufferNum || bufferNum == 0)
        return FALSE;

    Mix_Chunk* chunk = m_bufferPointers[bufferNum];
    if (!chunk)
        return FALSE;

    int channel = Mix_PlayChannel(-1, chunk, 0);
    if (channel < 0) {
        return FALSE;
    }

    m_channels[bufferNum] = channel;
    BufIsRun[bufferNum] = 1;
    SrcX[bufferNum] = x;
    SrcY[bufferNum] = vx;
    if (bufferNum != 1) {
        SetVolume(bufferNum, Volume[bufferNum]);
    }
    ControlPan(bufferNum);
    return TRUE;
}

extern int CenterX;

void CDirSound::ControlPan(UINT bufferNum) {
    static Uint32 lastUpdate[MAXSND1] = { 0 };
    Uint32 currentTime = SDL_GetTicks();
    if (BufIsRun[bufferNum] && (currentTime - lastUpdate[bufferNum] > 100)) {
        SrcX[bufferNum] += SrcY[bufferNum];
        int pan = (SrcX[bufferNum] - CenterX) << 1;

        if (pan < -4000) pan = -4000;
        if (pan > 4000) pan = 4000;

        SetPan(bufferNum, pan);

        if (rand() < 350) {
            IsPlaying(bufferNum);
        }
        lastUpdate[bufferNum] = currentTime;
    }
}

void CDirSound::SetVolume(UINT bufferNum, int vol) {
    if (bufferNum > m_currentBufferNum || bufferNum == 0)
        return;

    if (vol < -10000) vol = -10000;
    if (vol > 0) vol = 0;

    int sdlVol = 0;
    if (vol > -10000) {
        double db = vol / 100.0;
        double linear = pow(10.0, db / 20.0);
        sdlVol = static_cast<int>(linear * 128.0);
        if (sdlVol < 0) sdlVol = 0;
        if (sdlVol > 128) sdlVol = 128;
        if (vol <= -4000) sdlVol = 0;
    }

    int channel = m_channels[bufferNum];
    if (channel >= 0) {
        Mix_Volume(channel, sdlVol);
    }

    Volume[bufferNum] = vol;
}

void CDirSound::SetPan(UINT bufferNum, int pan) {
    if (bufferNum > m_currentBufferNum || bufferNum == 0)
        return;

    if (pan < -10000) pan = -10000;
    if (pan > 10000) pan = 10000;

    int left, right;
    if (pan <= -10000) {
        left = 255; right = 0;
    }
    else if (pan >= 10000) {
        left = 0; right = 255;
    }
    else {
        double normalizedPan = (pan + 10000.0) / 20000.0;
        left = static_cast<int>(255.0 * (1.0 - normalizedPan));
        right = static_cast<int>(255.0 * normalizedPan);
    }

    int channel = m_channels[bufferNum];
    if (channel >= 0) {
        Mix_SetPanning(channel, left, right);
    }
}

void CDirSound::ProcessSoundSystem() {
    for (int i = 0; i < MAXSND1; i++) {
        if (BufIsRun[i]) {
            ControlPan(i);
        }
    }
}

BOOL CDirSound::StopSound(UINT bufferNum) {
    if (bufferNum > m_currentBufferNum || bufferNum == 0)
        return FALSE;

    int channel = m_channels[bufferNum];
    if (channel >= 0) {
        Mix_HaltChannel(channel);
        m_channels[bufferNum] = -1;
    }
    BufIsRun[bufferNum] = 0;
    return TRUE;
}

int CDirSound::GetPos(UINT bufferNum) {
    return 0;
}

bool CDirSound::IsPlaying(UINT bufferNum) {
    if (bufferNum > m_currentBufferNum || bufferNum == 0)
        return false;

    int channel = m_channels[bufferNum];
    if (channel < 0)
        return false;

    return Mix_Playing(channel) != 0;
}

void CDirSound::SetGlobalSoundVolume(int vol) {
    if (vol < -10000) vol = -10000;
    if (vol > 0) vol = 0;

    for (UINT i = 2; i <= m_currentBufferNum; ++i) {
        Volume[i] = vol;
        if (IsPlaying(i)) {
            SetVolume(i, vol);
        }
    }
}

void CDirSound::SetGlobalMusicVolume(int vol) {
    WritePrivateProfileStringA("Audio Options", "MusicVolume", std::to_string(vol).c_str(), m_iniPath.c_str());
    UpdateMusicVolume(vol);
}

void CDirSound::SetLastVolume(short Vol) {
    if (m_currentBufferNum != 1) {
        Volume[m_currentBufferNum] = Vol;
    }
}