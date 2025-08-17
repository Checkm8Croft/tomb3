#pragma once
#include "../global/types.h"

long MyReadFile(void* hFile, void* lpBuffer, ulong nNumberOfBytesToRead, ulong* lpNumberOfBytesRead, void* lpOverlapped);
bool LoadPalette(void* file);
long LoadTexturePages(void* file);
long LoadRooms(void* file);
long LoadObjects(void* file);
long LoadSprites(void* file);
long LoadCameras(void* file);
long LoadSoundEffects(void* file);
long LoadBoxes(void* file);
long LoadAnimatedTextures(void* file);
long LoadItems(void* file);
long LoadDepthQ(void* file);
long LoadCinematic(void* file);
long LoadDemo(void* file);
long LoadSamples(void* file);
void LoadDemFile(const char* name);
long LoadLevel(const char* name, long number);
void S_UnloadLevelFile();
long S_LoadLevelFile(char* name, long number, long type);
const char* GetFullPath(const char* name);
void build_ext(char* name, const char* ext);
void AdjustTextureUVs(bool reset);
long Read_Strings(long num, char** strings, char** buffer, ulong* read, void* file);
long S_LoadGameFlow(const char* name);

extern CHANGE_STRUCT* changes;
extern RANGE_STRUCT* ranges;
extern short* aranges;
extern short* frames;
extern short* commands;
extern short* floor_data;
extern short* mesh_base;
extern long number_cameras;
extern long nTInfos;

extern PHDTEXTURESTRUCT phdtextinfo[MAX_TINFOS];
extern PHDSPRITESTRUCT phdspriteinfo[512];
extern uchar G_GouraudPalette[1024];
