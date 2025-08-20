#include "../tomb3/pch.h"
#include "audio.h"
#include "file.h"
#include "winmain.h"
#include "../game/control.h"
#include "specific.h"
#include "../game/inventry.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

// Semplice sistema audio per cross-platform
#ifdef _WIN32
#include <windows.h>
#include <mmsystem.h>
#else
#include <unistd.h>
#endif

static TRACK_INFO TrackInfos[130];
static long XATrack;
static bool acm_ready;
static bool acm_loop_track;
static bool acm_paused;
long acm_volume;

// Variabili per la gestione dei file audio
static FILE* audioFile = NULL;
static bool musicPlaying = false;

// Semplice implementazione audio per ora (puoi sostituire con OpenAL/SDL dopo)
static void* audioContext = NULL;

BOOL ACMEnumCallBack(HACMDRIVERID hadid, DWORD_PTR dwInstance, DWORD fdwSupport)
{
    return 0;
}

void ACMCloseFile()
{
    if (audioFile)
    {
        fclose(audioFile);
        audioFile = NULL;
    }
}

bool ACMOpenFile(const char* name)
{
    ACMCloseFile();
    audioFile = fopen(name, "rb");
    return audioFile != NULL;
}

void ACMEmulateCDStop()
{
    if (!acm_ready) return;
    
    printf("Audio: Stop track %d\n", XATrack);
    musicPlaying = false;
}

void ACMEmulateCDPlay(long track, long mode)
{
    if (!acm_ready || track > 130 || !TrackInfos[track].size)
        return;

    ACMEmulateCDStop();
    XATrack = track;
    acm_loop_track = (mode != 0);

    printf("Audio: Play track %d, loop: %s\n", track, acm_loop_track ? "yes" : "no");
    musicPlaying = true;
}

void ThreadACMEmulateCDPlay(long track, long mode)
{
    ACMEmulateCDPlay(track, mode);
}

long ACMGetTrackLocation()
{
    if (!musicPlaying) return 0;
    
    // Simula posizione del brano
    static long position = 0;
    position = (position + 100) % 30000;
    return position;
}

void ACMMute()
{
    acm_paused = false;
    acm_volume = 0;
    printf("Audio: Muted\n");
}

void ACMSetVolume(long volume)
{
    if (!acm_ready) return;

    if (volume == 0)
    {
        if (musicPlaying)
        {
            ACMEmulateCDStop();
            acm_paused = true;
        }
    }
    else
    {
        if (acm_paused)
        {
            ACMEmulateCDPlay(XATrack, acm_loop_track);
            acm_paused = false;
        }

        acm_volume = volume;
        printf("Audio: Volume set to %ld\n", volume);
    }
}

long ACMHandleNotifications()
{
    return 1;
}

long ACMSetupNotifications()
{
    return 1;
}

bool ACMInit()
{
    acm_ready = false;

    // Inizializzazione semplice - da espandere con vero sistema audio
    printf("Audio: Initializing...\n");

    // Carica informazioni tracce
    char wadname[80];
    strcpy(wadname, "audio/cdaudio.wad");
    
    if (!ACMOpenFile(wadname))
    {
        // Prova percorso alternativo
        char fullPath[256];
        
        // Cerca in directory corrente
        strcpy(fullPath, wadname);
        if (!ACMOpenFile(fullPath))
        {
            // Cerca in directory dati
            #ifdef _WIN32
            strcpy(fullPath, "data/");
            #else
            strcpy(fullPath, "./data/");
            #endif
            strcat(fullPath, wadname);
            
            if (!ACMOpenFile(fullPath))
            {
                printf("Audio: Could not open %s\n", wadname);
                return false;
            }
        }
    }

    if (audioFile)
    {
        size_t read = fread(TrackInfos, sizeof(TRACK_INFO), 130, audioFile);
        printf("Audio: Loaded %zu track infos\n", read);
        ACMCloseFile();
    }

    acm_ready = true;
    acm_volume = 100;
    acm_paused = false;

    printf("Audio: Initialized successfully\n");
    return true;
}

void ACMClose()
{
    if (!acm_ready) return;

    ACMEmulateCDStop();
    ACMCloseFile();
    
    printf("Audio: Shutdown\n");
    acm_ready = false;
}