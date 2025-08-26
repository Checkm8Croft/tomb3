#include "../tomb3/pch.h"
#include "utils.h"
#include "winmain.h"
#include <SDL2/SDL.h>

static Uint64 start_us;
static double period;

double UT_GetAccurateTimer()
{
    Uint64 counter = SDL_GetPerformanceCounter();
    return ((double)counter - start_us) * period;
}

void UT_InitAccurateTimer()
{
    Uint64 fq = SDL_GetPerformanceFrequency();
    
    if (fq > 0)
    {
        period = 1.0 / (double)fq;
        start_us = SDL_GetPerformanceCounter();
    }
    else
    {
        period = 0;
        start_us = SDL_GetTicks64();
    }
}

void UT_CenterWindow(SDL_Window* window)
{
    SDL_DisplayMode dm;
    if (SDL_GetCurrentDisplayMode(0, &dm) == 0)
    {
        int windowWidth, windowHeight;
        SDL_GetWindowSize(window, &windowWidth, &windowHeight);
        
        int x = (dm.w - windowWidth) / 2;
        int y = (dm.h - windowHeight) / 2;
        
        SDL_SetWindowPosition(window, x, y);
    }
}

char* UT_FindArg(const char* arg)
{
    char* str;

    str = strstr(G_lpCmdLine, arg);

    if (str)
        str += strlen(arg);

    return str;
}

// Per SDL2, le dialog box vanno reimplementate completamente
bool UT_OKCancelBox(const char* title, const char* message, SDL_Window* parent)
{
    const SDL_MessageBoxButtonData buttons[] = {
        { SDL_MESSAGEBOX_BUTTON_RETURNKEY_DEFAULT, 1, "OK" },
        { SDL_MESSAGEBOX_BUTTON_ESCAPEKEY_DEFAULT, 0, "Cancel" }
    };
    
    const SDL_MessageBoxData messageboxdata = {
        SDL_MESSAGEBOX_INFORMATION,
        parent,
        title,
        message,
        SDL_arraysize(buttons),
        buttons,
        nullptr
    };
    
    int buttonid;
    if (SDL_ShowMessageBox(&messageboxdata, &buttonid) < 0) {
        return false;
    }
    
    return (buttonid == 1);
}

// La funzione dialog procedure non è più necessaria con SDL2
INT_PTR CALLBACK UT_OKCB_DlgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    return 0;
}