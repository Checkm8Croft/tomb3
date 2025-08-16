#pragma once
#include "../global/types.h"
#include <string>
#include <unordered_map>

// Config file path (puoi cambiarlo)
#define CONFIG_FILE "tomb3_config.txt"

bool OpenConfig(const std::string& filename = CONFIG_FILE);
void CloseConfig();
void CFG_WriteLong(const std::string& key, ulong value);
void CFG_WriteBool(const std::string& key, bool value);
void CFG_WriteString(const std::string& key, const std::string& value);
void CFG_WriteFloat(const std::string& key, float value);
bool CFG_ReadLong(const std::string& key, ulong& value, ulong defaultValue);
bool CFG_ReadBool(const std::string& key, bool& value, bool defaultValue);
bool CFG_ReadString(const std::string& key, std::string& value, const std::string& defaultValue);
bool CFG_ReadFloat(const std::string& key, float& value, float defaultValue);
bool CFG_KeyWasCreated();

extern std::unordered_map<std::string, std::string> configMap;