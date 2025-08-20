#include "../tomb3/pch.h"
#include "dd.h"
#include "winmain.h"
#include <SDL2/SDL.h>

// Global SDL variables
SDL_Window* g_Window = nullptr;
SDL_Renderer* g_Renderer = nullptr;

bool DD_SpinMessageLoop(bool wait)
{
    SDL_Event event;
    static long nRecalls;
    static bool bQuit = false;

    nRecalls++;

    if (bQuit)
    {
        nRecalls--;
        return false;
    }

    if (wait)
        SDL_WaitEvent(nullptr);

    while (SDL_PollEvent(&event))
    {
        switch (event.type)
        {
        case SDL_QUIT:
            bQuit = true;
            GtWindowClosed = 1;
            nRecalls--;
            game_closedown = 1;
            return false;

        case SDL_WINDOWEVENT:
            switch (event.window.event)
            {
            case SDL_WINDOWEVENT_FOCUS_GAINED:
                // App.bFocus = true; // TODO: Find correct field name in APPWINDOW
                break;
            case SDL_WINDOWEVENT_FOCUS_LOST:
                // App.bFocus = false; // TODO: Find correct field name in APPWINDOW
                break;
            case SDL_WINDOWEVENT_MINIMIZED:
                // App.bFocus = false; // TODO: Find correct field name in APPWINDOW
                break;
            case SDL_WINDOWEVENT_RESTORED:
                // App.bFocus = true; // TODO: Find correct field name in APPWINDOW
                break;
            }
            break;

        case SDL_KEYDOWN:
        case SDL_KEYUP:
        case SDL_MOUSEBUTTONDOWN:
        case SDL_MOUSEBUTTONUP:
        case SDL_MOUSEMOTION:
            // Forward input events to game input handler
            // You'll need to implement HandleInputEvent() based on your input system
            // HandleInputEvent(event);
            break;
        }
    }

    // If we don't have focus, wait for events
    // TODO: Replace with correct focus field from APPWINDOW
    // if (!App.bFocus && wait)
    // {
    //     SDL_WaitEvent(nullptr);
    // }

    nRecalls--;
    return true;
}

int DD_LockSurface(SDL_Surface_Wrapper* surf, int flags)
{
    if (!surf || !surf->surface)
        return -1;

    if (surf->locked)
        return 0; // Already locked

    if (SDL_LockSurface(surf->surface) < 0)
        return -1;

    surf->pixels = surf->surface->pixels;
    surf->pitch = surf->surface->pitch;
    surf->locked = true;

    return 0; // Success
}

int DD_UnlockSurface(SDL_Surface_Wrapper* surf)
{
    if (!surf || !surf->surface || !surf->locked)
        return -1;

    SDL_UnlockSurface(surf->surface);
    surf->locked = false;
    surf->pixels = nullptr;

    return 0; // Success
}

int DD_CreateSurface(int width, int height, SDL_PixelFormat* format, SDL_Surface_Wrapper*& surf)
{
    surf = new SDL_Surface_Wrapper();
    if (!surf)
        return -1;

    // Create SDL surface
    if (format)
    {
        surf->surface = SDL_CreateRGBSurface(0, width, height, 
            format->BitsPerPixel, format->Rmask, format->Gmask, 
            format->Bmask, format->Amask);
    }
    else
    {
        // Default to 32-bit RGBA
        surf->surface = SDL_CreateRGBSurface(0, width, height, 32, 
            0x00FF0000, 0x0000FF00, 0x000000FF, 0xFF000000);
    }

    if (!surf->surface)
    {
        delete surf;
        surf = nullptr;
        return -1;
    }

    // Create texture for hardware acceleration (optional)
    if (g_Renderer)
    {
        surf->texture = SDL_CreateTextureFromSurface(g_Renderer, surf->surface);
        // Note: texture creation failure is not critical
    }

    surf->pixels = nullptr;
    surf->pitch = 0;
    surf->locked = false;

    return 0; // Success
}

int DD_EnsureSurfaceAvailable(SDL_Surface_Wrapper* surf, SDL_Surface_Wrapper* tSurf, bool clear)
{
    if (!surf || !surf->surface)
        return -1;

    // SDL surfaces don't get "lost" like DirectDraw surfaces
    // This function mainly exists for DirectDraw compatibility
    
    if (clear)
    {
        DD_ClearSurface(surf, nullptr, 0);
    }

    return 0; // Success
}

bool DD_ClearSurface(SDL_Surface_Wrapper* surf, SDL_Rect* rect, ulong col)
{
    if (!surf || !surf->surface)
        return false;

    // Convert color format if needed
    Uint32 sdlColor = SDL_MapRGBA(surf->surface->format, 
        (col >> 16) & 0xFF,  // Red
        (col >> 8) & 0xFF,   // Green
        col & 0xFF,          // Blue
        (col >> 24) & 0xFF   // Alpha
    );

    return SDL_FillRect(surf->surface, rect, sdlColor) == 0;
}

void DD_DestroySurface(SDL_Surface_Wrapper* surf)
{
    if (!surf)
        return;

    if (surf->texture)
    {
        SDL_DestroyTexture(surf->texture);
        surf->texture = nullptr;
    }

    if (surf->surface)
    {
        SDL_FreeSurface(surf->surface);
        surf->surface = nullptr;
    }

    delete surf;
}