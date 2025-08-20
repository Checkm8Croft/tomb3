#pragma once
#include "../global/types.h"
#include <SDL2/SDL.h>

// SDL2 equivalents for DirectInput functionality
void DI_ReadKeyboard(uchar* KeyMap);
void DI_StartKeyboard();
void DI_FinishKeyboard();
void DI_Start();
void DI_Finish();
bool DI_Create();

// Helper functions for SDL2 keyboard mapping
void DI_UpdateKeyMap();
bool DI_IsKeyPressed(SDL_Scancode key);