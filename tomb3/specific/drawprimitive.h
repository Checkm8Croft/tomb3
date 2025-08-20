#pragma once
#include "../global/types.h"

// Definizioni dei tipi di funzione per OpenGL
typedef int (*DRAWPRIMITIVE)(int PrimitiveType, void* Vertices, ulong VertexCount);
typedef int (*SETTEXTURE)(ulong Stage, TEXHANDLE pTexture);
typedef int (*BEGINSCENE)();
typedef int (*ENDSCENE)();
typedef int (*SETRENDERSTATE)(ulong dwRenderStateType, ulong dwRenderState);

// Dichiarazioni delle funzioni OpenGL
int HWDrawPrimitive(int PrimitiveType, void* Vertices, ulong VertexCount);
int HWSetTexture(ulong Stage, TEXHANDLE pTexture);
int HWBeginScene();
int HWEndScene();
int HWSetRenderState(ulong dwRenderStateType, ulong dwRenderState);
void InitDrawPrimitive();

// Variabili globali esterne
extern DRAWPRIMITIVE DrawPrimitive;
extern SETTEXTURE SetTexture;
extern BEGINSCENE BeginScene;
extern ENDSCENE EndScene;
extern SETRENDERSTATE SetRenderState;