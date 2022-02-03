// --- CMAKE generated variables for your plugin

#include "pluginstructures.h"

#ifndef _plugindescription_h
#define _plugindescription_h

#define QUOTE(name) #name
#define STR(macro) QUOTE(macro)
#define AU_COCOA_VIEWFACTORY_STRING STR(AU_COCOA_VIEWFACTORY_NAME)
#define AU_COCOA_VIEW_STRING STR(AU_COCOA_VIEW_NAME)

// --- AU Plugin Cocoa View Names (flat namespace)
#define AU_COCOA_VIEWFACTORY_NAME AUCocoaViewFactory_BB0B3ADE43A842AA9908D9D8603823F1
#define AU_COCOA_VIEW_NAME AUCocoaView_BB0B3ADE43A842AA9908D9D8603823F1

// --- BUNDLE IDs (MacOS Only)
const char* kAAXBundleID = "inpinseptipin.aax.DCOffsetv3.bundleID";
const char* kAUBundleID = "inpinseptipin.au.DCOffsetv3.bundleID";
const char* kVST3BundleID = "inpinseptipin.vst3.DCOffsetv3.bundleID";

// --- Plugin Names
const char* kPluginName = "DCOffsetv3";
const char* kShortPluginName = "DCOffsetv3";
const char* kAUBundleName = "DCOffsetv3";
const char* kAAXBundleName = "DCOffsetv3";
const char* kVSTBundleName = "DCOffsetv3";

// --- bundle name helper
inline static const char* getPluginDescBundleName()
{
#ifdef AUPLUGIN
	return kAUBundleName;
#endif

#ifdef AAXPLUGIN
	return kAAXBundleName;
#endif

#ifdef VSTPLUGIN
	return kVSTBundleName;
#endif

	// --- should never get here
	return kPluginName;
}

// --- Plugin Type
const pluginType kPluginType = pluginType::kFXPlugin;

// --- VST3 UUID
const char* kVSTFUID = "{BB0B3ADE-43A8-42AA-9908-D9D8603823F1}";

// --- 4-char codes
const int32_t kFourCharCode = 'ANwR';
const int32_t kAAXProductID = 'uERe';
const int32_t kManufacturerID = 'VNDR';

// --- Vendor information
const char* kVendorName = "inpinseptipin";
const char* kVendorURL = "inpinseptipin.github.io";
const char* kVendorEmail = "";

// --- Plugin Options
const bool kProcessFrames = true;
const uint32_t kBlockSize = DEFAULT_AUDIO_BLOCK_SIZE;
const bool kWantSidechain = false;
const uint32_t kLatencyInSamples = 0;
const double kTailTimeMsec = 0.000;
const bool kVSTInfiniteTail = false;
const bool kVSTSAA = false;
const uint32_t kVST3SAAGranularity = 1;
const uint32_t kAAXCategory = 0;

#endif


