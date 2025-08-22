#pragma once
#include "../global/types.h"
#include <SDL2/SDL.h>

// Definizioni per compatibilità
typedef SDL_Window* HWND;
typedef void* HINSTANCE;

typedef RECT* LPRECT;

// Simple GUID replacement for compatibility
typedef struct {
    unsigned long Data1;
    unsigned short Data2;
    unsigned short Data3;
    unsigned char Data4[8];
} GUID;
typedef GUID* LPGUID;
typedef const char* LPCSTR;
typedef void* LPVOID;

// Forward declarations
struct DXDIRECTSOUNDINFO;
struct DEVICEINFO;
struct DXCONFIG;

// Simplified device capabilities structure
struct GLCAPS
{
    ulong maxTextureWidth;
    ulong maxTextureHeight;
    bool supportsNPOT; // Non-power-of-two textures
    bool supportsFBO;  // Framebuffer objects
    bool supportsVBO;  // Vertex buffer objects
    bool supportsPBO;  // Pixel buffer objects
};

struct DIRECT3DINFO
{
    char About[100];
    char Name[100];
    GUID Guid;
    LPGUID lpGuid;
    ulong nDisplayMode;
    DISPLAYMODE* DisplayMode;
    GLCAPS caps;
    long index;
};

struct DXDIRECTSOUNDINFO
{
    char About[100];
    char Name[100];
    GUID Guid;
    LPGUID lpGuid;
};

struct DEVICEINFO
{
    ulong nD3DInfo;
    DIRECT3DINFO* D3DInfo;
    ulong nDSInfo;
    DXDIRECTSOUNDINFO* DSInfo;
};

// Prototipi delle funzioni
void DXClearBuffers(ulong flags, ulong color);
bool DXGetDeviceInfo(DEVICEINFO* device, HWND hWnd, HINSTANCE hInstance);
bool DXStartRenderer(DEVICEINFO* device, DXCONFIG* config, bool createNew, bool windowed);
bool DXSwitchVideoMode(long needed, long current, bool disableZBuffer);
bool DXUpdateFrame(bool runMessageLoop, LPRECT rect);
long DXToggleFullScreen();
void DXMove(long x, long y);
void DXFreeDeviceInfo(DEVICEINFO* device);
bool DXEnumDirectSound(LPGUID lpGuid, LPCSTR lpcstrDescription, LPCSTR lpcstrModule, LPVOID lpContext);

// Funzioni di utilità OpenGL
GLenum DXFormatToGLFormat(long bpp, bool& hasAlpha);

// Funzioni di supporto (dichiarate altrove ma necessarie qui)
void DXCreateMaxTPages(bool firstTime);
void WinFreeDX(bool free_dx);
bool WinDXInit(DEVICEINFO* device, DXCONFIG* config, bool firstTime);
void HWR_InitState();
void setup_screen_size();
void Log(const char* format, ...);