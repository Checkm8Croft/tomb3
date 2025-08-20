#include "../tomb3/pch.h"
#include "fmv.h"
#include "specific.h"
#include "winmain.h"
#include "file.h"
#include "input.h"
#include "audio.h"
#include "../tomb3/tomb3.h"
#include <SDL.h>
#include <SDL2/SDL_mixer.h>

long fmv_playing;

// Struttura per gestire il contesto del video
struct FMVContext {
    SDL_Window* window;
    SDL_Renderer* renderer;
    SDL_Texture* texture;
    int width;
    int height;
    bool isFullscreen;
};

static FMVContext* fmvContext = nullptr;

// Dichiarazioni forward delle funzioni
bool InitSDLForFMV();
void CleanupSDLForFMV();
void PlayFMV(const char* name);
void StopFMV();

bool InitSDLForFMV()
{
    // Inizializza SDL se non è già inizializzato
    if (SDL_WasInit(SDL_INIT_VIDEO) == 0) {
        if (SDL_InitSubSystem(SDL_INIT_VIDEO) < 0) {
            return false;
        }
    }
    
    if (SDL_WasInit(SDL_INIT_AUDIO) == 0) {
        if (SDL_InitSubSystem(SDL_INIT_AUDIO) < 0) {
            return false;
        }
    }
    
    // Inizializza SDL_mixer
    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
        return false;
    }
    
    return true;
}

void CleanupSDLForFMV()
{
    if (fmvContext) {
        if (fmvContext->texture) SDL_DestroyTexture(fmvContext->texture);
        if (fmvContext->renderer && !fmvContext->isFullscreen) {
            SDL_DestroyRenderer(fmvContext->renderer);
        }
        if (fmvContext->window && !fmvContext->isFullscreen) {
            SDL_DestroyWindow(fmvContext->window);
        }
        delete fmvContext;
        fmvContext = nullptr;
    }
    
    Mix_CloseAudio();
}

long FMV_Play(char* name)
{
    if (!InitSDLForFMV())
        return 0;

    fmv_playing = 1;
    S_CDStop();
    SDL_ShowCursor(SDL_DISABLE);
    
    // Riproduci il video
    PlayFMV(GetFullPath(name));
    
    // Ripristina il sistema
    StopFMV();
    fmv_playing = 0;

    SDL_ShowCursor(SDL_ENABLE);
    CleanupSDLForFMV();
    return 0;
}

long FMV_PlayIntro(char* name1, char* name2)
{
    if (!InitSDLForFMV())
        return 0;

    fmv_playing = 1;
    SDL_ShowCursor(SDL_DISABLE);
    
    PlayFMV(GetFullPath(name1));
    StopFMV();
    PlayFMV(GetFullPath(name2));
    StopFMV();
    
    fmv_playing = 0;

    SDL_ShowCursor(SDL_ENABLE);
    CleanupSDLForFMV();
    return 0;
}

void PlayFMV(const char* name)
{
    // Crea una nuova finestra dedicata per il FMV
    fmvContext = new FMVContext();
    
    fmvContext->window = SDL_CreateWindow("Tomb Raider III - FMV",
                                        SDL_WINDOWPOS_CENTERED,
                                        SDL_WINDOWPOS_CENTERED,
                                        640, 480,
                                        SDL_WINDOW_SHOWN);
    
    if (!fmvContext->window) {
        delete fmvContext;
        fmvContext = nullptr;
        return;
    }
    
    fmvContext->renderer = SDL_CreateRenderer(fmvContext->window, -1, 
                                            SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    
    if (!fmvContext->renderer) {
        SDL_DestroyWindow(fmvContext->window);
        delete fmvContext;
        fmvContext = nullptr;
        return;
    }
    
    fmvContext->isFullscreen = false;
    fmvContext->width = 640;
    fmvContext->height = 480;
    
    // Crea una texture di placeholder per il video
    SDL_Surface* placeholder = SDL_CreateRGBSurface(0, 640, 480, 32, 
                                                    0x00FF0000, 0x0000FF00, 0x000000FF, 0xFF000000);
    
    if (placeholder) {
        // Crea uno sfondo blu scuro
        SDL_FillRect(placeholder, NULL, SDL_MapRGB(placeholder->format, 0, 0, 64));
        
        // Disegna un rettangolo centrale per il messaggio
        SDL_Rect messageRect = {200, 200, 240, 80};
        SDL_FillRect(placeholder, &messageRect, SDL_MapRGB(placeholder->format, 0, 0, 128));
        
        // Disegna un bordo attorno al messaggio
        SDL_Rect borderRect = {198, 198, 244, 84};
        SDL_FillRect(placeholder, &borderRect, SDL_MapRGB(placeholder->format, 255, 255, 0));
        
        fmvContext->texture = SDL_CreateTextureFromSurface(fmvContext->renderer, placeholder);
        SDL_FreeSurface(placeholder);
    }
    
    // Loop di riproduzione
    SDL_Event event;
    bool quit = false;
    Uint32 startTime = SDL_GetTicks();
    Uint32 frameTime = 1000 / 25; // 25 FPS per i FMV
    
    while (!quit) {
        Uint32 currentTime = SDL_GetTicks();
        Uint32 elapsedTime = currentTime - startTime;
        
        // Esci dopo 10 secondi o se viene premuto un tasto
        if (elapsedTime > 10000) {
            break;
        }
        
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT || 
                event.type == SDL_KEYDOWN ||
                event.type == SDL_MOUSEBUTTONDOWN) {
                quit = true;
            }
        }
        
        // Aggiorna input del gioco - usa solo IN_OPTION che dovrebbe esistere
        if (S_UpdateInput() || (input & IN_OPTION)) {
            quit = true;
        }
        
        // Renderizza il frame
        SDL_SetRenderDrawColor(fmvContext->renderer, 0, 0, 0, 255);
        SDL_RenderClear(fmvContext->renderer);
        
        if (fmvContext->texture) {
            SDL_RenderCopy(fmvContext->renderer, fmvContext->texture, NULL, NULL);
        }
        
        // Disegna il messaggio di placeholder usando primitive SDL
        SDL_Rect messageBg = {200, 200, 240, 80};
        SDL_SetRenderDrawColor(fmvContext->renderer, 0, 0, 128, 255);
        SDL_RenderFillRect(fmvContext->renderer, &messageBg);
        
        SDL_Rect messageBorder = {198, 198, 244, 84};
        SDL_SetRenderDrawColor(fmvContext->renderer, 255, 255, 0, 255);
        SDL_RenderDrawRect(fmvContext->renderer, &messageBorder);
        
        // Aggiungi un indicatore di progressione
        SDL_Rect progressBar = {100, 450, (int)(440 * (elapsedTime / 10000.0f)), 20};
        SDL_SetRenderDrawColor(fmvContext->renderer, 255, 255, 0, 255);
        SDL_RenderFillRect(fmvContext->renderer, &progressBar);
        
        SDL_RenderPresent(fmvContext->renderer);
        
        SDL_Delay(frameTime);
    }
}

void StopFMV()
{
    CleanupSDLForFMV();
}

// Funzioni stub per mantenere la compatibilità
void WinPlayFMV(const char* name, bool play)
{
    PlayFMV(name);
}

void WinStopFMV(bool play)
{
    StopFMV();
}