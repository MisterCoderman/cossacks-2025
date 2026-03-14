// ==============================================
// Real POSIX implementations of Win32 file I/O
// for non-Windows platforms.
// Included by compat/windows.h
// ==============================================
#ifndef _COMPAT_FILE_IO_H_
#define _COMPAT_FILE_IO_H_

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#include <dirent.h>
#include <fnmatch.h>
#include <cstring>

// ---- Handle management ----
// We need to distinguish file descriptors from other handle types.
// Use a tagged wrapper: store (fd + 1) shifted, with a magic tag.
// This is internal to the compat layer.

#define _COMPAT_FILE_TAG 0x46494C45  // 'FILE'
#define _COMPAT_MMAP_TAG 0x4D4D4150  // 'MMAP'

struct _compat_handle_info {
    uint32_t tag;
    int fd;
    void* mmap_ptr;
    size_t mmap_size;
};

// We use a simple array to track handles.
// This is not thread-safe but matches the original game's single-threaded file I/O.
#define _COMPAT_MAX_HANDLES 256
static _compat_handle_info _compat_handles[_COMPAT_MAX_HANDLES];
static bool _compat_handles_init = false;

inline void _compat_init_handles() {
    if (!_compat_handles_init) {
        memset(_compat_handles, 0, sizeof(_compat_handles));
        _compat_handles_init = true;
    }
}

inline HANDLE _compat_alloc_file_handle(int fd) {
    _compat_init_handles();
    for (int i = 0; i < _COMPAT_MAX_HANDLES; i++) {
        if (_compat_handles[i].tag == 0) {
            _compat_handles[i].tag = _COMPAT_FILE_TAG;
            _compat_handles[i].fd = fd;
            _compat_handles[i].mmap_ptr = NULL;
            _compat_handles[i].mmap_size = 0;
            return (HANDLE)&_compat_handles[i];
        }
    }
    return INVALID_HANDLE_VALUE;
}

inline _compat_handle_info* _compat_get_handle(HANDLE h) {
    if (h == NULL || h == INVALID_HANDLE_VALUE) return NULL;
    _compat_handle_info* info = (_compat_handle_info*)h;
    if (info->tag == _COMPAT_FILE_TAG || info->tag == _COMPAT_MMAP_TAG)
        return info;
    return NULL;
}

// ---- CreateFileA ----
#undef CreateFile
#undef CreateFileA
inline HANDLE CreateFileA(LPCSTR name, DWORD access, DWORD share, void* sa,
                          DWORD disp, DWORD flags, HANDLE templ) {
    (void)share; (void)sa; (void)flags; (void)templ;
    int oflags = 0;

    // Access mode
    if ((access & GENERIC_READ) && (access & GENERIC_WRITE))
        oflags = O_RDWR;
    else if (access & GENERIC_WRITE)
        oflags = O_WRONLY;
    else
        oflags = O_RDONLY;

    // Disposition
    switch (disp) {
        case CREATE_ALWAYS:   oflags |= O_CREAT | O_TRUNC; break;
        case CREATE_NEW:      oflags |= O_CREAT | O_EXCL; break;
        case OPEN_ALWAYS:     oflags |= O_CREAT; break;
        case OPEN_EXISTING:   break; // no extra flags
        case TRUNCATE_EXISTING: oflags |= O_TRUNC; break;
    }

    int fd = open(name, oflags, 0666);
    if (fd < 0)
        return INVALID_HANDLE_VALUE;

    return _compat_alloc_file_handle(fd);
}
#define CreateFile CreateFileA

// ---- ReadFile ----
#undef ReadFile
inline BOOL ReadFile(HANDLE h, LPVOID buf, DWORD bytes, LPDWORD bytesRead, void* overlapped) {
    (void)overlapped;
    _compat_handle_info* info = _compat_get_handle(h);
    if (!info) return FALSE;
    ssize_t result = read(info->fd, buf, bytes);
    if (result < 0) {
        if (bytesRead) *bytesRead = 0;
        return FALSE;
    }
    if (bytesRead) *bytesRead = (DWORD)result;
    return TRUE;
}

// ---- WriteFile ----
#undef WriteFile
inline BOOL WriteFile(HANDLE h, LPCVOID buf, DWORD bytes, LPDWORD bytesWritten, void* overlapped) {
    (void)overlapped;
    _compat_handle_info* info = _compat_get_handle(h);
    if (!info) return FALSE;
    ssize_t result = write(info->fd, buf, bytes);
    if (result < 0) {
        if (bytesWritten) *bytesWritten = 0;
        return FALSE;
    }
    if (bytesWritten) *bytesWritten = (DWORD)result;
    return TRUE;
}

// ---- SetFilePointer ----
#undef SetFilePointer
inline DWORD SetFilePointer(HANDLE h, LONG dist, LPLONG high, DWORD method) {
    (void)high;
    _compat_handle_info* info = _compat_get_handle(h);
    if (!info) return (DWORD)-1;
    int whence = SEEK_SET;
    if (method == FILE_CURRENT) whence = SEEK_CUR;
    else if (method == FILE_END) whence = SEEK_END;
    off_t result = lseek(info->fd, dist, whence);
    if (result < 0) return (DWORD)-1;
    return (DWORD)result;
}

// ---- GetFileSize ----
#undef GetFileSize
inline DWORD GetFileSize(HANDLE h, LPDWORD high) {
    if (high) *high = 0;
    _compat_handle_info* info = _compat_get_handle(h);
    if (!info) return (DWORD)-1;
    struct stat st;
    if (fstat(info->fd, &st) < 0) return (DWORD)-1;
    return (DWORD)st.st_size;
}

// ---- CloseHandle ----
#undef CloseHandle
inline BOOL CloseHandle(HANDLE h) {
    _compat_handle_info* info = _compat_get_handle(h);
    if (!info) return TRUE; // non-file handle, just ignore
    if (info->mmap_ptr) {
        munmap(info->mmap_ptr, info->mmap_size);
        info->mmap_ptr = NULL;
    }
    if (info->fd >= 0) {
        close(info->fd);
        info->fd = -1;
    }
    info->tag = 0; // free the slot
    return TRUE;
}

// ---- CreateFileMapping / MapViewOfFile / UnmapViewOfFile ----
#undef CreateFileMapping
#undef CreateFileMappingA
inline HANDLE CreateFileMappingA(HANDLE hFile, void* lpAttr, DWORD flProtect,
                                  DWORD dwMaxHigh, DWORD dwMaxLow, LPCSTR lpName) {
    (void)lpAttr; (void)flProtect; (void)dwMaxHigh; (void)dwMaxLow; (void)lpName;
    _compat_handle_info* fileInfo = _compat_get_handle(hFile);
    if (!fileInfo) return NULL;

    struct stat st;
    if (fstat(fileInfo->fd, &st) < 0) return NULL;

    // Allocate a mapping handle that references the file
    _compat_init_handles();
    for (int i = 0; i < _COMPAT_MAX_HANDLES; i++) {
        if (_compat_handles[i].tag == 0) {
            _compat_handles[i].tag = _COMPAT_MMAP_TAG;
            _compat_handles[i].fd = fileInfo->fd; // reference the same fd
            _compat_handles[i].mmap_ptr = NULL;
            _compat_handles[i].mmap_size = (size_t)st.st_size;
            return (HANDLE)&_compat_handles[i];
        }
    }
    return NULL;
}
#define CreateFileMapping CreateFileMappingA

#undef MapViewOfFile
inline LPVOID MapViewOfFile(HANDLE hMap, DWORD dwAccess, DWORD dwOffHigh,
                             DWORD dwOffLow, size_t dwBytes) {
    (void)dwOffHigh; (void)dwOffLow;
    _compat_handle_info* info = _compat_get_handle(hMap);
    if (!info || info->tag != _COMPAT_MMAP_TAG) return NULL;

    int prot = PROT_READ;
    if (dwAccess & FILE_MAP_WRITE) prot |= PROT_WRITE;

    size_t mapSize = (dwBytes > 0) ? dwBytes : info->mmap_size;
    void* ptr = mmap(NULL, mapSize, prot, MAP_PRIVATE, info->fd, 0);
    if (ptr == MAP_FAILED) return NULL;

    info->mmap_ptr = ptr;
    info->mmap_size = mapSize;
    return ptr;
}

#undef UnmapViewOfFile
inline BOOL UnmapViewOfFile(LPCVOID lpBase) {
    // Find the handle that owns this mapping
    _compat_init_handles();
    for (int i = 0; i < _COMPAT_MAX_HANDLES; i++) {
        if (_compat_handles[i].tag == _COMPAT_MMAP_TAG &&
            _compat_handles[i].mmap_ptr == lpBase) {
            munmap(_compat_handles[i].mmap_ptr, _compat_handles[i].mmap_size);
            _compat_handles[i].mmap_ptr = NULL;
            _compat_handles[i].tag = 0; // free slot
            return TRUE;
        }
    }
    return TRUE;
}

// ---- FindFirstFile / FindNextFile / FindClose ----
// Use a struct to hold opendir state

struct _compat_find_data {
    uint32_t tag;
    DIR* dir;
    char pattern[MAX_PATH]; // just the filename pattern (e.g. "*.gsc")
    char dirpath[MAX_PATH]; // directory part
};

#define _COMPAT_FIND_TAG 0x46494E44 // 'FIND'

inline bool _compat_match_pattern(const char* name, const char* pattern) {
    return fnmatch(pattern, name, FNM_CASEFOLD) == 0;
}

#undef FindFirstFile
#undef FindFirstFileA
inline HANDLE FindFirstFileA(LPCSTR path, LPWIN32_FIND_DATAA data) {
    if (!path || !data) return INVALID_HANDLE_VALUE;

    // Split path into directory and pattern
    char dirpart[MAX_PATH] = ".";
    char filepart[MAX_PATH];
    strncpy(filepart, path, MAX_PATH - 1);
    filepart[MAX_PATH - 1] = 0;

    // Find last separator
    const char* lastSlash = strrchr(path, '/');
    const char* lastBack = strrchr(path, '\\');
    const char* sep = lastSlash;
    if (lastBack && (!sep || lastBack > sep)) sep = lastBack;

    if (sep) {
        size_t dirLen = sep - path;
        if (dirLen >= MAX_PATH) dirLen = MAX_PATH - 1;
        strncpy(dirpart, path, dirLen);
        dirpart[dirLen] = 0;
        strncpy(filepart, sep + 1, MAX_PATH - 1);
    }

    DIR* dir = opendir(dirpart);
    if (!dir) return INVALID_HANDLE_VALUE;

    _compat_find_data* fd = new _compat_find_data;
    fd->tag = _COMPAT_FIND_TAG;
    fd->dir = dir;
    strncpy(fd->pattern, filepart, MAX_PATH - 1);
    fd->pattern[MAX_PATH - 1] = 0;
    strncpy(fd->dirpath, dirpart, MAX_PATH - 1);
    fd->dirpath[MAX_PATH - 1] = 0;

    // Find first matching entry
    struct dirent* entry;
    while ((entry = readdir(dir)) != NULL) {
        if (entry->d_name[0] == '.' && (entry->d_name[1] == 0 ||
            (entry->d_name[1] == '.' && entry->d_name[2] == 0)))
            continue; // skip . and ..

        if (_compat_match_pattern(entry->d_name, fd->pattern)) {
            memset(data, 0, sizeof(*data));
            strncpy(data->cFileName, entry->d_name, MAX_PATH - 1);
            if (entry->d_type == DT_DIR)
                data->dwFileAttributes = FILE_ATTRIBUTE_DIRECTORY;
            else
                data->dwFileAttributes = FILE_ATTRIBUTE_NORMAL;
            return (HANDLE)fd;
        }
    }

    // No match found
    closedir(dir);
    delete fd;
    return INVALID_HANDLE_VALUE;
}
#define FindFirstFile FindFirstFileA

#undef FindNextFile
#undef FindNextFileA
inline BOOL FindNextFileA(HANDLE h, LPWIN32_FIND_DATAA data) {
    if (!h || h == INVALID_HANDLE_VALUE) return FALSE;
    _compat_find_data* fd = (_compat_find_data*)h;
    if (fd->tag != _COMPAT_FIND_TAG) return FALSE;

    struct dirent* entry;
    while ((entry = readdir(fd->dir)) != NULL) {
        if (entry->d_name[0] == '.' && (entry->d_name[1] == 0 ||
            (entry->d_name[1] == '.' && entry->d_name[2] == 0)))
            continue;

        if (_compat_match_pattern(entry->d_name, fd->pattern)) {
            memset(data, 0, sizeof(*data));
            strncpy(data->cFileName, entry->d_name, MAX_PATH - 1);
            if (entry->d_type == DT_DIR)
                data->dwFileAttributes = FILE_ATTRIBUTE_DIRECTORY;
            else
                data->dwFileAttributes = FILE_ATTRIBUTE_NORMAL;
            return TRUE;
        }
    }
    return FALSE;
}
#define FindNextFile FindNextFileA

#undef FindClose
inline BOOL FindClose(HANDLE h) {
    if (!h || h == INVALID_HANDLE_VALUE) return TRUE;
    _compat_find_data* fd = (_compat_find_data*)h;
    if (fd->tag != _COMPAT_FIND_TAG) return TRUE;
    if (fd->dir) closedir(fd->dir);
    fd->tag = 0;
    delete fd;
    return TRUE;
}

// ---- GetCurrentDirectory (real implementation) ----
#undef GetCurrentDirectory
#undef GetCurrentDirectoryA
inline DWORD GetCurrentDirectoryA(DWORD size, LPSTR buf) {
    if (!buf || size == 0) return 0;
    if (getcwd(buf, size) == NULL) return 0;
    return (DWORD)strlen(buf);
}
#define GetCurrentDirectory GetCurrentDirectoryA

// ---- SetCurrentDirectory (real implementation) ----
#undef SetCurrentDirectory
#undef SetCurrentDirectoryA
inline BOOL SetCurrentDirectoryA(LPCSTR path) {
    if (!path) return FALSE;
    return chdir(path) == 0;
}
#define SetCurrentDirectory SetCurrentDirectoryA

// ---- GetFileAttributes (real implementation) ----
#undef GetFileAttributes
#undef GetFileAttributesA
inline DWORD GetFileAttributesA(LPCSTR lpFile) {
    if (!lpFile) return INVALID_FILE_ATTRIBUTES;
    struct stat st;
    if (stat(lpFile, &st) < 0) return INVALID_FILE_ATTRIBUTES;
    if (S_ISDIR(st.st_mode)) return FILE_ATTRIBUTE_DIRECTORY;
    return FILE_ATTRIBUTE_NORMAL;
}
#define GetFileAttributes GetFileAttributesA

#endif // _COMPAT_FILE_IO_H_
