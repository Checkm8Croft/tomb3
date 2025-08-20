#pragma once
#include "../global/types.h"
#include <SDL2/SDL_audio.h>

// Struttura per i campioni audio
struct AUDIO_SAMPLE
{
    Uint8* data;
    Uint32 length;
    Uint32 frequency;
};

// Struttura per i canali audio
struct AUDIO_CHANNEL
{
    AUDIO_SAMPLE* sample;
    int volume;
    int pitch;
    int pan;
    bool playing;
    Uint32 position;
};

// Dichiarazioni delle funzioni SDL2 Audio
bool DS_IsChannelPlaying(long num);
long DS_GetFreeChannel();
long DS_StartSample(long num, long volume, long pitch, long pan, ulong flags);
void DS_FreeAllSamples();
bool DS_MakeSample(long num, SDL_AudioSpec* fmt, Uint8* data, Uint32 bytes);
void DS_AdjustVolumeAndPan(long num, long volume, long pan);
void DS_AdjustPitch(long num, long pitch);
void DS_StopSample(long num);
bool DS_Create();
bool DS_IsSoundEnabled();
bool DS_SetOutputFormat();
void DS_Start();
void DS_Finish();

// Variabili globali
extern AUDIO_SAMPLE DS_Buffers[256];
extern AUDIO_CHANNEL DS_Samples[32];