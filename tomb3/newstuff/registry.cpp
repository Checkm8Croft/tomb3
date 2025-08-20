#include "../tomb3/pch.h"
#include "registry.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef _WIN32
#include <windows.h>
#include <shlobj.h>
#else
#include <sys/stat.h>
#include <unistd.h>
#include <pwd.h>
#endif

// Struttura semplice per gestire la configurazione
typedef struct {
    char key[64];
    char value[256];
} ConfigEntry;

static ConfigEntry configEntries[100];
static int configCount = 0;
static bool configModified = false;
static bool configWasCreated = false;
static char configPath[512];

// Helper functions
const char* GetConfigPath() {
    if (configPath[0] == '\0') {
#ifdef _WIN32
        char appData[MAX_PATH];
        if (SUCCEEDED(SHGetFolderPathA(NULL, CSIDL_APPDATA, NULL, 0, appData))) {
            snprintf(configPath, sizeof(configPath), "%s\\TombRaider3\\%s", appData, CONFIG_FILE);
        } else {
            snprintf(configPath, sizeof(configPath), ".\\%s", CONFIG_FILE);
        }
#else
        // Linux/macOS - usa ~/.config/tombraider3/
        const char* home = getenv("HOME");
        if (!home) {
            struct passwd* pw = getpwuid(getuid());
            if (pw) home = pw->pw_dir;
        }
        
        if (home) {
            snprintf(configPath, sizeof(configPath), "%s/.config/tombraider3/%s", home, CONFIG_FILE);
            // Crea la directory se non esiste
            char dirPath[512];
            snprintf(dirPath, sizeof(dirPath), "%s/.config/tombraider3", home);
            mkdir(dirPath, 0755);
        } else {
            snprintf(configPath, sizeof(configPath), "./%s", CONFIG_FILE);
        }
#endif
    }
    return configPath;
}

bool OpenConfig(const char* filename) {
    configCount = 0;
    configModified = false;
    
    const char* path = GetConfigPath();
    FILE* file = fopen(path, "r");
    
    if (!file) {
        configWasCreated = true;
        return true; // File verrà creato al salvataggio
    }
    
    configWasCreated = false;
    char line[512];
    
    while (fgets(line, sizeof(line), file) && configCount < 100) {
        // Salta commenti e linee vuote
        if (line[0] == '#' || line[0] == '\n' || line[0] == '\r') continue;
        
        char* equals = strchr(line, '=');
        if (!equals) continue;
        
        *equals = '\0';
        char* key = line;
        char* value = equals + 1;
        
        // Rimuovi newline dal valore
        char* newline = strchr(value, '\n');
        if (newline) *newline = '\0';
        
        newline = strchr(value, '\r');
        if (newline) *newline = '\0';
        
        strncpy(configEntries[configCount].key, key, sizeof(configEntries[0].key) - 1);
        strncpy(configEntries[configCount].value, value, sizeof(configEntries[0].value) - 1);
        configCount++;
    }
    
    fclose(file);
    return true;
}

void CloseConfig() {
    if (configModified) {
        const char* path = GetConfigPath();
        FILE* file = fopen(path, "w");
        
        if (file) {
            fprintf(file, "# Tomb Raider 3 Configuration\n");
            fprintf(file, "# Auto-generated file\n\n");
            
            for (int i = 0; i < configCount; i++) {
                fprintf(file, "%s=%s\n", configEntries[i].key, configEntries[i].value);
            }
            
            fclose(file);
        }
    }
    
    configCount = 0;
    configModified = false;
}

void CFG_WriteLong(const char* key, ulong value) {
    // Cerca se la chiave esiste già
    for (int i = 0; i < configCount; i++) {
        if (strcmp(configEntries[i].key, key) == 0) {
            snprintf(configEntries[i].value, sizeof(configEntries[i].value), "%lu", value);
            configModified = true;
            return;
        }
    }
    
    // Aggiungi nuova entry
    if (configCount < 100) {
        strncpy(configEntries[configCount].key, key, sizeof(configEntries[0].key) - 1);
        snprintf(configEntries[configCount].value, sizeof(configEntries[0].value), "%lu", value);
        configCount++;
        configModified = true;
    }
}

void CFG_WriteBool(const char* key, bool value) {
    CFG_WriteLong(key, value ? 1 : 0);
}

void CFG_WriteString(const char* key, const char* value) {
    for (int i = 0; i < configCount; i++) {
        if (strcmp(configEntries[i].key, key) == 0) {
            strncpy(configEntries[i].value, value, sizeof(configEntries[i].value) - 1);
            configModified = true;
            return;
        }
    }
    
    if (configCount < 100) {
        strncpy(configEntries[configCount].key, key, sizeof(configEntries[0].key) - 1);
        strncpy(configEntries[configCount].value, value, sizeof(configEntries[0].value) - 1);
        configCount++;
        configModified = true;
    }
}

void CFG_WriteFloat(const char* key, float value) {
    char buffer[64];
    snprintf(buffer, sizeof(buffer), "%.5f", value);
    CFG_WriteString(key, buffer);
}

bool CFG_ReadLong(const char* key, ulong* value, ulong defaultValue) {
    for (int i = 0; i < configCount; i++) {
        if (strcmp(configEntries[i].key, key) == 0) {
            *value = strtoul(configEntries[i].value, NULL, 10);
            return true;
        }
    }
    
    *value = defaultValue;
    CFG_WriteLong(key, defaultValue);
    return false;
}

bool CFG_ReadBool(const char* key, bool* value, bool defaultValue) {
    ulong temp;
    bool found = CFG_ReadLong(key, &temp, defaultValue ? 1 : 0);
    *value = (temp != 0);
    return found;
}

bool CFG_ReadString(const char* key, char* value, const char* defaultValue, int valueSize) {
    for (int i = 0; i < configCount; i++) {
        if (strcmp(configEntries[i].key, key) == 0) {
            strncpy(value, configEntries[i].value, valueSize - 1);
            value[valueSize - 1] = '\0';
            return true;
        }
    }
    
    strncpy(value, defaultValue, valueSize - 1);
    value[valueSize - 1] = '\0';
    CFG_WriteString(key, defaultValue);
    return false;
}

bool CFG_ReadFloat(const char* key, float* value, float defaultValue) {
    for (int i = 0; i < configCount; i++) {
        if (strcmp(configEntries[i].key, key) == 0) {
            *value = atof(configEntries[i].value);
            return true;
        }
    }
    
    *value = defaultValue;
    CFG_WriteFloat(key, defaultValue);
    return false;
}

bool CFG_KeyWasCreated() {
    return configWasCreated;
}

// Funzioni di compatibilità con il vecchio codice
bool REG_OpenKey(LPCSTR lpSubKey) {
    return OpenConfig(lpSubKey);
}

bool OpenRegistry(LPCSTR SubKeyName) {
    return OpenConfig(SubKeyName);
}

void REG_CloseKey() {
    CloseConfig();
}

void CloseRegistry() {
    CloseConfig();
}

void REG_WriteLong(char* SubKeyName, ulong value) {
    CFG_WriteLong(SubKeyName, value);
}

void REG_WriteBool(char* SubKeyName, bool value) {
    CFG_WriteBool(SubKeyName, value);
}

void REG_WriteString(char* SubKeyName, char* string, long length) {
    CFG_WriteString(SubKeyName, string);
}

void REG_WriteFloat(char* SubKeyName, float value) {
    CFG_WriteFloat(SubKeyName, value);
}

bool REG_ReadLong(char* SubKeyName, ulong& value, ulong defaultValue) {
    return CFG_ReadLong(SubKeyName, &value, defaultValue);
}

bool REG_ReadBool(char* SubKeyName, bool& value, bool defaultValue) {
    return CFG_ReadBool(SubKeyName, &value, defaultValue);
}

bool REG_ReadString(char* SubKeyName, char* value, long length, char* defaultValue) {
    return CFG_ReadString(SubKeyName, value, defaultValue, length);
}

bool REG_ReadFloat(char* SubKeyName, float& value, float defaultValue) {
    return CFG_ReadFloat(SubKeyName, &value, defaultValue);
}

bool REG_KeyWasCreated() {
    return CFG_KeyWasCreated();
}

// Nota: REG_WriteBlock e REG_ReadBlock sono più complessi da implementare
// in un file di testo semplice. Potresti volerli omettere o implementare
// con encoding base64 se necessario.
void REG_WriteBlock(char* SubKeyName, LPVOID block, long size) {
    // Implementazione opzionale con base64
}

bool REG_ReadBlock(char* SubKeyName, LPVOID block, long size, LPVOID dBlock) {
    // Implementazione opzionale con base64
    return false;
}