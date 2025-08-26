#include "../tomb3/pch.h"
#include "hwrender.h"
#include "texture.h"
#include "../3dsystem/hwinsert.h"
#include "init.h"
#include "winmain.h"
#include "output.h"
#include "drawbars.h"
#include "../tomb3/tomb3.h"
#include <cmath>
#include "single.h"
// Includi i file header necessari per le definizioni
#include "../game/control.h"
#include "specific.h"
#include <SDL2/SDL.h>

// Usa le definizioni consistenti con gli altri header
#define MAX_UNROLL_VERTICES 8192

// Variabili globali - dichiarazioni compatibili
extern long nDrawnPoints;
extern bool bBlueEffect;
extern GLTEXTUREINFO* TexturePtrs[MAX_TPAGES];  // Usa MAX_TPAGES invece di MAX_TEXTURES
extern GLTEXTUREINFO Textures[MAX_TPAGES];
extern long GlobalAlpha;  // Usa long invece di ulong per compatibilità
extern TEXTUREBUCKET Buckets[MAX_BUCKETS];
extern GLVERTEX* UnRollBuffer;  // Usa lo stesso tipo dichiarato in init.h

// OpenGL function pointers
typedef void (*DrawPrimitiveFunc)(GLenum, void*, GLsizei);
typedef void (*SetRenderStateFunc)(GLenum, GLint);
typedef void (*BeginSceneFunc)();
typedef void (*EndSceneFunc)();
typedef void (*DrawRoutineFunc)(long, void*, long, long);

static DrawPrimitiveFunc DrawPrimitive = nullptr;
static SetRenderStateFunc SetRenderState = nullptr;
static BeginSceneFunc BeginScene = nullptr;
static EndSceneFunc EndScene = nullptr;
static DrawRoutineFunc DrawRoutine = nullptr;

float GammaOption = 3.0F;
uchar ColorTable[256];

static GLenum dpPrimitiveType;









void HWR_EnableColorSubtraction(bool enable)
{
    if (enable)
    {
        glBlendFunc(GL_ZERO, GL_ONE_MINUS_SRC_COLOR);
    }
    else
    {
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    }
}

void HWR_ResetZBuffer()
{
    zBufWriteEnabled = false;
    zBufCompareEnabled = false;
    
    glDisable(GL_DEPTH_TEST);
    glDepthMask(GL_FALSE);
}

void HWR_ResetColorKey()
{
    HWR_EnableAlphaBlend(false);
    HWR_EnableColorKey(false);
    HWR_EnableColorAddition(false);
}

void HWR_ResetCurrentTexture()
{
    HWR_SetCurrentTexture(0);
}

void HWR_BeginScene()
{
    bBlueEffect = 0;
    HWR_GetAllTextureHandles();
    HWR_EnableAlphaBlend(false);
    HWR_EnableColorKey(false);
    HWR_EnableColorAddition(false);
    HWR_ResetCurrentTexture();
    
    if (BeginScene)
        BeginScene();
        
    nDrawnPoints = 0;

    if (G_dxConfig.bZBuffer)
    {
        for (int i = 0; i < MAX_BUCKETS; i++)
        {
            Buckets[i].TPage = (GLTEXTUREINFO*)-1;
            Buckets[i].nVtx = 0;
        }
    }
}


void HWR_DrawRoutines(long nVtx, void* vtx, long nDrawType, long TPage)
{
    if (!DrawPrimitive) return;

    switch (nDrawType)
    {
    case DT_POLY_GT:
        HWR_EnableAlphaBlend(false);
        HWR_SetCurrentTexture(TexturePtrs[TPage]);
        DrawPrimitive(dpPrimitiveType, vtx, nVtx);
        return;

    case DT_POLY_WGT:
        HWR_SetCurrentTexture(TexturePtrs[TPage]);
        HWR_EnableZBuffer(true, true);
        HWR_EnableAlphaBlend(true);
        HWR_EnableColorAddition(false);
        DrawPrimitive(dpPrimitiveType, vtx, nVtx);
        HWR_EnableZBuffer(false, true);
        return;

    case DT_POLY_G:
        HWR_SetCurrentTexture(0);
        HWR_EnableAlphaBlend(false);
        DrawPrimitive(dpPrimitiveType, vtx, nVtx);
        return;

    case DT_LINE_SOLID:
        HWR_SetCurrentTexture(0);
        HWR_EnableAlphaBlend(false);
        HWR_EnableColorAddition(false);
        DrawPrimitive(GL_LINES, vtx, nVtx);
        return;

    case DT_LINE_ALPHA:
        HWR_SetCurrentTexture(0);
        HWR_EnableAlphaBlend(true);
        HWR_EnableColorAddition(true);
        DrawPrimitive(GL_LINES, vtx, nVtx);
        return;

    case DT_POLY_GA:
        HWR_SetCurrentTexture(0);
        HWR_EnableAlphaBlend(true);
        HWR_EnableColorAddition(false);
        DrawPrimitive(dpPrimitiveType, vtx, nVtx);
        return;

    case DT_POLY_WGTA:
        HWR_EnableZBuffer(false, true);
        HWR_SetCurrentTexture(TexturePtrs[TPage]);
        HWR_EnableColorAddition(true);
        HWR_EnableAlphaBlend(true);
        DrawPrimitive(dpPrimitiveType, vtx, nVtx);
        return;

    case DT_POLY_COLSUB:
        HWR_EnableZBuffer(false, true);
        HWR_SetCurrentTexture(TexturePtrs[TPage]);
        HWR_EnableColorSubtraction(true);
        HWR_EnableAlphaBlend(true);
        DrawPrimitive(dpPrimitiveType, vtx, nVtx);
        return;

    case DT_POLY_GTA:
        HWR_SetCurrentTexture(0);
        HWR_EnableAlphaBlend(true);
        HWR_EnableColorAddition(true);
        DrawPrimitive(dpPrimitiveType, vtx, nVtx);
        return;
    }
}

void HWR_InitGamma(float gamma)
{
    if (tomb3.psx_contrast)
        gamma = 2.5F;

    gamma = 1.0F / (gamma / 10.0F * 4.0F);

    for (int i = 0; i < 256; i++)
        ColorTable[i] = uchar(pow((double)i / 256.0F, gamma) * 256.0F);
}

void HWR_InitState()
{
    glDisable(GL_CULL_FACE);
    glShadeModel(GL_SMOOTH);
    glDisable(GL_DITHER);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDisable(GL_BLEND);
    glDisable(GL_ALPHA_TEST);

    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    if (tomb3.disable_gamma)
        GammaOption = 2.5F;

    HWR_InitGamma(GammaOption);
    HWR_ResetCurrentTexture();
    HWR_ResetColorKey();
    HWR_ResetZBuffer();
    DrawRoutine = HWR_DrawRoutines;
    bAlphaTesting = true;
    GlobalAlpha = 0xFF000000;
}

bool HWR_Init()
{
    VertexBuffer = 0;
    HWR_InitState();
    return true;
}

void HWR_DrawPolyList(long num, long* pSort)
{
    void* vtx;
    short* pInfo;
    long polyType;
    long x0, y0, x1, y1, bar, p;
    short nVtx, nDrawType, TPage;

    dpPrimitiveType = GL_TRIANGLE_FAN;

    for (int i = 0; i < num; i++)
    {
        pInfo = (short*)pSort[0];
        polyType = pSort[2];

        if (polyType == POLYTYPE_HEALTHBAR ||
            polyType == POLYTYPE_AIRBAR ||
            polyType == POLYTYPE_DASHBAR ||
            polyType == POLYTYPE_COLDBAR)
        {
            x0 = pInfo[0];
            y0 = pInfo[1];
            x1 = pInfo[2];
            y1 = pInfo[3];
            bar = pInfo[4];
            p = pInfo[5];

            if (polyType == POLYTYPE_HEALTHBAR)
                DoPSXHealthBar(x0, y0, x1, y1, bar, p);
            else if (polyType == POLYTYPE_DASHBAR)
                DoPSXDashBar(x0, y0, x1, y1, bar, p);
            else if (polyType == POLYTYPE_AIRBAR)
                DoPSXAirBar(x0, y0, x1, y1, bar, p);
            else if (polyType == POLYTYPE_COLDBAR)
                DoPSXColdBar(x0, y0, x1, y1, bar, p);

            pSort += 3;
            continue;
        }

        nDrawType = pInfo[0];
        TPage = pInfo[1];
        nVtx = pInfo[2];
        vtx = *((void**)(pInfo + 3));
        
        if (DrawRoutine)
            DrawRoutine(nVtx, vtx, nDrawType, TPage);
            
        pSort += 3;
    }
}

void HWR_DrawPolyListBF(long num, long* pSort)
{
    // Implementazione semplificata per OpenGL
    for (int i = 0; i < num; i++)
    {
        short* pInfo = (short*)pSort[0];
        short nDrawType = pInfo[0];
        short TPage = pInfo[1];
        short nVtx = pInfo[2];
        void* vtx = *((void**)(pInfo + 3));

        if (DrawRoutine)
            DrawRoutine(nVtx, vtx, nDrawType, TPage);
            
        pSort += 3;
    }
}

void HWR_FreeTexturePages()
{
    for (int i = 0; i < MAX_TPAGES; i++)
    {
        if (Textures[i].hasAlpha) // Usa hasAlpha come flag per texture di livello
        {
            // Cleanup OpenGL texture
            if (Textures[i].handle)
            {
                glDeleteTextures(1, &Textures[i].handle);
                Textures[i].handle = 0;
            }
        }
    }
}

void HWR_GetAllTextureHandles()
{
    GLTEXTUREINFO* tex;
    long n = 0;

    memset(TexturePtrs, 0, sizeof(TexturePtrs));

    for (int i = 0; i < MAX_TPAGES; i++)
    {
        tex = &Textures[i];
        if (tex->hasAlpha) // Usa hasAlpha come flag per texture di livello
        {
            TexturePtrs[n] = tex;
            n++;
        }
    }

    for (int i = 0; i < MAX_TPAGES; i++)
    {
        tex = &Textures[i];
        if (tex->bpp == 32) // Usa bpp come flag per texture di picture
        {
            TexturePtrs[n] = tex;
            n++;
        }
    }
}

void HWR_LoadTexturePages(long nPages, uchar* src, uchar* palette)
{
    HWR_FreeTexturePages();

    for (int i = 0; i < nPages; i++)
    {
        // Creazione texture OpenGL
        GLuint texture;
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);
        
        // Caricamento dati texture (placeholder)
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 256, 256, 0, GL_RGBA, GL_UNSIGNED_BYTE, src);
        
        Textures[i].handle = texture;
        Textures[i].hasAlpha = true; // Segna come texture di livello
        
        src += 0x20000;
    }

    HWR_GetAllTextureHandles();
}

void HWR_SetCurrentTexture(GLTEXTUREINFO* tex)
{
    static GLuint lastTextureHandle = 0;

    if (!tex)
    {
        lastTextureHandle = 0;
        glBindTexture(GL_TEXTURE_2D, 0);
        return;
    }

    if (tex->handle != lastTextureHandle)
    {
        glBindTexture(GL_TEXTURE_2D, tex->handle);
        lastTextureHandle = tex->handle;
    }
}

// Inizializzazione funzioni OpenGL
void InitOpenGLFunctions()
{
    // Implementazioni di base
    DrawPrimitive = [](GLenum prim, void* data, GLsizei count) {
        // Implementazione del disegno OpenGL
        glBegin(prim);
        GLVERTEX* vtx = (GLVERTEX*)data;
        for (int i = 0; i < count; i++) {
            glColor4ubv((GLubyte*)&vtx[i].color);
            glTexCoord2f(vtx[i].tu, vtx[i].tv);
            glVertex3f(vtx[i].sx, vtx[i].sy, vtx[i].sz);
        }
        glEnd();
    };

    SetRenderState = [](GLenum state, GLint value) {
        // Impostazione stato OpenGL
        switch (state) {
            case GL_BLEND:
                if (value) glEnable(GL_BLEND);
                else glDisable(GL_BLEND);
                break;
            case GL_DEPTH_TEST:
                if (value) glEnable(GL_DEPTH_TEST);
                else glDisable(GL_DEPTH_TEST);
                break;
            // Aggiungi altri stati se necessario
        }
    };

    
}