//-----------------------------------------------------------------------------
// RackAFXDLL.h
//
// Interface definitions
//-----------------------------------------------------------------------------
#define NOMINMAX	// --- needed for VSTGUI4 min/max defs
#include <windows.h>
#include "Rafx2Plugin.h" 

const UINT CURRENT_PLUGIN_API = 64;

#define DllExport extern "C" __declspec(dllexport)

DllExport Rafx2PluginBase* createRafx2Plugin();
DllExport UINT getAPIVersion();

