#include "../tomb3/pch.h"
#include "drawprimitive.h"
#include "winmain.h"
#include "hwrender.h"
#include <GL/gl.h>

// Definizioni delle variabili globali
DRAWPRIMITIVE DrawPrimitive;
SETTEXTURE SetTexture;
BEGINSCENE BeginScene;
ENDSCENE EndScene;
SETRENDERSTATE SetRenderState;

// Funzioni OpenGL
GLenum ConvertPrimitiveType(int dxPrimitiveType)
{
    switch (dxPrimitiveType)
    {
    case 4: // TRIANGLELIST
        return GL_TRIANGLES;
    case 5: // TRIANGLESTRIP
        return GL_TRIANGLE_STRIP;
    case 6: // TRIANGLEFAN
        return GL_TRIANGLE_FAN;
    case 1: // POINTLIST
        return GL_POINTS;
    case 2: // LINELIST
        return GL_LINES;
    case 3: // LINESTRIP
        return GL_LINE_STRIP;
    default:
        return GL_TRIANGLES;
    }
}

int HWDrawPrimitive(int PrimitiveType, void* Vertices, ulong VertexCount)
{
    GLenum glPrimitive = ConvertPrimitiveType(PrimitiveType);
    
    // Assumendo che i vertici siano in formato GLVERTEX
    GLVERTEX* glVertices = (GLVERTEX*)Vertices;
    
    glBegin(glPrimitive);
    for (ulong i = 0; i < VertexCount; i++)
    {
        glTexCoord2f(glVertices[i].tu, glVertices[i].tv);
        glColor4ub(
            RGBA_GETRED(glVertices[i].color),
            RGBA_GETGREEN(glVertices[i].color),
            RGBA_GETBLUE(glVertices[i].color),
            RGBA_GETALPHA(glVertices[i].color)
        );
        glVertex3f(glVertices[i].sx, glVertices[i].sy, glVertices[i].sz);
    }
    glEnd();

    return 0; // Success
}

int HWSetTexture(ulong Stage, TEXHANDLE pTexture)
{
    glBindTexture(GL_TEXTURE_2D, pTexture);
    return 0;
}

int HWBeginScene()
{
    AppFrameRate();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    return 0;
}

int HWEndScene()
{
    // In OpenGL, lo swap dei buffer viene fatto altrove
    return 0;
}

int HWSetRenderState(ulong dwRenderStateType, ulong dwRenderState)
{
    switch (dwRenderStateType)
    {
    case 27: // ALPHABLENDENABLE
        if (dwRenderState)
            glEnable(GL_BLEND);
        else
            glDisable(GL_BLEND);
        break;
    case 19: // ZENABLE
        if (dwRenderState)
            glEnable(GL_DEPTH_TEST);
        else
            glDisable(GL_DEPTH_TEST);
        break;
    case 22: // CULLMODE
        if (dwRenderState == 1) // CULL_NONE
            glDisable(GL_CULL_FACE);
        else
        {
            glEnable(GL_CULL_FACE);
            glCullFace(dwRenderState == 2 ? GL_FRONT : GL_BACK);
        }
        break;
    }
    return 0;
}

void InitDrawPrimitive()
{
    // Inizializza le funzioni con le implementazioni OpenGL
    DrawPrimitive = HWDrawPrimitive;
    SetTexture = HWSetTexture;
    BeginScene = HWBeginScene;
    EndScene = HWEndScene;
    SetRenderState = HWSetRenderState;
}