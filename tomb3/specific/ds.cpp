#include "../tomb3/pch.h"
#include "ds.h"
#include "winmain.h"
#include "../game/camera.h"
#include <SDL2/SDL.h>

AUDIO_SAMPLE DS_Buffers[256];
AUDIO_CHANNEL DS_Samples[32];
SDL_AudioDeviceID audio_device;

bool DS_IsChannelPlaying(long num)
{
    if (num < 0 || num >= 32)
        return false;
    
    return DS_Samples[num].playing;
}

long DS_GetFreeChannel()
{
    for (int i = 0; i < 32; i++)
    {
        if (!DS_Samples[i].playing)
            return i;
    }

    for (int i = 0; i < 32; i++)
    {
        if (!DS_IsChannelPlaying(i))
            return i;
    }

    return -1;
}

long DS_StartSample(long num, long volume, long pitch, long pan, ulong flags)
{
    long channel = DS_GetFreeChannel();
    
    if (channel < 0 || num < 0 || num >= 256 || !DS_Buffers[num].data)
        return -1;

    // In SDL2, la riproduzione audio è gestita dal mixer
    // Qui dovresti implementare la logica per mixare l'audio
    DS_Samples[channel].sample = &DS_Buffers[num];
    DS_Samples[channel].volume = volume;
    DS_Samples[channel].pitch = pitch;
    DS_Samples[channel].pan = pan;
    DS_Samples[channel].playing = true;
    DS_Samples[channel].position = 0;

    return channel;
}

void DS_FreeAllSamples()
{
    for (int i = 0; i < 256; i++)
    {
        if (DS_Buffers[i].data)
        {
            free(DS_Buffers[i].data);
            DS_Buffers[i].data = nullptr;
            DS_Buffers[i].length = 0;
        }
    }
}

bool DS_MakeSample(long num, SDL_AudioSpec* fmt, Uint8* data, Uint32 bytes)
{
    if (num < 0 || num >= 256)
        return false;

    DS_Buffers[num].data = (Uint8*)malloc(bytes);
    if (!DS_Buffers[num].data)
        return false;

    memcpy(DS_Buffers[num].data, data, bytes);
    DS_Buffers[num].length = bytes;
    DS_Buffers[num].frequency = fmt->freq;

    return true;
}

void DS_AdjustVolumeAndPan(long num, long volume, long pan)
{
    if (num >= 0 && num < 32 && DS_Samples[num].playing)
    {
        DS_Samples[num].volume = volume;
        DS_Samples[num].pan = pan;
    }
}

void DS_AdjustPitch(long num, long pitch)
{
    if (num >= 0 && num < 32 && DS_Samples[num].playing)
    {
        DS_Samples[num].pitch = pitch;
    }
}

void DS_StopSample(long num)
{
    if (num >= 0 && num < 32)
    {
        DS_Samples[num].playing = false;
        DS_Samples[num].sample = nullptr;
    }
}

bool DS_Create()
{
    SDL_AudioSpec desired, obtained;
    
    SDL_memset(&desired, 0, sizeof(desired));
    desired.freq = 22050;
    desired.format = AUDIO_S16;
    desired.channels = 2;
    desired.samples = 4096;
    
    audio_device = SDL_OpenAudioDevice(NULL, 0, &desired, &obtained, 0);
    return audio_device > 0;
}

bool DS_IsSoundEnabled()
{
    return audio_device > 0;
}

bool DS_SetOutputFormat()
{
    // In SDL2, il formato è impostato in DS_Create
    return true;
}

void DS_Start()
{
    memset(DS_Buffers, 0, sizeof(DS_Buffers));
    memset(DS_Samples, 0, sizeof(DS_Samples));
    camera.mike_at_lara = 0;

    if (DS_Create())
    {
        SDL_PauseAudioDevice(audio_device, 0); // Start audio playback
    }
}

void DS_Finish()
{
    DS_FreeAllSamples();
    
    if (audio_device > 0)
    {
        SDL_CloseAudioDevice(audio_device);
        audio_device = 0;
    }
}