#pragma once
#include "../global/types.h"
#include <SDL2/SDL.h>

// Forward declarations
struct DEVICEINFO;
struct DXCONFIG;

// Define Windows types for compatibility
typedef SDL_RWops* HANDLE;
typedef void* LPVOID;
typedef void* LPOVERLAPPED;
typedef SDL_RWops* FILE_HANDLE;

// Windows constants
#define GMEM_FIXED 0
#define GENERIC_READ 0
#define OPEN_EXISTING 0
#define FILE_ATTRIBUTE_NORMAL 0
#define FILE_FLAG_SEQUENTIAL_SCAN 0
#define INVALID_HANDLE_VALUE nullptr
#define FILE_CURRENT SEEK_CUR

// Define WAVEFORMATEX structure for audio
typedef struct {
    unsigned short wFormatTag;
    unsigned short nChannels;
    unsigned long nSamplesPerSec;
    unsigned long nAvgBytesPerSec;
    unsigned short nBlockAlign;
    unsigned short wBitsPerSample;
    unsigned short cbSize;
} WAVEFORMATEX;
typedef WAVEFORMATEX* LPWAVEFORMATEX;

// Windows functions replacements
void* GlobalAlloc(int flags, size_t size);
void GlobalFree(void* ptr);
HANDLE CreateFile(const char* filename, int access, int share, void* security, int creation, int attributes, void* template_file);
bool ReadFile(HANDLE file, void* buffer, size_t size, size_t* bytes_read, void* overlapped);
bool SetFilePointer(HANDLE file, long distance, long* distance_high, int method);
bool CloseHandle(HANDLE file);
void lstrcpy(char* dest, const char* src);
void wsprintf(char* dest, const char* format, ...);

// Additional function declarations
void AdjustTextureUVs(bool reset);
const char* GetFullPath(const char* name);
void build_ext(char* name, const char* ext);
long Read_Strings(long num, char** strings, char** buffer, ulong* read, HANDLE file);