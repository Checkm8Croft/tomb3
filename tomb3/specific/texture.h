#pragma once
#include "../global/types.h"

// Sostituisci tutte le funzioni e variabili DirectX con OpenGL equivalenti

// Rimuovi tutto il blocco #if (DIRECT3D_VERSION < 0x900)
// e usa solo le versioni OpenGL

void DXTextureSetGreyScale(bool set);

void DXClearAllTextures(GLTEXTUREINFO* list);
void DXTextureCleanup(long index, GLTEXTUREINFO* list);
GLTEXTUREINFO* DXRestoreSurfaceIfLost(long index, GLTEXTUREINFO* list);
long DXTextureAdd(long w, long h, uchar* src, GLTEXTUREINFO* list, long bpp, ulong flags);

extern GLTEXTUREINFO Textures[MAX_TPAGES];
extern GLTEXTUREINFO* TexturePtrs[MAX_TPAGES];
extern long nTextures;
