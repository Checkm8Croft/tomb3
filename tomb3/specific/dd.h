#pragma once
#include "../global/types.h"
#include <SDL2/SDL.h>

// SDL2 equivalents for DirectDraw functionality
struct SDL_Surface_Wrapper {
    SDL_Surface* surface;
    SDL_Texture* texture;  // For hardware acceleration
    void* pixels;
    int pitch;
    bool locked;
};

// Function declarations
bool DD_SpinMessageLoop(bool wait);
int DD_LockSurface(SDL_Surface_Wrapper* surf, int flags);
int DD_UnlockSurface(SDL_Surface_Wrapper* surf);
int DD_CreateSurface(int width, int height, SDL_PixelFormat* format, SDL_Surface_Wrapper*& surf);
int DD_EnsureSurfaceAvailable(SDL_Surface_Wrapper* surf, SDL_Surface_Wrapper* tSurf, bool clear);
bool DD_ClearSurface(SDL_Surface_Wrapper* surf, SDL_Rect* rect, ulong col);
void DD_DestroySurface(SDL_Surface_Wrapper* surf);

// Global SDL variables (you'll need to initialize these)
extern SDL_Window* g_Window;
extern SDL_Renderer* g_Renderer;