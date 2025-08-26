// single.h - SOLO DICHIARAZIONI
#pragma once
#include "../global/types.h"
#include "winmain.h"
#include "hwrender.h" 
#include "dxshell.h"        // Aggiungi questo - probabilmente dove è definito DEVICEINFO
extern DXCONFIG AppDXConfig;  // Aggiungi questa linea
extern bool bAlphaTesting;
// Dichiarazioni esterne
extern DXCONFIG G_dxConfig;
extern bool zBufWriteEnabled;
extern bool zBufCompareEnabled; 
extern bool AlphaBlendEnabled;
extern APPWINDOW App;
extern DEVICEINFO AppDeviceInfo;
// Dichiarazioni di funzioni (SENZA corpi)
void GLSaveScreen();
void HWR_EnableColorAddition(bool enable);
void HWR_EnableZBuffer(bool write, bool compare);
void HWR_EnableAlphaBlend(bool enable);
void HWR_EnableColorKey(bool enable);
void HWR_EndScene();