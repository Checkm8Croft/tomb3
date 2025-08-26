#include "single.h"
#include <GL/gl.h>
#include <SDL2/SDL.h>

// Definizioni delle variabili globali
DXCONFIG G_dxConfig;
bool zBufWriteEnabled = false;
bool zBufCompareEnabled = false;
bool AlphaBlendEnabled = false;
DXCONFIG AppDXConfig;  // Aggiungi questa linea
bool bAlphaTesting = false;
DEVICEINFO AppDeviceInfo;  // Definizione
bool AppWindowed = false;
GLVERTEX* CurrentGLVertex = nullptr;
bool bMonoScreen = false;
// Implementazioni delle funzioni
void GLSaveScreen() {
    GLint viewport[4];
    glGetIntegerv(GL_VIEWPORT, viewport);
    
    // Implement screen capture functionality
    // This is a placeholder - implement based on your needs
    // You'd typically use glReadPixels here
    // to capture the screen to a buffer or file
}

void HWR_EnableColorAddition(bool enable) {
    if (enable) {
        glBlendFunc(GL_ONE, GL_ONE);
    } else {
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    }
}

void HWR_EnableZBuffer(bool write, bool compare) {
    if (!G_dxConfig.bZBuffer)
        return;

    if (write != zBufWriteEnabled) {
        glDepthMask(write ? GL_TRUE : GL_FALSE);
        zBufWriteEnabled = write;
    }

    if (compare != zBufCompareEnabled) {
        if (compare) {
            glEnable(GL_DEPTH_TEST);
            glDepthFunc(GL_LEQUAL);
        } else {
            glDisable(GL_DEPTH_TEST);
        }
        zBufCompareEnabled = compare;
    }
}

void HWR_EnableAlphaBlend(bool enable) {
    if (enable) {
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    } else {
        glDisable(GL_BLEND);
    }
}

void HWR_EnableColorKey(bool enable) {
    if (enable) {
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        AlphaBlendEnabled = true;
    } else {
        glDisable(GL_BLEND);
        AlphaBlendEnabled = false;
    }
}

void HWR_EndScene() {
    glFlush();
    SDL_GL_SwapWindow(App.sdl_window);
}