// Stub for <objbase.h> on non-Windows platforms
#ifndef _COMPAT_OBJBASE_H_
#define _COMPAT_OBJBASE_H_

#include "windows.h"

// CoInitialize / CoUninitialize stubs
inline HRESULT CoInitialize(LPVOID reserved) { (void)reserved; return S_OK; }
inline void CoUninitialize() {}
inline HRESULT CoCreateInstance(REFCLSID clsid, IUnknown* outer, DWORD ctx, REFIID riid, LPVOID* ppv) {
    (void)clsid; (void)outer; (void)ctx; (void)riid; (void)ppv;
    return E_NOTIMPL;
}

#define CLSCTX_INPROC_SERVER 0x1

#endif
