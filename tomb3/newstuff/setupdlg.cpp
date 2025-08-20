#include "../tomb3/pch.h"
#include "setupdlg.h"
#include "../tomb3/tomb3.h"
#include "registry.h"
#include <stdio.h>

// Strutture semplificate per la configurazione
struct DISPLAY_MODE {
    int width;
    int height;
    int bpp;
    const char* description;
};

struct DISPLAY_CONFIG {
    bool fullscreen;
    int width;
    int height;
    int bpp;
    bool vsync;
    bool filtering;
};

static DISPLAY_CONFIG currentConfig;
static DISPLAY_MODE availableModes[] = {
    {640, 480, 16, "640x480 High Colour (16 Bit)"},
    {640, 480, 32, "640x480 True Colour (32 Bit)"},
    {800, 600, 16, "800x600 High Colour (16 Bit)"},
    {800, 600, 32, "800x600 True Colour (32 Bit)"},
    {1024, 768, 16, "1024x768 High Colour (16 Bit)"},
    {1024, 768, 32, "1024x768 True Colour (32 Bit)"},
    {1280, 720, 32, "1280x720 True Colour (32 Bit)"},
    {1280, 1024, 32, "1280x1024 True Colour (32 Bit)"},
    {1920, 1080, 32, "1920x1080 True Colour (32 Bit)"}
};
static int numModes = sizeof(availableModes) / sizeof(availableModes[0]);

// Funzione semplice per mostrare le opzioni di configurazione
void ShowConfigOptions() {
    printf("=== Tomb Raider 3 Configuration ===\n");
    printf("1. Fullscreen: %s\n", currentConfig.fullscreen ? "Yes" : "No");
    printf("2. Resolution: %dx%d (%d bit)\n", currentConfig.width, currentConfig.height, currentConfig.bpp);
    printf("3. V-Sync: %s\n", currentConfig.vsync ? "Yes" : "No");
    printf("4. Filtering: %s\n", currentConfig.filtering ? "Yes" : "No");
    printf("5. Apply and Save\n");
    printf("6. Cancel\n");
    printf("Choose option (1-6): ");
}

// Funzione per applicare la configurazione
void ApplyConfig() {
    // Qui implementeresti l'applicazione delle impostazioni OpenGL
    printf("Applying configuration...\n");
    printf("Mode: %s\n", currentConfig.fullscreen ? "Fullscreen" : "Windowed");
    printf("Resolution: %dx%d\n", currentConfig.width, currentConfig.height);
    printf("Color Depth: %d bit\n", currentConfig.bpp);
    printf("V-Sync: %s\n", currentConfig.vsync ? "Enabled" : "Disabled");
    printf("Filtering: %s\n", currentConfig.filtering ? "Enabled" : "Disabled");
}

bool SetupDialog(void* device, void* cfg, void* hinstance) {
    // Carica la configurazione esistente
    if (OpenRegistry("Tomb3Config")) {
        REG_ReadBool("Fullscreen", currentConfig.fullscreen, false);
        REG_ReadLong("Width", (ulong&)currentConfig.width, 800);
        REG_ReadLong("Height", (ulong&)currentConfig.height, 600);
        REG_ReadLong("Bpp", (ulong&)currentConfig.bpp, 32);
        REG_ReadBool("VSync", currentConfig.vsync, true);
        REG_ReadBool("Filtering", currentConfig.filtering, true);
        CloseRegistry();
    } else {
        // Valori di default
        currentConfig.fullscreen = false;
        currentConfig.width = 800;
        currentConfig.height = 600;
        currentConfig.bpp = 32;
        currentConfig.vsync = true;
        currentConfig.filtering = true;
    }

    int choice = 0;
    bool configChanged = false;

    while (choice != 5 && choice != 6) {
        ShowConfigOptions();
        
        if (scanf("%d", &choice) != 1) {
            while (getchar() != '\n'); // Clear input buffer
            continue;
        }

        switch (choice) {
            case 1:
                currentConfig.fullscreen = !currentConfig.fullscreen;
                configChanged = true;
                break;
                
            case 2: {
                printf("\nAvailable Resolutions:\n");
                for (int i = 0; i < numModes; i++) {
                    printf("%d. %s\n", i + 1, availableModes[i].description);
                }
                printf("Choose resolution (1-%d): ", numModes);
                
                int resChoice;
                if (scanf("%d", &resChoice) == 1 && resChoice >= 1 && resChoice <= numModes) {
                    currentConfig.width = availableModes[resChoice - 1].width;
                    currentConfig.height = availableModes[resChoice - 1].height;
                    currentConfig.bpp = availableModes[resChoice - 1].bpp;
                    configChanged = true;
                }
                break;
            }
                
            case 3:
                currentConfig.vsync = !currentConfig.vsync;
                configChanged = true;
                break;
                
            case 4:
                currentConfig.filtering = !currentConfig.filtering;
                configChanged = true;
                break;
                
            case 5:
                if (configChanged) {
                    ApplyConfig();
                    
                    // Salva la configurazione
                    if (OpenRegistry("Tomb3Config")) {
                        REG_WriteBool("Fullscreen", currentConfig.fullscreen);
                        REG_WriteLong("Width", currentConfig.width);
                        REG_WriteLong("Height", currentConfig.height);
                        REG_WriteLong("Bpp", currentConfig.bpp);
                        REG_WriteBool("VSync", currentConfig.vsync);
                        REG_WriteBool("Filtering", currentConfig.filtering);
                        CloseRegistry();
                    }
                }
                return true;
                
            case 6:
                return false;
                
            default:
                printf("Invalid choice! Please try again.\n");
                break;
        }
        
        printf("\n");
    }
    
    return false;
}

// Funzioni di compatibilità (vuote per non rompere il codice esistente)
static void InitDDDrivers(void* hwnd) {}
static void InitD3DDrivers(void* hwnd) {}
static void InitD3DDrivers(void* hwnd, long nDrivers) {}
static void InitVideoModes(void* hwnd, long nD3D) {}
static void InitVideoModes(void* hwnd, long nDD, long nD3D) {}
static void InitTextures(void* hwnd, long nDD, long nD3D) {}
static void InitDSAdapters(void* hwnd) {}
static void InitDialogBox(void* hwnd) {}