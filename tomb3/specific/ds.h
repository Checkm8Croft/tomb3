#pragma once
#include "../global/types.h"
#include <SDL2/SDL_audio.h>
#include <SDL2/SDL_mixer.h>
#include <OpenAL/al.h>
#include <OpenAL/alc.h>
#define DSBVOLUME_MIN (-10000)
#define DSBPLAY_LOOPING (0x00000001)
// Struttura per i campioni audio
struct AUDIO_SAMPLE
{
    Uint8* data;
    Uint32 length;
    Uint32 frequency;
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

struct AUDIO_CHANNEL
{
    bool active;
    AUDIO_SAMPLE* sample;
    int volume;
    int pitch;
    int pan;
    bool playing;
    Uint32 position;
    Mix_Chunk* chunk;    // SDL2 mixer chunk
    int channel;         // SDL2 mixer channel number
    bool isActive;   
    
#ifdef _WIN32
    LPDIRECTSOUNDBUFFER buffer;
#else
    // Implementazione OpenAL
    ALuint source;
    ALuint buffer;
#endif

    // Costruttore
    AUDIO_CHANNEL() : active(false)
    {
#ifdef _WIN32
        buffer = nullptr;
#else
        source = 0;
        buffer = 0;
#endif
    }

    // Metodo Start - Avvia la riproduzione
    bool Start(long loop = 0)
    {
        if (!active) return false;

#ifdef _WIN32
        if (buffer)
        {
            buffer->SetCurrentPosition(0); // Rewind
            DWORD flags = (loop & 1) ? DSBPLAY_LOOPING : 0;
            HRESULT hr = buffer->Play(0, 0, flags);
            return SUCCEEDED(hr);
        }
#else
        if (source && buffer)
        {
            alSourcei(source, AL_BUFFER, buffer);
            alSourcei(source, AL_LOOPING, (loop & 1) ? AL_TRUE : AL_FALSE);
            alSourcePlay(source);
            return alGetError() == AL_NO_ERROR;
        }
#endif
        return false;
    }

    // Metodo Stop - Ferma la riproduzione
    void Stop()
    {
        if (!active) return;

#ifdef _WIN32
        if (buffer)
        {
            buffer->Stop();
        }
#else
        if (source)
        {
            alSourceStop(source);
        }
#endif
    }

    // Metodo Release - Rilascia le risorse
    void Release()
    {
        if (!active) return;

#ifdef _WIN32
        if (buffer)
        {
            buffer->Stop();
            buffer->Release();
            buffer = nullptr;
        }
#else
        if (source)
        {
            alSourceStop(source);
            alDeleteSources(1, &source);
            source = 0;
        }
        if (buffer)
        {
            alDeleteBuffers(1, &buffer);
            buffer = 0;
        }
#endif
        active = false;
    }

    // Metodo per impostare il volume
    void SetVolume(long volume)
    {
        if (!active) return;

        // Converti da range [-10000, 0] a [0.0, 1.0] per OpenAL
        float gain = 1.0f;
        if (volume < 0)
        {
            gain = (10000.0f + volume) / 10000.0f;
            if (gain < 0.0f) gain = 0.0f;
        }

#ifdef _WIN32
        if (buffer)
        {
            buffer->SetVolume(volume);
        }
#else
        if (source)
        {
            alSourcef(source, AL_GAIN, gain);
        }
#endif
    }

    // Metodo per impostare il pan
    void SetPan(long pan)
    {
        if (!active) return;

        // Converti da range [-10000, 10000] a [-1.0, 1.0] per OpenAL
        float panValue = pan / 10000.0f;

#ifdef _WIN32
        if (buffer)
        {
            buffer->SetPan(pan);
        }
#else
        if (source)
        {
            alSource3f(source, AL_POSITION, panValue, 0.0f, 0.0f);
        }
#endif
    }

    // Metodo per impostare il pitch
    void SetPitch(long pitch)
    {
        if (!active) return;

        // Converti pitch (1000 = normale, 2000 = doppia velocità, etc.)
        float pitchValue = pitch / 1000.0f;

#ifdef _WIN32
        if (buffer)
        {
            // DirectSound non supporta nativamente il pitch change
            // Potresti aver bisogno di implementare questo diversamente
        }
#else
        if (source)
        {
            alSourcef(source, AL_PITCH, pitchValue);
        }
#endif
    }

    // Metodo per verificare se sta riproducendo
    bool IsPlaying()
    {
        if (!active) return false;

#ifdef _WIN32
        if (buffer)
        {
            DWORD status;
            buffer->GetStatus(&status);
            return (status & DSBSTATUS_PLAYING) != 0;
        }
#else
        if (source)
        {
            ALint state;
            alGetSourcei(source, AL_SOURCE_STATE, &state);
            return state == AL_PLAYING;
        }
#endif
        return false;
    }
};
extern AUDIO_CHANNEL DS_Samples[32];

// Dichiarazioni delle funzioni globali
bool DS_Init(void* hwnd);
void DS_Shutdown();