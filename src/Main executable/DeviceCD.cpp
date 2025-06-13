// ==============================================
// Эмулятор MCI на базе SDL2 Mixer
// MR.CODERMAN 2025
// ==============================================
#include "windows.h"
#pragma pack(1)
#include "DeviceCD.h"
#include "TMixer.h"
#include <stdio.h>
#include "ResFile.h"
#include "gFile.h"
#include <SDL_mixer.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// Глобальный экземпляр проигрывателя
CDeviceCD CDPLAY;

// Конфигурация треков
int StartTrack = 2;
int NTracks = 19;
byte TracksMask[32] = { 2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20 };

// Буферы загруженных WAV
static Mix_Chunk* chunkTracks[32] = { nullptr };
static int currentChannel = -1;
static DWORD currentVolume = 5000;
static bool SDL_Mixer_Initialized = false;

// Переменные для плейлиста
static int PrevTrack1 = -1, PrevTrack2 = -1, PrevTrack3 = -1;
static int NextCommand = -1;

// Предварительные объявления
static void channelFinished(int channel);
void PlayCDTrack(int Id);
void PlayRandomTrack();
extern HWND hwnd;
extern int CurrentNation, PlayMode;

/////////////////////////////////////////////////////////////////////////////
// Реализация CDeviceCD

CDeviceCD::CDeviceCD()
{
    // Инициализация SDL_mixer (один раз)
    if (!SDL_Mixer_Initialized) {
        if (Mix_OpenAudio(22050, AUDIO_S16SYS, 2, 4096) < 0) {
            MessageBox(hwnd, Mix_GetError(), "Mix_OpenAudio failed", MB_ICONERROR);
        }
        Mix_ChannelFinished(channelFinished);
        SDL_Mixer_Initialized = true;
    }

    // Загрузка конфигурации треков
    if (GFILE* f = Gopen("Tracks.cd", "r")) {
        Gscanf(f, "%d%d", &StartTrack, &NTracks);
        for (int i = 0; i < NTracks; ++i)
            Gscanf(f, "%d", TracksMask + i);
        Gclose(f);
    }

    Open();
}

CDeviceCD::~CDeviceCD()
{
    Close();
}

bool CDeviceCD::Open()
{
    int loadedCount = 0;
    FOpened = FALSE;
    for (int i = 0; i < NTracks; ++i) {
        int trackNum = StartTrack + i;
        char filename[260];
        sprintf(filename, "Tracks\\Track_%d.wav", trackNum);

        // Если ранее уже загружен, освободить
        if (chunkTracks[i]) {
            Mix_FreeChunk(chunkTracks[i]);
            chunkTracks[i] = nullptr;
        }

        // Простой вызов загрузки WAV
        chunkTracks[i] = Mix_LoadWAV(filename);
        if (chunkTracks[i]) {
            loadedCount++;
        }
    }

    // Успешно, если хотя бы один трек загружен
    FOpened = (loadedCount > 0);
    return FOpened;
}

bool CDeviceCD::Close()
{
    if (!FOpened)
        return FALSE;

    // Остановка всех каналов и очистка
    Mix_HaltChannel(-1);
    for (int i = 0; i < NTracks; ++i) {
        if (chunkTracks[i]) {
            Mix_FreeChunk(chunkTracks[i]);
            chunkTracks[i] = nullptr;
        }
    }

    FOpened = FALSE;
    return TRUE;
}

bool CDeviceCD::Pause()
{
    if (FOpened && currentChannel != -1) {
        Mix_Pause(currentChannel);
        return TRUE;
    }
    return FALSE;
}

bool CDeviceCD::Resume()
{
    if (FOpened && currentChannel != -1) {
        Mix_Resume(currentChannel);
        return TRUE;
    }
    return FALSE;
}

bool CDeviceCD::Stop()
{
    if (FOpened && currentChannel != -1) {
        Mix_HaltChannel(currentChannel);
        currentChannel = -1;
        return TRUE;
    }
    return FALSE;
}

DWORD CDeviceCD::GetVolume()
{
    return currentVolume;
}

bool CDeviceCD::SetVolume(DWORD Volume)
{
    currentVolume = Volume;
    int sdlVol = (Volume * MIX_MAX_VOLUME) / 10000;
    Mix_Volume(-1, sdlVol);
    return TRUE;
}

bool CDeviceCD::Play(DWORD Track)
{
    if (!FOpened)
        return FALSE;

    if (Track < StartTrack || Track >= StartTrack + NTracks)
        return FALSE;

    int idx = Track - StartTrack;
    if (!chunkTracks[idx])
        return FALSE;

    // Остановка предыдущего трека
    if (currentChannel != -1)
        Mix_HaltChannel(currentChannel);

    // Воспроизведение нового трека
    currentChannel = Mix_PlayChannel(-1, chunkTracks[idx], 0);
    return (currentChannel >= 0);
}

/////////////////////////////////////////////////////////////////////////////
// Callback при окончании канала

static void channelFinished(int channel)
{
    if (channel != currentChannel)
        return;

    if (!PlayMode)
    {
        NextCommand = -1;
        return;
    }

    if (NextCommand == -1)
    {
        PlayRandomTrack();
    }
    else if (NextCommand >= 1000)
    {
        PlayCDTrack(NextCommand - 1000);
        NextCommand = -1;
    }
    else
    {
        PlayCDTrack(NextCommand);
        NextCommand = -1;
    }
}


/////////////////////////////////////////////////////////////////////////////
// Глобальные функции

void PlayCDTrack(int Id)
{
    CDPLAY.Play(Id);
}

void PlayRandomTrack()
{
    if (PlayMode == 1 && CurrentNation != -1) {
        PlayCDTrack(TracksMask[CurrentNation]);
        return;
    }

    int Track;
    do {
        Track = (((GetTickCount() & 4095) * NTracks) >> 12) + StartTrack;
    } while (Track == PrevTrack1 || Track == PrevTrack2 || Track == PrevTrack3);

    PrevTrack3 = PrevTrack2;
    PrevTrack2 = PrevTrack1;
    PrevTrack1 = Track;

    PlayCDTrack(Track);
}

void StopPlayCD()
{
    CDPLAY.Stop();
    NextCommand = -1;
}

int GetCDVolume()
{
    return CDPLAY.GetVolume() / 10;
}

void SetCDVolume(int Vol)
{
    CDPLAY.SetVolume(Vol * 10);
}

// Legacy callback for MCINotify
LRESULT CD_MCINotify(UINT wFlags, LONG lDevId)
{
    return 0;
}