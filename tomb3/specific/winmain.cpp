#include "../tomb3/pch.h"
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

void Log(const char* s, ...)
{
#ifdef DO_LOG
    va_list list;
    char buf[4096];

    if (!logF)
        logF = fopen("tomb3_log.txt", "w+");

    va_start(list, s);
    vsnprintf(buf, sizeof(buf), s, list);
    strcat(buf, "\n");
    va_end(list);
    fwrite(buf, strlen(buf), 1, logF);
#endif
}

// Main loop di esempio
int main(int argc, char** argv)
{
    printf("Starting Tomb Raider III...\n");
    App.width = 800;
    App.height = 600;
    App.windowed = true;
    App.rScreen.left = 100;
    App.rScreen.top = 100;

    if (!GLAppInit(nullptr, nullptr, true)) {
        S_ExitSystem("Unable to initialize OpenGL/SDL2 window");
        return 1;
    }

    // Game loop
    bool running = true;
    while (running) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT)
                running = false;
        }
        // Rendering e logica qui
        SDL_GL_SwapWindow(sdlWindow);
        AppFrameRate();
    }

    AppExit();
    return 0;
}
