#pragma once

#include "../global/types.h"
// Non serve più <string> e <unordered_map>

// Config file path (puoi cambiarlo)
#define CONFIG_FILE "tomb3_config.txt"

bool OpenConfig(const char* filename);
void CloseConfig();
void CFG_WriteLong(const char* key, ulong value);
void CFG_WriteBool(const char* key, bool value);
void CFG_WriteString(const char* key, const char* value);
void CFG_WriteFloat(const char* key, float value);
bool CFG_ReadLong(const char* key, ulong* value, ulong defaultValue);
bool CFG_ReadBool(const char* key, bool* value, bool defaultValue);
bool CFG_ReadString(const char* key, char* value, const char* defaultValue, int valueSize);
bool CFG_ReadFloat(const char* key, float* value, float defaultValue);
bool CFG_KeyWasCreated();

// Puoi usare una semplice struct o array per la config invece di unordered_map