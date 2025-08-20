#pragma once

#include "../global/types.h"

// Definizione dei tipi Windows per compatibilità
typedef const char* LPCSTR;
typedef void* LPVOID;
typedef unsigned long DWORD;

// Config file path
#define CONFIG_FILE "tomb3_config.txt"

// Funzioni principali di configurazione
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

// Funzioni di compatibilità con il registro Windows
bool REG_OpenKey(LPCSTR lpSubKey);
bool OpenRegistry(LPCSTR SubKeyName);
void REG_CloseKey();
void CloseRegistry();
void REG_WriteLong(char* SubKeyName, ulong value);
void REG_WriteBool(char* SubKeyName, bool value);
void REG_WriteBlock(char* SubKeyName, LPVOID block, long size);
void REG_WriteString(char* SubKeyName, char* string, long length);
void REG_WriteFloat(char* SubKeyName, float value);
bool REG_ReadLong(char* SubKeyName, ulong& value, ulong defaultValue);
bool REG_ReadBool(char* SubKeyName, bool& value, bool defaultValue);
bool REG_ReadBlock(char* SubKeyName, LPVOID block, long size, LPVOID dBlock);
bool REG_ReadString(char* SubKeyName, char* value, long length, char* defaultValue);
bool REG_ReadFloat(char* SubKeyName, float& value, float defaultValue);
bool REG_KeyWasCreated();