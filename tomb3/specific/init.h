#pragma once
#include "../global/types.h"

void ShutdownGame();
void CalculateWibbleTable();
ushort GetRandom(WATERTAB* wt, long lp);
void init_water_table();
void init_game_malloc();
void* game_malloc(long size);
void game_free(long size);
long S_InitialiseSystem();

extern char* malloc_ptr;
extern char* malloc_buffer;

typedef unsigned long ulong;



extern GLVERTEX* CurrentGLVertex;
extern GLVERTEX* VertexBuffer;
extern GLVERTEX* UnRollBuffer;

extern WATERTAB WaterTable[22][64];
extern float wibble_table[32];