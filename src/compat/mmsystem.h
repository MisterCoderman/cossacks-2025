// Stub for <mmsystem.h> on non-Windows platforms
#ifndef _COMPAT_MMSYSTEM_H_
#define _COMPAT_MMSYSTEM_H_

#include "windows.h"

typedef UINT MMRESULT;
#define TIMERR_NOERROR 0
#define MMSYSERR_NOERROR 0
#define TIME_PERIODIC  0x0001

inline MMRESULT timeSetEvent(UINT delay, UINT res, void* proc, DWORD user, UINT event) {
    (void)delay; (void)res; (void)proc; (void)user; (void)event; return 0;
}
inline MMRESULT timeKillEvent(UINT id) { (void)id; return 0; }
inline MMRESULT timeBeginPeriod(UINT period) { (void)period; return 0; }
inline MMRESULT timeEndPeriod(UINT period) { (void)period; return 0; }
inline DWORD timeGetTime() { return 0; }

// ---- MMIO stubs ----
typedef HANDLE HMMIO;
#define MMIO_READ       0x00000000
#define MMIO_ALLOCBUF   0x00010000
#define MMIO_FINDRIFF   0x00000020
#define MMIO_FINDCHUNK  0x00000010

#define mmioFOURCC(ch0, ch1, ch2, ch3) MAKEFOURCC(ch0, ch1, ch2, ch3)

typedef struct {
    FOURCC ckid;
    DWORD  cksize;
    FOURCC fccType;
    DWORD  dwDataOffset;
    DWORD  dwFlags;
} MMCKINFO;

// ---- Real MMIO implementation for WAV file reading ----
// HMMIO wraps a FILE*
inline HMMIO mmioOpenA(LPSTR szFilename, void* lpmmioinfo, DWORD dwOpenFlags) {
    (void)lpmmioinfo; (void)dwOpenFlags;
    if (!szFilename) return NULL;
    // Convert backslashes to forward slashes for macOS/Linux
    char fixedPath[512];
    strncpy(fixedPath, szFilename, sizeof(fixedPath) - 1);
    fixedPath[sizeof(fixedPath) - 1] = 0;
    for (char* p = fixedPath; *p; p++) {
        if (*p == '\\') *p = '/';
    }
    FILE* f = fopen(fixedPath, "rb");
    return (HMMIO)f;
}
#define mmioOpen mmioOpenA

inline MMRESULT mmioDescend(HMMIO hmmio, MMCKINFO* lpck, const MMCKINFO* lpckParent, UINT wFlags) {
    FILE* f = (FILE*)hmmio;
    if (!f || !lpck) return MMSYSERR_NOERROR + 1;

    if (wFlags & MMIO_FINDRIFF) {
        // Search for RIFF chunk with matching fccType
        fseek(f, 0, SEEK_SET);
        FOURCC ckid, cksize, fccType;
        if (fread(&ckid, 4, 1, f) != 1) return MMSYSERR_NOERROR + 1;
        if (fread(&cksize, 4, 1, f) != 1) return MMSYSERR_NOERROR + 1;
        if (fread(&fccType, 4, 1, f) != 1) return MMSYSERR_NOERROR + 1;
        if (ckid != mmioFOURCC('R','I','F','F') || fccType != lpck->fccType)
            return MMSYSERR_NOERROR + 1;
        lpck->ckid = ckid;
        lpck->cksize = cksize;
        lpck->dwDataOffset = 12; // data starts after RIFF header
        return MMSYSERR_NOERROR;
    }
    if (wFlags & MMIO_FINDCHUNK) {
        // Search for a sub-chunk within parent
        DWORD parentEnd = lpckParent ? (lpckParent->dwDataOffset + lpckParent->cksize) : 0xFFFFFFFF;
        DWORD pos = (DWORD)ftell(f);
        while (pos < parentEnd) {
            FOURCC ckid;
            DWORD cksize;
            if (fread(&ckid, 4, 1, f) != 1) return MMSYSERR_NOERROR + 1;
            if (fread(&cksize, 4, 1, f) != 1) return MMSYSERR_NOERROR + 1;
            if (ckid == lpck->ckid) {
                lpck->cksize = cksize;
                lpck->dwDataOffset = (DWORD)ftell(f);
                return MMSYSERR_NOERROR;
            }
            // Skip this chunk (align to 2-byte boundary)
            DWORD skip = (cksize + 1) & ~1;
            fseek(f, skip, SEEK_CUR);
            pos = (DWORD)ftell(f);
        }
        return MMSYSERR_NOERROR + 1; // chunk not found
    }
    return MMSYSERR_NOERROR + 1;
}

inline MMRESULT mmioAscend(HMMIO hmmio, MMCKINFO* lpck, UINT wFlags) {
    (void)wFlags;
    FILE* f = (FILE*)hmmio;
    if (!f || !lpck) return MMSYSERR_NOERROR;
    // Seek past the end of this chunk
    DWORD endPos = lpck->dwDataOffset + ((lpck->cksize + 1) & ~1);
    fseek(f, endPos, SEEK_SET);
    return MMSYSERR_NOERROR;
}

inline LONG mmioRead(HMMIO hmmio, char* pch, LONG cch) {
    FILE* f = (FILE*)hmmio;
    if (!f || !pch || cch <= 0) return 0;
    return (LONG)fread(pch, 1, cch, f);
}

inline MMRESULT mmioClose(HMMIO hmmio, UINT wFlags) {
    (void)wFlags;
    FILE* f = (FILE*)hmmio;
    if (f) fclose(f);
    return MMSYSERR_NOERROR;
}

// ---- MCI stubs ----
typedef UINT MCIDEVICEID;
typedef DWORD MCIERROR;

// MCI command message stubs
#define MCI_OPEN        0x0803
#define MCI_CLOSE       0x0804
#define MCI_PLAY        0x0806
#define MCI_STOP        0x0808
#define MCI_SET         0x080D
#define MCI_STATUS      0x0814
#define MCI_OPEN_TYPE           0x00002000L
#define MCI_OPEN_ELEMENT        0x00000200L
#define MCI_SET_TIME_FORMAT     0x00000400L
#define MCI_FORMAT_TMSF         2
#define MCI_FORMAT_MILLISECONDS 0
#define MCI_STATUS_ITEM         0x00000100L
#define MCI_STATUS_NUMBER_OF_TRACKS 3
#define MCI_STATUS_LENGTH       1
#define MCI_STATUS_POSITION     2
#define MCI_TRACK               0x00000010L
#define MCI_FROM                0x00000004L
#define MCI_TO                  0x00000008L
#define MCI_NOTIFY              0x00000001L
#define MCI_DEVTYPE_CD_AUDIO    ((WORD)516)
#define MCI_WAIT                0x00000002L
#define MCI_NOTIFY_SUCCESSFUL   0x0001

#define MCI_MAKE_TMSF(t,m,s,f) ((DWORD)(((BYTE)(t))|((WORD)(m)<<8)|((WORD)(s)<<16)|((WORD)(f)<<24)))
#define MCI_TMSF_TRACK(tmsf) ((BYTE)(tmsf))
#define MCI_TMSF_MINUTE(tmsf) ((BYTE)(((WORD)(tmsf)) >> 8))
#define MCI_TMSF_SECOND(tmsf) ((BYTE)((tmsf)>>16))

typedef struct {
    DWORD  dwCallback;
    MCIDEVICEID wDeviceID;
    LPCSTR lpstrDeviceType;
    LPCSTR lpstrElementName;
    LPCSTR lpstrAlias;
} MCI_OPEN_PARMSA, *LPMCI_OPEN_PARMSA;
#define MCI_OPEN_PARMS MCI_OPEN_PARMSA

typedef struct {
    DWORD dwCallback;
    DWORD dwFrom;
    DWORD dwTo;
} MCI_PLAY_PARMS;

typedef struct {
    DWORD dwCallback;
    DWORD dwTimeFormat;
    DWORD dwAudio;
} MCI_SET_PARMS;

typedef struct {
    DWORD dwCallback;
    DWORD dwReturn;
    DWORD dwItem;
    DWORD dwTrack;
} MCI_STATUS_PARMS;

typedef struct {
    DWORD dwCallback;
} MCI_GENERIC_PARMS;

inline MCIERROR mciSendCommandA(MCIDEVICEID id, UINT msg, DWORD flags, DWORD param) {
    (void)id; (void)msg; (void)flags; (void)param; return 0;
}
#define mciSendCommand mciSendCommandA

// ---- Mixer stubs ----
typedef HANDLE HMIXER;
typedef HMIXER* LPHMIXER;

typedef struct {
    DWORD cbStruct;
    DWORD dwControlID;
    DWORD dwControlType;
    DWORD cChannels;
    union {
        DWORD cSteps;
        DWORD cbDetails;
        DWORD reserved[6];
    } Metrics;
    CHAR szShortName[16];
    CHAR szName[64];
    DWORD dwMinimum;
    DWORD dwMaximum;
    DWORD fdwControl;
} MIXERCONTROLA, *LPMIXERCONTROLA;
#define MIXERCONTROL MIXERCONTROLA

typedef struct {
    DWORD cbStruct;
    DWORD dwControlID;
    DWORD cChannels;
    union {
        HWND hwndOwner;
        DWORD cMultipleItems;
    };
    DWORD cbDetails;
    void* paDetails;
} MIXERCONTROLDETAILS;

typedef struct {
    LONG  fValue;
} MIXERCONTROLDETAILS_BOOLEAN;

typedef struct {
    DWORD dwValue;
} MIXERCONTROLDETAILS_UNSIGNED;

typedef struct {
    DWORD cbStruct;
    DWORD dwDestination;
    DWORD dwSource;
    DWORD dwLineID;
    DWORD fdwLine;
    DWORD dwUser;
    DWORD dwComponentType;
    DWORD cChannels;
    DWORD cConnections;
    DWORD cControls;
    CHAR  szShortName[16];
    CHAR  szName[64];
    struct {
        DWORD dwType;
        DWORD dwDeviceID;
        WORD  wMid;
        WORD  wPid;
        WORD  vDriverVersion;
        CHAR  szPname[32];
    } Target;
} MIXERLINEA, *LPMIXERLINEA;
#define MIXERLINE MIXERLINEA

typedef struct {
    DWORD cbStruct;
    DWORD dwLineID;
    DWORD dwControlType;
    DWORD cControls;
    DWORD cbmxctrl;
    LPMIXERCONTROLA pamxctrl;
} MIXERLINECONTROLSA, *LPMIXERLINECONTROLSA;
#define MIXERLINECONTROLS MIXERLINECONTROLSA

#define MIXER_GETLINEINFOF_COMPONENTTYPE  0x00000003L
#define MIXER_GETLINECONTROLSF_ONEBYTYPE  0x00000002L
#define MIXER_GETCONTROLDETAILSF_VALUE    0x00000000L
#define MIXER_SETCONTROLDETAILSF_VALUE    0x00000000L
#define MIXERLINE_COMPONENTTYPE_DST_SPEAKERS 0x00000004L
#define MIXERCONTROL_CONTROLTYPE_VOLUME      0x50030001L
#define MIXERCONTROL_CONTROLTYPE_MUTE        0x20010002L

inline MMRESULT mixerOpen(LPHMIXER phmx, UINT uMxId, DWORD dwCallback, DWORD dwInstance, DWORD fdwOpen) {
    (void)phmx; (void)uMxId; (void)dwCallback; (void)dwInstance; (void)fdwOpen; return MMSYSERR_NOERROR;
}
inline MMRESULT mixerClose(HMIXER hmx) { (void)hmx; return MMSYSERR_NOERROR; }
inline MMRESULT mixerGetLineInfoA(HMIXER hmx, LPMIXERLINEA pmxl, DWORD fdwInfo) {
    (void)hmx; (void)pmxl; (void)fdwInfo; return MMSYSERR_NOERROR;
}
#define mixerGetLineInfo mixerGetLineInfoA
inline MMRESULT mixerGetLineControlsA(HMIXER hmx, LPMIXERLINECONTROLSA pmxlc, DWORD fdwControls) {
    (void)hmx; (void)pmxlc; (void)fdwControls; return MMSYSERR_NOERROR;
}
#define mixerGetLineControls mixerGetLineControlsA
inline MMRESULT mixerGetControlDetailsA(HMIXER hmx, MIXERCONTROLDETAILS* pmxcd, DWORD fdwDetails) {
    (void)hmx; (void)pmxcd; (void)fdwDetails; return MMSYSERR_NOERROR;
}
#define mixerGetControlDetails mixerGetControlDetailsA
inline MMRESULT mixerSetControlDetails(HMIXER hmx, MIXERCONTROLDETAILS* pmxcd, DWORD fdwDetails) {
    (void)hmx; (void)pmxcd; (void)fdwDetails; return MMSYSERR_NOERROR;
}

// ---- Additional mixer stubs ----
typedef HMIXER HMIXEROBJ;
#define MIXER_OBJECTF_HMIXER        0x80000000L
#define MIXER_GETLINEINFOF_SOURCE   0x00000001L
#define CALLBACK_NULL               0x00000000L
#define CALLBACK_WINDOW             0x00010000L
#define MIXERLINE_COMPONENTTYPE_SRC_LAST 0x00001008L
#define MIXERLINE_COMPONENTTYPE_SRC_WAVEOUT 0x00001000L

inline UINT mixerGetNumDevs() { return 0; }

// Fix MIXERCONTROLDETAILS to have dwControlID
#undef MIXERCONTROLDETAILS_DEFINED

// ---- GlobalAllocPtr / GlobalFreePtr ----
#define GMEM_MOVEABLE 0x0002
#define GMEM_SHARE    0x2000
#define GlobalAllocPtr(flags, cb) malloc(cb)
#define GlobalFreePtr(lp) free(lp)

#endif
