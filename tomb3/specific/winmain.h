#pragma once
#include "../global/types.h"

// Inizializzazione OpenGL e finestra/app
bool GLAppInit(GLDEVICEINFO* device, GLCONFIG* config, bool createNew);
void AppExit();
float AppFrameRate();
void AppSetStyle(bool fullscreen, ulong& set);
void S_ExitSystem(const char* msg);
void Log(const char* s, ...);

#ifdef DO_LOG
extern FILE* logF;
#endif

extern APPWINDOW App;
extern HWCONFIG HWConfig;
extern char* G_lpCmdLine;
extern long game_closedown;
extern bool GtWindowClosed;
extern long distanceFogValue;
extern long farz;