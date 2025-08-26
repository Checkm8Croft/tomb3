#define _GNU_SOURCE 1

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include "single.h"



// Usa array C-style invece di vector
#define MAX_TEXTURE_SIZE (4096 * 4096 * 4)
static unsigned char texture_buffer[MAX_TEXTURE_SIZE];
// Definisci manualmente le strutture se necessario
struct timespec {
    time_t tv_sec;
    long tv_nsec;
};

struct tm {
    int tm_sec;
    int tm_min;
    int tm_hour;
    int tm_mday;
    int tm_mon;
    int tm_year;
    int tm_wday;
    int tm_yday;
    int tm_isdst;
};
#include "../tomb3/pch.h"
#include "texture.h"
#include "winmain.h"
#include "../global/types.h"
GLTEXTUREINFO Textures[MAX_TPAGES];
GLTEXTUREINFO* TexturePtrs[MAX_TPAGES];
long nTextures;
GLTEXTUREINFO TextureSurfaces[MAX_TPAGES];

static bool bSetColorKey = true;
static bool bMakeGrey;

// ==================== FUNZIONI OPENGL ====================

long GLTextureNewPalette(uchar* palette)
{
    return 1;
}

void GLResetPalette()
{
    bSetColorKey = true;
}

void GLReleasePalette()
{
}

GLuint GLTextureGetHandle(GLSurface* surf)
{
    return surf->texture_id;
}

bool GLCreateTextureSurface(GLTEXTUREINFO* tex, GLTEXTUREINFO* glinfo)
{
    glGenTextures(1, &tex->handle);
    glBindTexture(GL_TEXTURE_2D, tex->handle);
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    
    tex->width = 256;
    tex->height = 256;
    tex->bpp = glinfo->bpp;
    tex->hasAlpha = glinfo->hasAlpha;
    
    GLenum format = tex->hasAlpha ? GL_RGBA : GL_RGB;
    GLenum type = GL_UNSIGNED_BYTE;
    
    glTexImage2D(GL_TEXTURE_2D, 0, format, 256, 256, 0, 
                 format, type, NULL);
    
    return glGetError() == GL_NO_ERROR;
}

long GLTextureMakeDeviceSurface(long w, long h, GLTEXTURE* list)
{
    GLTEXTURE* tex;
    long index;

    index = DXTextureFindTextureSlot((GLTEXTUREINFO*)list);
    if (index < 0)
        return -1;

    tex = &list[index];
    memset(tex, 0, sizeof(GLTEXTURE));
    tex->nWidth = w;
    tex->nHeight = h;
    tex->dwFlags = TF_USED;

    glGenTextures(1, &tex->texHandle);
    glBindTexture(GL_TEXTURE_2D, tex->texHandle);
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, 
                 GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    
    if (glGetError() != GL_NO_ERROR)
        return -1;

    return index;
}

void GLTextureSetGreyScale(bool set)
{
    bMakeGrey = set;
}

long GLTextureFindTextureSlot(GLTEXTURE* tex)
{
    for (int i = 0; i < MAX_TPAGES; i++)
    {
        if (!(tex[i].dwFlags & TF_USED))
            return i;
    }
    return -1;
}

void GLClearAllTextures(GLTEXTURE* list)
{
    for (int i = 0; i < MAX_TPAGES; i++)
        GLTextureCleanup(i, list);
}

void GLTextureCleanup(long index, GLTEXTURE* list)
{
    GLTEXTURE* tex = &list[index];
    
    if (tex->texHandle)
    {
        glDeleteTextures(1, &tex->texHandle);
        tex->texHandle = 0;
    }
    
    tex->dwFlags = TF_EMPTY;
    tex->nWidth = 0;
    tex->nHeight = 0;
}

GLTEXTURE* GLRestoreSurfaceIfLost(long index, GLTEXTURE* list)
{
    return &list[index];
}

static void CopyTexture16(long w, long h, uchar* src, uchar* dest, long pitch, ulong flags)
{
    ushort* s = (ushort*)src;
    ulong* d;

    for (int i = 0; i < h; i++)
    {
        d = (ulong*)(dest + pitch * i);
        
        for (int j = 0; j < w; j++)
        {
            uchar r = (*s >> 7) & 0xF8;
            uchar g = (*s >> 2) & 0xF8;
            uchar b = (*s << 3) & 0xF8;
            uchar a = ((*s >> 15) & 1) ? 255 : 0;

            if (bMakeGrey)
            {
                r = b;
                g = b;
            }

            if (flags == TF_PICTEX)
                a = 255;

            *d++ = RGBA_MAKE(r, g, b, a);
            s++;
        }
    }
}

static void CopyTexture32(long w, long h, uchar* src, uchar* dest, long pitch, ulong flags)
{
    ulong* s = (ulong*)src;
    ulong* d;

    for (int i = 0; i < h; i++)
    {
        d = (ulong*)(dest + pitch * i);

        for (int j = 0; j < w; j++)
        {
            uchar r = RGBA_GETRED(*s);
            uchar g = RGBA_GETGREEN(*s);
            uchar b = RGBA_GETBLUE(*s);
            uchar a = RGBA_GETALPHA(*s);

            if (bMakeGrey)
            {
                r = b;
                g = b;
            }

            if (flags == TF_PICTEX)
                a = 255;

            *d++ = RGBA_MAKE(r, g, b, a);
            s++;
        }
    }
}

long GLTextureAdd(long w, long h, uchar* src, GLTEXTURE* list, long bpp, ulong flags)
{
    GLTEXTURE* tex;
    long index;

    index = GLTextureMakeDeviceSurface(w, h, list);
    if (index < 0)
        return -1;

    tex = &list[index];
    tex->dwFlags |= flags;

    uchar* convertedData = (uchar*)malloc(w * h * 4);
    
    if (bpp == 8888) {
        CopyTexture32(w, h, src, convertedData, w * 4, flags);
    } else if (bpp == 888) {
        uchar* s = src;
        uchar* d = convertedData;
        for (int i = 0; i < w * h; i++) {
            d[0] = s[0];
            d[1] = s[1];
            d[2] = s[2];
            d[3] = 255;
            s += 3;
            d += 4;
        }
    } else if (bpp == 565 || bpp == 555) {
        CopyTexture16(w, h, src, convertedData, w * 4, flags);
    }

    glBindTexture(GL_TEXTURE_2D, tex->texHandle);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, 
                 GL_RGBA, GL_UNSIGNED_BYTE, convertedData);
    free(convertedData);
    return index;
}

void GLCreateMaxTPages(long create)
{
    GLTEXTUREINFO* tex;
    long n;
    
    GLTEXTUREINFO defaultInfo;
    defaultInfo.bpp = 32;
    defaultInfo.hasAlpha = true;
    
    for (n = 0; n < MAX_TPAGES; n++)
    {
        tex = &TextureSurfaces[n];
        
        if (!GLCreateTextureSurface(tex, &defaultInfo))
            break;
    }
    
    nTextures = n;
}

void GLFreeTPages()
{
    for (int i = 0; i < MAX_TPAGES; i++)
    {
        GLTEXTUREINFO* tex = &TextureSurfaces[i];
        
        if (tex->handle)
        {
            glDeleteTextures(1, &tex->handle);
            tex->handle = 0;
        }
        
        memset(tex, 0, sizeof(GLTEXTUREINFO));
    }
    
    nTextures = 0;
}

// ==================== RIMOZIONE CODICE DIRECTX ====================

// Rimuovi completamente tutte le funzioni DirectX originali
// e sostituisci con funzioni di compatibilità

long DXTextureMakeDeviceSurface(long w, long h, GLTEXTURE* list)
{
    return GLTextureMakeDeviceSurface(w, h, list);
}

void DXTextureCleanup(long index, GLTEXTURE* list)
{
    GLTextureCleanup(index, list);
}

long DXTextureAdd(long w, long h, uchar* src, GLTEXTURE* list, long bpp, ulong flags)
{
    return GLTextureAdd(w, h, src, list, bpp, flags);
}

void DXCreateMaxTPages(long create)
{
    GLCreateMaxTPages(create);
}

void DXFreeTPages()
{
    GLFreeTPages();
}

// Funzioni stub per mantenere la compatibilità
long DXTextureNewPalette(uchar* palette) { return 1; }
void DXResetPalette() {}
void DXReleasePalette() {}