#pragma warning (disable : 4035)

#include <windows.h>

void isiDecryptMem(LPBYTE lpbBuffer, DWORD dwSize, BYTE dbKey) {
    // Проверка входных параметров
    if (!lpbBuffer || dwSize == 0) return; // Проверка на nullptr и нулевой размер

    // Приведение указателя к массиву байт
    LPBYTE buffer = lpbBuffer;

    // Цикл дешифрования
    for (DWORD i = 0; i < dwSize; i++) {
        BYTE al = buffer[i];      // Читаем текущий байт
        al = ~al;                 // Инверсия битов (not)
        al ^= dbKey;              // XOR с ключом
        buffer[i] = al;           // Записываем результат
    }
}

void isiEncryptMem(LPBYTE lpbBuffer, DWORD dwSize, BYTE dbKey) {
    // Проверка входных параметров
    if (!lpbBuffer || dwSize == 0) return; // Проверка на nullptr и нулевой размер

    // Цикл шифрования
    BYTE invertedKey = ~dbKey; // Инвертированный ключ
    for (DWORD i = 0; i < dwSize; i++) {
        BYTE al = lpbBuffer[i];    // Читаем текущий байт
        al ^= invertedKey;         // XOR с инвертированным ключом
        al = ~al;                  // Инверсия результата
        lpbBuffer[i] = al;         // Записываем результат
    }
}

DWORD isiCalcHash(LPSTR lpszFileName) {
    char szFileName[64];

    // Инициализация буфера
    ZeroMemory(szFileName, 64);
    if (lpszFileName) {
        // Копируем строку и приводим к верхнему регистру
        strncpy_s(szFileName, 64, lpszFileName, _TRUNCATE);
        for (int i = 0; i < 64 && szFileName[i]; i++) {
            szFileName[i] = toupper(szFileName[i]);
        }
    }

    DWORD hash = 0;
    for (int i = 0; i < 16; i++) {
        DWORD value = *(DWORD*)&szFileName[i * 4];
        // Перестановка байтов: xchg ah,al
        DWORD swapped = (value & 0xFFFF0000) | ((value & 0xFF) << 8) | ((value & 0xFF00) >> 8);
        // rol eax,16
        swapped = (swapped << 16) | (swapped >> 16);
        // xchg ah,al
        swapped = (swapped & 0xFFFF0000) | ((swapped & 0xFF) << 8) | ((swapped & 0xFF00) >> 8);
        hash += swapped;
    }

    return hash;
}


BOOL isiMatchesMask(LPSTR lpszFile, LPSTR lpszMask) {
    char szFile[255];
    char szMask[255];

    // Копирование и приведение к верхнему регистру
    if (lpszFile) {
        strncpy_s(szFile, 255, lpszFile, _TRUNCATE);
        for (int i = 0; i < 255 && szFile[i]; i++) {
            szFile[i] = toupper(szFile[i]);
        }
    }
    else {
        szFile[0] = 0;
    }

    if (lpszMask) {
        strncpy_s(szMask, 255, lpszMask, _TRUNCATE);
        for (int i = 0; i < 255 && szMask[i]; i++) {
            szMask[i] = toupper(szMask[i]);
        }
    }
    else {
        szMask[0] = 0;
    }

    const char* file = szFile;
    const char* mask = szMask;

    while (true) {
        char maskChar = *mask;
        char fileChar = *file;

        // Если обе строки закончились одновременно
        if (maskChar == 0 && fileChar == 0) {
            return TRUE;
        }

        // Если одна строка закончилась, а другая нет
        if (maskChar == 0 || fileChar == 0) {
            // Исключение: если маска заканчивается на '*', это допустимо
            if (maskChar == '*') {
                mask++;
                continue;
            }
            return FALSE;
        }

        if (maskChar == '?') {
            mask++;
            file++;
            continue;
        }

        if (maskChar == '*') {
            mask++;
            if (*mask == 0) return TRUE; // '*' в конце маски — совпадение

            while (*file) {
                if (*file == *mask) {
                    file++;
                    mask++;
                    break;
                }
                file++;
            }
            continue;
        }

        if (maskChar != fileChar) {
            return FALSE;
        }

        mask++;
        file++;
    }

    return TRUE;
}

BOOL isiFileExists(LPSTR lpszFileName) {
    if (!lpszFileName || !lpszFileName[0]) return FALSE; // Проверка на nullptr и пустую строку

    WIN32_FIND_DATA FindData;
    HANDLE hFindFile = FindFirstFileA(lpszFileName, &FindData);
    if (hFindFile != INVALID_HANDLE_VALUE) {
        FindClose(hFindFile);
        return TRUE;
    }
    return FALSE;
}

#pragma warning (default : 4035)