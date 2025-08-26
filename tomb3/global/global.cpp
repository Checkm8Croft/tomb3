#include "types.h"
#include "../specific/dxshell.h"
// In un file .cpp (es. global.cpp)
DXCONFIG AppDXConfig;
DEVICEINFO AppDeviceInfo;
bool AppWindowed;
DXCONFIG G_dxConfig;
bool bMonoScreen;
GLVERTEX* CurrentGLVertex;
bool zBufWriteEnabled = false;
bool zBufCompareEnabled = false;
bool AlphaBlendEnabled = false;
