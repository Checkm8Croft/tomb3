#pragma once
#include "../global/types.h"
#include "init.h"
// Funzioni di rendering hardware OpenGL
void HWR_EnableZBuffer(bool write, bool compare);
void HWR_EnableAlphaBlend(bool enable);
void HWR_EnableColorAddition(bool enable);
void HWR_EnableColorSubtraction(bool enable);
void HWR_ResetZBuffer();
void HWR_ResetCurrentTexture();
void HWR_BeginScene();
void HWR_EndScene();
void HWR_SetCurrentTexture(GLTEXTUREINFO* tex);
void HWR_DrawRoutines(long nVtx, GLVERTEX* vtx, long nDrawType, long TPage);

void HWR_InitGamma(float gamma);
void HWR_InitState();
bool HWR_Init();
void HWR_DrawPolyList(long num, long* pSort);
void HWR_DrawPolyListBF(long num, long* pSort);
void HWR_FreeTexturePages();
void HWR_GetAllTextureHandles();
void HWR_LoadTexturePages(long nPages, uchar* src, uchar* palette);
void HWR_SetCurrentTexture(GLTEXTUREINFO* tex);

extern float GammaOption;
extern uchar ColorTable[256];
extern bool bAlphaTesting;