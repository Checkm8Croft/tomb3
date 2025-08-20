#include "../tomb3/pch.h"
#include "display.h"
#include "../3dsystem/3d_gen.h"
#include "winmain.h"

double screen_sizer = 1.0;
double game_sizer = 1.0;
long VidSizeLocked;
short DumpX = 25;
short DumpY = 25;
short DumpWidth = 50;
short DumpHeight = 1;
char GtFullScreenClearNeeded;

void setup_screen_size()
{
    DISPLAYMODE* dm;
    long w, h, sw, sh;

    // Get display mode from OpenGL device info
    if (App.deviceInfo && App.deviceInfo->nDisplayInfo > 0) {
        GLDISPLAYINFO* displayInfo = &App.deviceInfo->DisplayInfo[App.glConfig.nVMode];
        if (displayInfo->nDisplayMode > 0) {
            dm = &displayInfo->DisplayMode[App.glConfig.nVMode];
        } else {
            // Fallback to current window size
            w = App.width;
            h = App.height;
            goto calculate_viewport;
        }
    } else {
        // Fallback to current window size
        w = App.width;
        h = App.height;
        goto calculate_viewport;
    }
    
    w = dm->w;
    h = dm->h;

calculate_viewport:
    sw = long(w * screen_sizer);
    sh = long(h * screen_sizer);

    if (sw > w)
        sw = w;

    if (sh > h)
        sh = h;

    phd_InitWindow((w - sw) / 2, (h - sh) / 2, sw, sh, 20, farz, 80, w, h);
    DumpX = short((w - sw) / 2);
    DumpY = short((h - sh) / 2);
    DumpWidth = (short)sw;
    DumpHeight = (short)sh;
    GtFullScreenClearNeeded = 1;
}

void IncreaseScreenSize()
{
    if (screen_sizer != 1)
    {
        screen_sizer += 0.08F;

        if (screen_sizer > 1)
            screen_sizer = 1;

        game_sizer = screen_sizer;
        setup_screen_size();
    }
}

void DecreaseScreenSize()
{
    if (screen_sizer != 0.44F)
    {
        screen_sizer -= 0.08F;

        if (screen_sizer < 0.44F)
            screen_sizer = 0.44F;

        game_sizer = screen_sizer;
        setup_screen_size();
    }
}

void TempVideoAdjust(long a, double sizer)
{
    VidSizeLocked = 1;

    if (sizer != screen_sizer)
    {
        screen_sizer = sizer;
        setup_screen_size();
    }
}

void TempVideoRemove()
{
    VidSizeLocked = 0;

    if (screen_sizer != game_sizer)
    {
        screen_sizer = game_sizer;
        setup_screen_size();
    }
}