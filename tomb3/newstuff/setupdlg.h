#pragma once
#include "../global/types.h"

// Forward declarations per compatibilità
struct DEVICEINFO;
struct DXCONFIG;

#ifdef _WIN32
#include <windows.h>
typedef HINSTANCE HINSTANCE_TYPE;
#else
typedef void* HINSTANCE_TYPE;
#endif

bool SetupDialog(DEVICEINFO* device, DXCONFIG* cfg, HINSTANCE_TYPE hinstance);