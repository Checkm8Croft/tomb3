#include "../tomb3/pch.h"
#include "../tomb3/tomb3.h"
#include "utils.h"
#include "smain.h"
#include "option.h"
#include "ds.h"
#include "di.h"
#include "audio.h"
#include "display.h"
#include "winmain.h"
#include "hwrender.h"
#include "../game/invfunc.h"
#include "../tomb3/tomb3.h"
#include <SDL2/SDL.h>
#include "init.h"

#ifdef DO_LOG
FILE* logF = 0;
#endif

APPWINDOW App;
HWCONFIG HWConfig;
char* G_lpCmdLine;
long game_closedown;
bool GtWindowClosed;
long distanceFogValue;
long farz;

// SDL window e context
static SDL_Window* sdlWindow = nullptr;
static SDL_GLContext glContext = nullptr;

bool GLAppInit(GLDEVICEINFO* device, GLCONFIG* config, bool createNew)
{
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0)
        return false;

    sdlWindow = SDL_CreateWindow(
        "Tomb Raider III",
        App.rScreen.left, App.rScreen.top,
        App.width, App.height,
        App.windowed ? SDL_WINDOW_OPENGL : (SDL_WINDOW_OPENGL | SDL_WINDOW_FULLSCREEN)
    );
    if (!sdlWindow)
        return false;

    glContext = SDL_GL_CreateContext(sdlWindow);
    if (!glContext)
        return false;

    SDL_GL_SetSwapInterval(1); // VSync

    // Inizializza OpenGL state
    HWR_InitState();

    return true;
}

void AppExit()
{
    ShutdownGame();
    if (glContext) SDL_GL_DeleteContext(glContext);
    if (sdlWindow) SDL_DestroyWindow(sdlWindow);
    SDL_Quit();
    exit(0);
}

float AppFrameRate()
{
    static Uint32 lastTime = 0, frames = 0;
    static float fps = 0.0f;
    Uint32 currentTime = SDL_GetTicks();
    frames++;
    if (currentTime - lastTime > 1000) {
        fps = frames * 1000.0f / (currentTime - lastTime);
        lastTime = currentTime;
        frames = 0;
    }
    App.fps = fps;
    return fps;
}

void AppSetStyle(bool fullscreen, ulong& set)
{
    if (fullscreen)
        SDL_SetWindowFullscreen(sdlWindow, SDL_WINDOW_FULLSCREEN);
    else
        SDL_SetWindowFullscreen(sdlWindow, 0);
    // 'set' può essere usato per flag custom, se serve
}

void S_ExitSystem(const char* msg)
{
    SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Error", msg, sdlWindow);
    ShutdownGame();
    exit(1);
}


// Main loop di esempio
int main(int argc, char** argv)
{
     if (argc > 0) {
        G_lpCmdLine = argv[0];  // o costruisci la command line dagli argv
    } else {
        G_lpCmdLine = "";
    }
    printf("[1] Starting Tomb Raider III...\n");
    
    // Inizializzazione SDL2
    printf("[2] Initializing SDL2...\n");
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_EVENTS) < 0) {
        printf("SDL_Init failed: %s\n", SDL_GetError());
        return 1;
    }
    printf("[3] SDL2 initialized successfully\n");

    // Crea la finestra SDL2
    printf("[4] Creating SDL2 window...\n");
    SDL_Window* sdlWindow = SDL_CreateWindow("Tomb Raider III",
                                            SDL_WINDOWPOS_CENTERED,
                                            SDL_WINDOWPOS_CENTERED,
                                            800, 600,
                                            SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
    
    if (!sdlWindow) {
        printf("SDL_CreateWindow failed: %s\n", SDL_GetError());
        return 1;
    }
    printf("[5] SDL2 window created successfully\n");

    // Crea contesto OpenGL
    printf("[6] Creating OpenGL context...\n");
    SDL_GLContext glContext = SDL_GL_CreateContext(sdlWindow);
    if (!glContext) {
        printf("SDL_GL_CreateContext failed: %s\n", SDL_GetError());
        return 1;
    }
    printf("[7] OpenGL context created successfully\n");

    // Inizializza il gioco
   printf("[8] Loading game settings...\n");
bool settingsLoaded = S_LoadSettings();

// Controlla se G_lpCmdLine è NULL prima di chiamare UT_FindArg
bool setupArg = false;
if (G_lpCmdLine != nullptr) {
    setupArg = UT_FindArg("setup");
} else {
    printf("G_lpCmdLine is NULL, skipping setup check\n");
}

printf("[8.5] S_LoadSettings: %d, setupArg: %d\n", settingsLoaded, setupArg);

if (setupArg) {
    printf("Setup required - exiting\n");
    return 0;
}
printf("[9] Game settings loaded successfully\n");
    // Inizializza sistemi del gioco
    printf("[10] Initializing TIME system...\n");
    TIME_Init();
    printf("[11] Initializing HWR system...\n");
    HWR_Init();
    printf("[12] Starting DS system...\n");
    DS_Start();
    printf("[13] Starting DI system...\n");
    DI_Start();
    printf("[14] Initializing ACM system...\n");
    ACMInit();
    printf("[15] Setting up screen size...\n");
    setup_screen_size();
    
    game_closedown = 0;
    GtWindowClosed = 0;
    GtFullScreenClearNeeded = 0;

    printf("[16] Starting main game loop...\n");
    // Avvia il game loop principale
    GameMain();

    printf("[17] Game ended, cleaning up...\n");
    // Cleanup
    SDL_GL_DeleteContext(glContext);
    SDL_DestroyWindow(sdlWindow);
    SDL_Quit();
    
    printf("[18] Cleanup completed, exiting...\n");
    return 0;
}