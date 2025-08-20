#include "../tomb3/pch.h"
#include "dxshell.h"
#include "winmain.h"
#include "hwrender.h"
#include "drawprimitive.h"
#include "display.h"
#include "texture.h"
#include "../tomb3/tomb3.h"

// Include SDL con OpenGL
#include <SDL.h>
#include <SDL_opengl.h>

// Strutture dati per OpenGL
static SDL_Window* G_window;
static SDL_GLContext G_glContext;

// Variabile globale App
extern APPWINDOW App;

// These should be defined in the global scope based on original DX code
extern DEVICEINFO AppDeviceInfo;
extern DXCONFIG AppDXConfig;
extern bool AppWindowed;

static void* AddStruct(void* p, long num, long size)
{
    void* ptr;

    if (!num)
        ptr = malloc(size);
    else
        ptr = realloc(p, size * (num + 1));

    if (ptr)
        memset((char*)ptr + size * num, 0, size);
    return ptr;
}

// ... [rest of the utility functions remain the same] ...

bool DXStartRenderer(DEVICEINFO* device, DXCONFIG* config, bool createNew, bool windowed)
{
    Log("Starting DXStartRenderer with OpenGL");

    if (config->nD3D >= device->nD3DInfo || config->nVMode >= device->D3DInfo[config->nD3D].nDisplayMode)
    {
        Log("Invalid device or mode selection");
        return false;
    }

    DISPLAYMODE* dm = &device->D3DInfo[config->nD3D].DisplayMode[config->nVMode];

    if (createNew)
    {
        // Crea finestra SDL
        Uint32 flags = SDL_WINDOW_OPENGL;
        if (!windowed)
            flags |= SDL_WINDOW_FULLSCREEN;

        G_window = SDL_CreateWindow("Tomb Raider III",
                                    SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                    dm->w, dm->h, flags);
        
        if (!G_window)
        {
            Log("SDL_CreateWindow failed: %s", SDL_GetError());
            return false;
        }

        // Crea contesto OpenGL (Core Profile per macOS)
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
        SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
        SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
        SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);

        G_glContext = SDL_GL_CreateContext(G_window);
        if (!G_glContext)
        {
            Log("SDL_GL_CreateContext failed: %s", SDL_GetError());
            SDL_DestroyWindow(G_window);
            G_window = nullptr;
            return false;
        }

        // Imposta il viewport
        glViewport(0, 0, dm->w, dm->h);
        
        // Salva le dimensioni (usando la struttura RECT originale)
        App.rViewport.left = 0;
        App.rViewport.top = 0;
        App.rViewport.right = dm->w;
        App.rViewport.bottom = dm->h;
        
        App.rScreen = App.rViewport;
        
        // Set initial OpenGL state
        glEnable(GL_TEXTURE_2D);
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LEQUAL);
        glEnable(GL_CULL_FACE);
        glCullFace(GL_BACK);
        
        Log("OpenGL Vendor: %s", glGetString(GL_VENDOR));
        Log("OpenGL Renderer: %s", glGetString(GL_RENDERER));
        Log("OpenGL Version: %s", glGetString(GL_VERSION));
    }
    else
    {
        // Cambia modalità finestra
        if (windowed)
        {
            SDL_SetWindowFullscreen(G_window, 0);
            SDL_SetWindowSize(G_window, dm->w, dm->h);
            SDL_SetWindowPosition(G_window, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);
        }
        else
        {
            SDL_SetWindowFullscreen(G_window, SDL_WINDOW_FULLSCREEN);
        }
        
        glViewport(0, 0, dm->w, dm->h);
        
        // Aggiorna dimensioni
        App.rViewport.right = dm->w;
        App.rViewport.bottom = dm->h;
        App.rScreen = App.rViewport;
    }

    // Inizializza stato OpenGL
    HWR_InitState();
    
    // Crea texture buffer
    DXCreateMaxTPages(true);

    if (!nTextures)
    {
        Log("nTextures is 0, DXCreateMaxTPages failed");
        return false;
    }

    Log("DXStartRenderer finished successfully with OpenGL");
    return true;
}

bool DXSwitchVideoMode(long needed, long current, bool disableZBuffer)
{
    if (current == needed)
        return false;

    AppDXConfig.nVMode = needed;
    
    // Riavvia il renderer con la nuova risoluzione
    if (!WinDXInit(&AppDeviceInfo, &AppDXConfig, false))
    {
        WinFreeDX(false);
        AppDXConfig.nVMode = current;
        WinDXInit(&AppDeviceInfo, &AppDXConfig, false);
        return false;
    }

    HWR_InitState();
    setup_screen_size();
    return true;
}

bool DXUpdateFrame(bool runMessageLoop, LPRECT rect)
{
    // Swap buffers
    SDL_GL_SwapWindow(G_window);
    
    // Pulisce il buffer per il frame successivo
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    if (runMessageLoop)
    {
        // Gestisci eventi SDL
        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_QUIT)
                return false;
                
            if (event.type == SDL_KEYDOWN)
            {
                // Handle key presses
            }
            else if (event.type == SDL_MOUSEBUTTONDOWN || event.type == SDL_MOUSEBUTTONUP)
            {
                // Handle mouse events
            }
            else if (event.type == SDL_WINDOWEVENT)
            {
                if (event.window.event == SDL_WINDOWEVENT_RESIZED)
                {
                    // Handle window resize
                    glViewport(0, 0, event.window.data1, event.window.data2);
                }
            }
        }
    }
    
    return true;
}

long DXToggleFullScreen()
{
    if (!G_window)
        return -1;

    Uint32 isFullscreen = SDL_GetWindowFlags(G_window) & SDL_WINDOW_FULLSCREEN;
    
    if (SDL_SetWindowFullscreen(G_window, isFullscreen ? 0 : SDL_WINDOW_FULLSCREEN) != 0)
    {
        Log("DXToggleFullScreen failed: %s", SDL_GetError());
        return -1;
    }
    
    AppWindowed = !isFullscreen;
    
    // Update window dimensions
    int w, h;
    SDL_GetWindowSize(G_window, &w, &h);
    App.rViewport.right = w;
    App.rViewport.bottom = h;
    App.rScreen = App.rViewport;
    
    glViewport(0, 0, w, h);
    
    return 1;
}

void DXMove(long x, long y)
{
    if (AppWindowed && G_window)
    {
        SDL_SetWindowPosition(G_window, x, y);
        
        // Aggiorna le coordinate dello schermo
        DISPLAYMODE* dm = &AppDeviceInfo.D3DInfo[AppDXConfig.nD3D].DisplayMode[AppDXConfig.nVMode];
        
        App.rScreen.left = x;
        App.rScreen.top = y;
        App.rScreen.right = x + dm->w;
        App.rScreen.bottom = y + dm->h;
    }
}

// ... [rest of the functions remain the same] ...

void DXFreeDeviceInfo(DEVICEINFO* device)
{
    // Libera le strutture dati
    for (ulong i = 0; i < device->nD3DInfo; i++)
    {
        if (device->D3DInfo[i].DisplayMode)
            free(device->D3DInfo[i].DisplayMode);
    }
    
    if (device->D3DInfo)
        free(device->D3DInfo);
    
    if (device->DSInfo)
        free(device->DSInfo);

    memset(device, 0, sizeof(DEVICEINFO));
}

// Funzioni stub per compatibilità
bool DXEnumDirectSound(LPGUID lpGuid, LPCSTR lpcstrDescription, LPCSTR lpcstrModule, LPVOID lpContext)
{
    // Implementazione per SDL audio
    return true;
}

// Cleanup function
void DXCleanup()
{
    if (G_glContext)
    {
        SDL_GL_DeleteContext(G_glContext);
        G_glContext = nullptr;
    }
    
    if (G_window)
    {
        SDL_DestroyWindow(G_window);
        G_window = nullptr;
    }
    
    SDL_Quit();
}