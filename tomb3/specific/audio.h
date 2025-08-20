#pragma once
#include "../global/types.h"

// Definizioni cross-platform
#ifdef _WIN32
#include <windows.h>
#include <mmsystem.h>
#else
typedef void* HACMDRIVERID;
typedef unsigned long DWORD;
typedef unsigned long* DWORD_PTR;
typedef int BOOL;
#endif

BOOL ACMEnumCallBack(HACMDRIVERID hadid, DWORD_PTR dwInstance, DWORD fdwSupport);
void ACMCloseFile();
bool ACMOpenFile(const char* name);
void ACMEmulateCDStop();
void ACMEmulateCDPlay(long track, long mode);
void ThreadACMEmulateCDPlay(long track, long mode);
long ACMGetTrackLocation();
void ACMMute();
void ACMSetVolume(long volume);
long ACMHandleNotifications();
long ACMSetupNotifications();
bool ACMInit();
void ACMClose();

extern long acm_volume;