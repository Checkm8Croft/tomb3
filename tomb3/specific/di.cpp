#include "../tomb3/pch.h"
#include "di.h"
#include "winmain.h"
#include <SDL2/SDL.h>

// SDL2 keyboard state
static const Uint8* g_KeyboardState = nullptr;
static bool g_KeyboardInitialized = false;

// DirectInput to SDL2 scancode mapping
// This maps Windows virtual key codes to SDL scancodes
static SDL_Scancode VK_to_SDL[256];

void InitKeyMapping()
{
    static bool initialized = false;
    if (initialized) return;
    
    // Initialize the mapping table
    for (int i = 0; i < 256; i++) {
        VK_to_SDL[i] = SDL_SCANCODE_UNKNOWN;
    }
    
    // Common key mappings (Windows VK codes to SDL scancodes)
    VK_to_SDL[0x01] = SDL_SCANCODE_UNKNOWN; // VK_LBUTTON (mouse)
    VK_to_SDL[0x02] = SDL_SCANCODE_UNKNOWN; // VK_RBUTTON (mouse)
    VK_to_SDL[0x08] = SDL_SCANCODE_BACKSPACE;
    VK_to_SDL[0x09] = SDL_SCANCODE_TAB;
    VK_to_SDL[0x0D] = SDL_SCANCODE_RETURN;
    VK_to_SDL[0x10] = SDL_SCANCODE_LSHIFT;
    VK_to_SDL[0x11] = SDL_SCANCODE_LCTRL;
    VK_to_SDL[0x12] = SDL_SCANCODE_LALT;
    VK_to_SDL[0x13] = SDL_SCANCODE_PAUSE;
    VK_to_SDL[0x14] = SDL_SCANCODE_CAPSLOCK;
    VK_to_SDL[0x1B] = SDL_SCANCODE_ESCAPE;
    VK_to_SDL[0x20] = SDL_SCANCODE_SPACE;
    VK_to_SDL[0x21] = SDL_SCANCODE_PAGEUP;
    VK_to_SDL[0x22] = SDL_SCANCODE_PAGEDOWN;
    VK_to_SDL[0x23] = SDL_SCANCODE_END;
    VK_to_SDL[0x24] = SDL_SCANCODE_HOME;
    VK_to_SDL[0x25] = SDL_SCANCODE_LEFT;
    VK_to_SDL[0x26] = SDL_SCANCODE_UP;
    VK_to_SDL[0x27] = SDL_SCANCODE_RIGHT;
    VK_to_SDL[0x28] = SDL_SCANCODE_DOWN;
    VK_to_SDL[0x2C] = SDL_SCANCODE_PRINTSCREEN;
    VK_to_SDL[0x2D] = SDL_SCANCODE_INSERT;
    VK_to_SDL[0x2E] = SDL_SCANCODE_DELETE;
    
    // Number keys
    for (int i = 0; i < 10; i++) {
        VK_to_SDL[0x30 + i] = (SDL_Scancode)(SDL_SCANCODE_0 + i);
    }
    
    // Letter keys
    for (int i = 0; i < 26; i++) {
        VK_to_SDL[0x41 + i] = (SDL_Scancode)(SDL_SCANCODE_A + i);
    }
    
    // Function keys
    for (int i = 0; i < 12; i++) {
        VK_to_SDL[0x70 + i] = (SDL_Scancode)(SDL_SCANCODE_F1 + i);
    }
    
    // Numpad
    for (int i = 0; i < 10; i++) {
        VK_to_SDL[0x60 + i] = (SDL_Scancode)(SDL_SCANCODE_KP_0 + i);
    }
    
    // Additional keys
    VK_to_SDL[0x6A] = SDL_SCANCODE_KP_MULTIPLY;
    VK_to_SDL[0x6B] = SDL_SCANCODE_KP_PLUS;
    VK_to_SDL[0x6D] = SDL_SCANCODE_KP_MINUS;
    VK_to_SDL[0x6E] = SDL_SCANCODE_KP_PERIOD;
    VK_to_SDL[0x6F] = SDL_SCANCODE_KP_DIVIDE;
    
    // Modifier keys
    VK_to_SDL[0xA0] = SDL_SCANCODE_LSHIFT;
    VK_to_SDL[0xA1] = SDL_SCANCODE_RSHIFT;
    VK_to_SDL[0xA2] = SDL_SCANCODE_LCTRL;
    VK_to_SDL[0xA3] = SDL_SCANCODE_RCTRL;
    VK_to_SDL[0xA4] = SDL_SCANCODE_LALT;
    VK_to_SDL[0xA5] = SDL_SCANCODE_RALT;
    
    initialized = true;
}

void DI_ReadKeyboard(uchar* KeyMap)
{
    if (!g_KeyboardInitialized || !KeyMap) {
        if (KeyMap) memset(KeyMap, 0, 256);
        return;
    }
    
    // Update SDL keyboard state
    g_KeyboardState = SDL_GetKeyboardState(nullptr);
    
    // Clear the keymap
    memset(KeyMap, 0, 256);
    
    // Convert SDL keyboard state to DirectInput-style keymap
    for (int vk = 0; vk < 256; vk++) {
        SDL_Scancode scancode = VK_to_SDL[vk];
        if (scancode != SDL_SCANCODE_UNKNOWN && g_KeyboardState[scancode]) {
            KeyMap[vk] = 0x80; // DirectInput uses 0x80 for pressed keys
        }
    }
}

void DI_StartKeyboard()
{
    if (g_KeyboardInitialized) {
        return;
    }
    
    InitKeyMapping();
    
    // Initialize SDL keyboard subsystem if not already done
    if (!(SDL_WasInit(SDL_INIT_EVENTS))) {
        if (SDL_InitSubSystem(SDL_INIT_EVENTS) < 0) {
            S_ExitSystem("SDL Events initialization failed");
            return;
        }
    }
    
    g_KeyboardState = SDL_GetKeyboardState(nullptr);
    g_KeyboardInitialized = true;
}

void DI_FinishKeyboard()
{
    g_KeyboardInitialized = false;
    g_KeyboardState = nullptr;
    // Note: We don't quit SDL here as it might be used elsewhere
}

void DI_Start()
{
    if (!DI_Create()) {
        S_ExitSystem("DI_Create failed");
        return;
    }
    
    DI_StartKeyboard();
}

void DI_Finish()
{
    DI_FinishKeyboard();
    // SDL cleanup is handled elsewhere
}

bool DI_Create()
{
    // SDL2 doesn't need explicit "DirectInput creation"
    // Just ensure SDL is initialized
    if (!SDL_WasInit(SDL_INIT_EVENTS)) {
        return SDL_InitSubSystem(SDL_INIT_EVENTS) == 0;
    }
    return true;
}

// Helper functions
void DI_UpdateKeyMap()
{
    // This function can be called to update the keyboard state
    // if needed outside of DI_ReadKeyboard
    g_KeyboardState = SDL_GetKeyboardState(nullptr);
}

bool DI_IsKeyPressed(SDL_Scancode key)
{
    if (!g_KeyboardInitialized || !g_KeyboardState) {
        return false;
    }
    return g_KeyboardState[key] != 0;
}