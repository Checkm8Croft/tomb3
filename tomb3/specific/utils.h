#pragma once
#include "../global/types.h"
#include <SDL2/SDL.h>

    typedef SDL_Window* HWND;
    typedef Uint32 UINT;
    typedef Uint32 WPARAM;
    typedef Sint32 LPARAM;
    typedef Sint32 INT_PTR;
    #define CALLBACK
double UT_GetAccurateTimer();
void UT_InitAccurateTimer();
void UT_CenterWindow(SDL_Window* window);
char* UT_FindArg(const char* arg);
INT_PTR CALLBACK UT_OKCB_DlgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
bool UT_OKCancelBox(char* lpTemplateName, SDL_Window* parentWindow);
