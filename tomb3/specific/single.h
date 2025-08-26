// single.h - SOLO DICHIARAZIONI
#pragma once
#include "../global/types.h"
#include "winmain.h"
#include "hwrender.h" 
#include "dxshell.h"        // Aggiungi questo - probabilmente dove è definito DEVICEINFO
#include "init.h"
extern DXCONFIG AppDXConfig;  // Aggiungi questa linea
extern bool bAlphaTesting;
// Dichiarazioni esterne
extern DXCONFIG G_dxConfig;
extern bool zBufWriteEnabled;
extern bool zBufCompareEnabled; 
extern bool AlphaBlendEnabled;
extern bool AppWindowed;
extern APPWINDOW App;
extern DEVICEINFO AppDeviceInfo;
extern bool AppWindowed;
extern bool bMonoScreen;
extern GLVERTEX* CurrentGLVertex;
extern DXCONFIG G_dxConfig;
extern bool bMonoScreen;
// Dichiarazioni di funzioni (SENZA corpi)
void GLSaveScreen();
void HWR_EnableColorAddition(bool enable);
void HWR_EnableZBuffer(bool write, bool compare);
void HWR_EnableAlphaBlend(bool enable);
void HWR_EnableColorKey(bool enable);
void HWR_EndScene();