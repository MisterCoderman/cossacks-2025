// ==============================================
// SDL event translation for Win32 PeekMessage/DispatchMessage
// Translates SDL events into Win32 MSG structs so the game's
// existing WindowProc can handle input unchanged.
// ==============================================
#ifndef _COMPAT_SDL_EVENTS_H_
#define _COMPAT_SDL_EVENTS_H_

// SDL.h must be found via include path (set by CMake)
#if __has_include(<SDL.h>)
#include <SDL.h>
#elif __has_include(<SDL2/SDL.h>)
#include <SDL2/SDL.h>
#else
#error "SDL.h not found - install SDL2 development headers"
#endif

// Track keyboard state for GetKeyState/GetKeyboardState
// Defined once in platform_main.cpp, declared extern here
extern BYTE _compat_key_state[256];

// Map SDL keycodes to Win32 VK codes
inline int _sdl_key_to_vk(SDL_Keycode key) {
    if (key >= SDLK_a && key <= SDLK_z) return 'A' + (key - SDLK_a);
    if (key >= SDLK_0 && key <= SDLK_9) return '0' + (key - SDLK_0);
    switch (key) {
        case SDLK_ESCAPE:    return VK_ESCAPE;
        case SDLK_RETURN:    return VK_RETURN;
        case SDLK_SPACE:     return VK_SPACE;
        case SDLK_TAB:       return VK_TAB;
        case SDLK_BACKSPACE: return VK_BACK;
        case SDLK_DELETE:    return VK_DELETE;
        case SDLK_PERIOD:    return 0xBE; // VK_OEM_PERIOD — avoid clash with VK_DELETE (both 0x2E)
        case SDLK_COMMA:     return 0xBC; // VK_OEM_COMMA
        case SDLK_MINUS:     return 0xBD; // VK_OEM_MINUS
        case SDLK_EQUALS:    return 0xBB; // VK_OEM_PLUS (= key)
        case SDLK_SEMICOLON: return 0xBA; // VK_OEM_1
        case SDLK_SLASH:     return 0xBF; // VK_OEM_2
        case SDLK_BACKQUOTE: return 0xC0; // VK_OEM_3
        case SDLK_LEFTBRACKET:  return 0xDB; // VK_OEM_4
        case SDLK_BACKSLASH:    return 0xDC; // VK_OEM_5
        case SDLK_RIGHTBRACKET: return 0xDE; // VK_OEM_6
        case SDLK_QUOTE:        return 0xDE; // VK_OEM_7
        case SDLK_LEFT:      return VK_LEFT;
        case SDLK_RIGHT:     return VK_RIGHT;
        case SDLK_UP:        return VK_UP;
        case SDLK_DOWN:      return VK_DOWN;
        case SDLK_LSHIFT:
        case SDLK_RSHIFT:    return VK_SHIFT;
        case SDLK_LCTRL:
        case SDLK_RCTRL:     return VK_CONTROL;
        case SDLK_LALT:
        case SDLK_RALT:      return VK_MENU;
        case SDLK_F1:        return VK_F1;
        case SDLK_F2:        return VK_F2;
        case SDLK_F3:        return VK_F3;
        case SDLK_F4:        return VK_F4;
        case SDLK_F5:        return VK_F5;
        case SDLK_F6:        return VK_F6;
        case SDLK_F7:        return VK_F7;
        case SDLK_F8:        return VK_F8;
        case SDLK_F9:        return VK_F9;
        case SDLK_F10:       return VK_F10;
        case SDLK_F11:       return VK_F11;
        case SDLK_F12:       return VK_F12;
        case SDLK_PAUSE:     return VK_PAUSE;
        case SDLK_HOME:      return VK_HOME;
        case SDLK_END:       return VK_END;
        case SDLK_PAGEUP:    return VK_PRIOR;
        case SDLK_PAGEDOWN:  return VK_NEXT;
        case SDLK_INSERT:    return VK_INSERT;
        case SDLK_SCROLLLOCK:return VK_SCROLL;
        case SDLK_NUMLOCKCLEAR: return VK_NUMLOCK;
        case SDLK_KP_0:      return VK_NUMPAD0;
        case SDLK_KP_1:      return VK_NUMPAD1;
        case SDLK_KP_2:      return VK_NUMPAD2;
        case SDLK_KP_3:      return VK_NUMPAD3;
        case SDLK_KP_4:      return VK_NUMPAD4;
        case SDLK_KP_5:      return VK_NUMPAD5;
        case SDLK_KP_6:      return VK_NUMPAD6;
        case SDLK_KP_7:      return VK_NUMPAD7;
        case SDLK_KP_8:      return VK_NUMPAD8;
        case SDLK_KP_9:      return VK_NUMPAD9;
        default:              return key & 0xFF;
    }
}

// Track modifier key state for wParam
extern WPARAM _sdl_mouse_wparam_storage;
#define _sdl_mouse_wparam _sdl_mouse_wparam_storage

inline void _sdl_update_modifiers(Uint16 mod) {
    // Preserve mouse button state, only update keyboard modifiers
    _sdl_mouse_wparam &= (MK_LBUTTON | MK_RBUTTON | MK_MBUTTON);
    if (mod & KMOD_SHIFT)  _sdl_mouse_wparam |= 0x0004; // MK_SHIFT
    if (mod & KMOD_CTRL)   _sdl_mouse_wparam |= 0x0008; // MK_CONTROL
}

// PeekMessage: poll one SDL event and translate to MSG
inline BOOL PeekMessageA(LPMSG msg, HWND hw, UINT min_val, UINT max_val, UINT remove) {
    (void)hw; (void)min_val; (void)max_val; (void)remove;
    if (!msg) return FALSE;

    SDL_Event sdlEvent;
    while (SDL_PollEvent(&sdlEvent)) {
        memset(msg, 0, sizeof(MSG));
        msg->hwnd = NULL;

        switch (sdlEvent.type) {
        case SDL_QUIT:
            msg->message = WM_QUIT;
            msg->wParam = 0;
            return TRUE;

        case SDL_MOUSEMOTION: {
            _sdl_update_modifiers(SDL_GetModState());
            msg->message = WM_MOUSEMOVE;
            msg->wParam = _sdl_mouse_wparam;
            // Clamp negative coords to 0 — MAKELONG casts to WORD (unsigned),
            // wrapping -1 to 65535 which triggers wrong-direction edge scrolling
            int mx = sdlEvent.motion.x < 0 ? 0 : sdlEvent.motion.x;
            int my = sdlEvent.motion.y < 0 ? 0 : sdlEvent.motion.y;
            msg->lParam = MAKELONG(mx, my);
            return TRUE;
        }

        case SDL_MOUSEBUTTONDOWN: {
            _sdl_update_modifiers(SDL_GetModState());
            if (sdlEvent.button.button == SDL_BUTTON_LEFT) {
                msg->message = WM_LBUTTONDOWN;
                _sdl_mouse_wparam |= MK_LBUTTON;
            } else if (sdlEvent.button.button == SDL_BUTTON_RIGHT) {
                msg->message = WM_RBUTTONDOWN;
                _sdl_mouse_wparam |= MK_RBUTTON;
            } else if (sdlEvent.button.button == SDL_BUTTON_MIDDLE) {
                msg->message = WM_MBUTTONDOWN;
                _sdl_mouse_wparam |= MK_MBUTTON;
            } else continue; // skip unknown buttons
            msg->wParam = _sdl_mouse_wparam;
            int bx = sdlEvent.button.x < 0 ? 0 : sdlEvent.button.x;
            int by = sdlEvent.button.y < 0 ? 0 : sdlEvent.button.y;
            msg->lParam = MAKELONG(bx, by);
            return TRUE;
        }

        case SDL_MOUSEBUTTONUP: {
            _sdl_update_modifiers(SDL_GetModState());
            if (sdlEvent.button.button == SDL_BUTTON_LEFT) {
                msg->message = WM_LBUTTONUP;
                _sdl_mouse_wparam &= ~MK_LBUTTON;
            } else if (sdlEvent.button.button == SDL_BUTTON_RIGHT) {
                msg->message = WM_RBUTTONUP;
                _sdl_mouse_wparam &= ~MK_RBUTTON;
            } else if (sdlEvent.button.button == SDL_BUTTON_MIDDLE) {
                msg->message = WM_MBUTTONUP;
                _sdl_mouse_wparam &= ~MK_MBUTTON;
            } else continue;
            msg->wParam = _sdl_mouse_wparam;
            int bx = sdlEvent.button.x < 0 ? 0 : sdlEvent.button.x;
            int by = sdlEvent.button.y < 0 ? 0 : sdlEvent.button.y;
            msg->lParam = MAKELONG(bx, by);
            return TRUE;
        }

        case SDL_MOUSEWHEEL:
            msg->message = WM_MOUSEWHEEL;
            msg->wParam = MAKELONG(0, (short)(sdlEvent.wheel.y * 120));
            msg->lParam = 0;
            return TRUE;

        case SDL_KEYDOWN:
        {
            int vk = _sdl_key_to_vk(sdlEvent.key.keysym.sym);
            if (vk >= 0 && vk < 256) _compat_key_state[vk] = 0x80;
            // Track modifiers
            if (sdlEvent.key.keysym.mod & KMOD_SHIFT)   _compat_key_state[VK_SHIFT] = 0x80;
            if (sdlEvent.key.keysym.mod & KMOD_CTRL)    _compat_key_state[VK_CONTROL] = 0x80;
            if (sdlEvent.key.keysym.mod & KMOD_ALT)     _compat_key_state[VK_MENU] = 0x80;
            msg->message = WM_KEYDOWN;
            msg->wParam = vk;
            msg->lParam = 0;
            return TRUE;
        }

        case SDL_KEYUP:
        {
            int vk = _sdl_key_to_vk(sdlEvent.key.keysym.sym);
            if (vk >= 0 && vk < 256) _compat_key_state[vk] = 0;
            if (!(sdlEvent.key.keysym.mod & KMOD_SHIFT)) _compat_key_state[VK_SHIFT] = 0;
            if (!(sdlEvent.key.keysym.mod & KMOD_CTRL))  _compat_key_state[VK_CONTROL] = 0;
            if (!(sdlEvent.key.keysym.mod & KMOD_ALT))   _compat_key_state[VK_MENU] = 0;
            msg->message = WM_KEYUP;
            msg->wParam = vk;
            msg->lParam = 0;
            return TRUE;
        }

        case SDL_WINDOWEVENT:
            if (sdlEvent.window.event == SDL_WINDOWEVENT_FOCUS_GAINED) {
                msg->message = WM_ACTIVATEAPP;
                msg->wParam = TRUE;
                return TRUE;
            } else if (sdlEvent.window.event == SDL_WINDOWEVENT_FOCUS_LOST) {
                msg->message = WM_ACTIVATEAPP;
                msg->wParam = FALSE;
                return TRUE;
            }
            continue; // skip other window events

        default:
            continue; // skip unhandled event types
        }
    }
    return FALSE; // no more events
}
#define PeekMessage PeekMessageA

inline BOOL TranslateMessage(const MSG* msg) { (void)msg; return TRUE; }

// DispatchMessage calls WindowProc directly
typedef LRESULT (*_compat_wndproc_t)(HWND, UINT, WPARAM, LPARAM);
extern _compat_wndproc_t _compat_wndproc_storage;
#define _compat_wndproc _compat_wndproc_storage

inline void _compat_set_wndproc(_compat_wndproc_t proc) {
    _compat_wndproc = proc;
}

inline LRESULT DispatchMessageA(const MSG* msg) {
    if (_compat_wndproc && msg) {
        return _compat_wndproc(msg->hwnd, msg->message, msg->wParam, msg->lParam);
    }
    return 0;
}
#define DispatchMessage DispatchMessageA

#endif // _COMPAT_SDL_EVENTS_H_
