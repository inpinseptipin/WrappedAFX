#include "Rafx2Plugin.h"
#include "..\PluginKernel\plugingui.h"

// --- CTOR
Rafx2Plugin::Rafx2Plugin()
{
	// --- RAFX2+
	pluginCore = nullptr;
	pluginCore = new PluginCore();
	//if (!pluginCore)
	//	; // --- critical failure log to output window

	guiPluginConnector = new GUIPluginConnector(pluginCore);
	//if (!guiPluginConnector)
	//	; // --- critical failure log to output window

	// --- trackpad init
	setupTrackPad();
}

Rafx2Plugin::~Rafx2Plugin()
{
	if (guiPluginConnector)
		delete guiPluginConnector;
	guiPluginConnector = nullptr;

	if (pluginCore)
		delete pluginCore;
	pluginCore = nullptr;
}

bool Rafx2Plugin::initialize(PluginInfo& _pluginInfo)
{
	if (!pluginCore) return false;
	return pluginCore->initialize(_pluginInfo);
}

bool Rafx2Plugin::prepareForPlay(ResetInfo& info)
{
	if (!pluginCore) return false;
	return pluginCore->reset(info);
}

bool Rafx2Plugin::processAudioBuffers(ProcessBufferInfo& processInfo)
{
	if (!pluginCore) return false;
	return pluginCore->processAudioBuffers(processInfo);
}

bool Rafx2Plugin::processMIDIEvent(midiEvent& event)
{
	if (!pluginCore) return false;
	return pluginCore->processMIDIEvent(event);
}

// --- Rafx2Plugin::processRafx2Message
bool Rafx2Plugin::processRafx2Message(const Rafx2MessageInfo& messageInfo)
{
	// --- decode message
	switch (messageInfo.message)
	{
		// --- this can only come from RackAFX, so we need to handle that thunk issue
		case PLUGINGUI_REGISTER_CUSTOMVIEW:
		{
			if (!messageInfo.inMessageData || !pluginCore) return false;
			MessageInfo messageStuff;
			messageStuff.message = PLUGINGUI_REGISTER_CUSTOMVIEW;
			messageStuff.inMessageData = messageInfo.inMessageData;
			messageStuff.inMessageString = messageInfo.inMessageString;
			messageStuff.outMessageData = messageInfo.outMessageData;
			pluginCore->processMessage(messageStuff);

			break;
		}
		case PLUGINGUI_DIDOPEN:
		{
			if (!pluginCore) return false;
			MessageInfo messageStuff;
			messageStuff.message = PLUGINGUI_DIDOPEN;
			pluginCore->processMessage(messageStuff);
			break;
		}

		case PLUGINGUI_WILLCLOSE:
		{
			if (!pluginCore) return false;
			MessageInfo messageStuff;
			messageStuff.message = PLUGINGUI_WILLCLOSE;
			pluginCore->processMessage(messageStuff);
			break;
		}

		case PLUGINGUI_TIMERPING:
		{
			if (!pluginCore) return false;
			MessageInfo messageStuff;
			messageStuff.message = PLUGINGUI_TIMERPING;
			pluginCore->processMessage(messageStuff);
			break;
		}

		case PLUGINGUI_QUERY_HASUSERCUSTOM:
		{
			if (!pluginCore) return false;
			if (messageInfo.inMessageString == "GUI_PANEL_WILL_CHANGE" == 0)
			{
				MessageInfo messageStuff;
				messageStuff.message = PLUGINGUI_QUERY_HASUSERCUSTOM;
				messageStuff.inMessageString.assign("GUI_PANEL_WILL_CHANGE");
				pluginCore->processMessage(messageStuff);
			}

			return true; // handled
		}

		// --------------------------------------------------------------- //

		// --- return descriptor
		case PLUGIN_QUERY_DESCRIPTION:
		{
			if(!messageInfo.outMessageData || !pluginCore) return false;
			Rafx2PluginDescriptor* pidescriptor = (Rafx2PluginDescriptor*)messageInfo.outMessageData;

			// --- fill out descriptor
			pidescriptor->pluginName = pluginCore->getPluginName();
			pidescriptor->shortPluginName = pluginCore->getShortPluginName();
			pidescriptor->vendorName = pluginCore->getVendorName();
			pidescriptor->pluginTypeCode = pluginCore->getPluginType();
			pidescriptor->latencyInSamples = (unsigned int)pluginCore->getLatencyInSamples();
			pidescriptor->hasSidechain = pluginCore->hasSidechain();
			pidescriptor->wantsMIDI = pluginCore->wantsMIDI();
			pidescriptor->hasCustomGUI = pluginCore->hasCustomGUI();
			pidescriptor->tailTimeInMSec = (float)pluginCore->getTailTimeInMSec();
			pidescriptor->numPluginParameters = (unsigned int)pluginCore->getPluginParameterCount();

			return true; // handled
		}

		// --- return the Rafx2PluginParameter equivalent
		case PLUGIN_QUERY_PARAMETER:
		{
			if (!messageInfo.inMessageData || !messageInfo.outMessageData || !pluginCore) return false;

			unsigned int* index = (unsigned int*)messageInfo.inMessageData;
			Rafx2PluginParameter* piparameter = (Rafx2PluginParameter*)messageInfo.outMessageData;

			// --- get the core parameter and pass info back
			PluginParameter* piParam = pluginCore->getPluginParameterByIndex(*index);
			if (!piParam) return false; // --- not handled

			piparameter->parameterName = piParam->getControlName();
			piparameter->parameterUnits = piParam->getControlUnits();
			piparameter->stringList = piParam->getCommaSeparatedStringList();

			piparameter->variableType = piParam->getControlVariableType();
			piparameter->controlID = piParam->getControlID();
			piparameter->detectorMode = piParam->getDetectorMode();
			piparameter->controlTaper = piParam->getControlTaper();
			piparameter->boundvariableType = piParam->getBoundVariableType();

			piparameter->logMeter = piParam->getLogMeter();
			piparameter->invertedMeter = piParam->getInvertedMeter();
			piparameter->enableParameterSmoothing = piParam->getParameterSmoothing();
			piparameter->discreteSwitch = piParam->getIsDiscreteSwitch();

			piparameter->meterAttack_ms = piParam->getMeterAttack_ms();
			piparameter->meterRelease_ms = piParam->getMeterRelease_ms();
			piparameter->minValue = piParam->getMinValue();
			piparameter->maxValue = piParam->getMaxValue();
			piparameter->defaultValue = piParam->getDefaultValue();
			piparameter->controlValue = piParam->getControlValue();

			piparameter->smoothingTimeMsec = piParam->getSmoothingTimeMsec();

			return true;	// handled
		}
		case PLUGIN_QUERY_TRACKPAD_X:
		{
			if (!messageInfo.outMessageData || !pluginCore) return false;

			JSControl* jsControl = (JSControl*)messageInfo.outMessageData;
			*jsControl = joystick_X_Control;
			return true;
		}
		case PLUGIN_QUERY_TRACKPAD_Y:
		{
			if (!messageInfo.outMessageData || !pluginCore) return false;

			JSControl* jsControl = (JSControl*)messageInfo.outMessageData;
			*jsControl = joystick_Y_Control;
			return true;
		}

		// --- from GUI interaction; this is only for sending back GUI infor,
		//     and NOT for adjusting internal variables
		case PLUGINGUI_PARAMETER_CHANGED:
		{
			if (!messageInfo.inMessageData || !pluginCore) return false;
			Rafx2PluginParameter* piParam = (Rafx2PluginParameter*)messageInfo.inMessageData;
			pluginCore->guiParameterChanged(piParam->controlID, piParam->controlValue);

			return true;	// handled
		}

		// --- open our GUI
		case PLUGINGUI_USER_CUSTOMOPEN:
		{
			if (!messageInfo.inMessageData || !pluginCore) return false;

			// --- make the GUI
			CreateGUIInfo* guiInfo = (CreateGUIInfo*)messageInfo.inMessageData;
			guiPluginConnector->setParentConnector(guiInfo->guiPluginConnector);

			std::vector<PluginParameter*>* PluginParameterPtr = pluginCore->makePluginParameterVectorCopy();

			// --- create GUI
			VSTGUI::UTF8StringPtr _xmlFile = "PluginGUI.uidesc";
			pluginGUI = new VSTGUI::PluginGUI(_xmlFile);
			if (!pluginGUI) return false;

			bool opened = ((VSTGUI::PluginGUI*)pluginGUI)->open("Editor", guiInfo->window, PluginParameterPtr, VSTGUI::PlatformType::kHWND, guiPluginConnector, nullptr);

			// --- delete the PluginParameterPtr guts, and pointer too...
			for (std::vector<PluginParameter*>::iterator it = PluginParameterPtr->begin(); it != PluginParameterPtr->end(); ++it)
			{
				delete *it;
			}

			delete PluginParameterPtr;

			if (opened)
			{
				float width, height = 0.f;

				((VSTGUI::PluginGUI*)pluginGUI)->getSize(width, height);
				guiInfo->width = width;
				guiInfo->height = height;

				((VSTGUI::PluginGUI*)pluginGUI)->setGUIWindowFrame(guiInfo->guiWindowFrame);
			}
			return true;
		}

		// --- close our GUI
		case PLUGINGUI_USER_CUSTOMCLOSE:
		{
			if (pluginGUI)
			{
				// --- clear it
				guiPluginConnector->setParentConnector(nullptr);

				// --- close it
				((VSTGUI::PluginGUI*)pluginGUI)->close();

				// --- self-destruct
				VSTGUI::PluginGUI* oldGUI = ((VSTGUI::PluginGUI*)pluginGUI);
				pluginGUI = 0;
				oldGUI->forget();
			}
			return true;
		}

		// --- resync the GUI to the current parameter values
		case PLUGINGUI_USER_CUSTOMSYNC:
		{
			resyncGUI();
			return true;
		}

		// --- for sendGUI updates
		case PLUGINGUI_EXTERNAL_SET_ACTUALVALUE:
		{
			if (!pluginCore) return false;
			if (!pluginGUI) return false;

			GUIParameter* guiParam = (GUIParameter*)messageInfo.inMessageData;
			if (guiParam)
			{
				ParameterUpdateInfo paramInfo;
				pluginCore->updatePluginParameter(guiParam->controlID, guiParam->actualValue, paramInfo);
				resyncGUIControl(guiParam->controlID);
			}
			return true; // handled
		}
	}
	return false; // not handled
}

// --- parameter functions
//
// --- these two are called from bufferprocessing only
bool Rafx2Plugin::updatePluginParameter(uint32_t controlID, double actualValue)
{
	if (!pluginCore) return false;
//	if (pluginGUI) return false;

	ParameterUpdateInfo paramInfo;
	paramInfo.bufferProcUpdate = true;
	return pluginCore->updatePluginParameter(controlID, actualValue, paramInfo);
}

bool Rafx2Plugin::updatePluginParameterNormalized(uint32_t controlID, double normalizedValue, bool applyTaper, bool updateBoundVariable)
{
	if (!pluginCore) return false;

	ParameterUpdateInfo paramInfo;
	paramInfo.bufferProcUpdate = true;
	paramInfo.applyTaper = applyTaper;
	bool success = pluginCore->updatePluginParameterNormalized(controlID, normalizedValue, paramInfo);
	if(updateBoundVariable)
		pluginCore->updatePIParamBoundValue(controlID);

	return success;
}

// --- these can be called at any time; not used in RAFX2 implementation
void Rafx2Plugin::setParameterNormalizedByIndex(uint32_t index, double normalizedValue)
{
	if (!pluginCore) return;
	pluginCore->getPluginParameterByIndex(index)->setControlValueNormalized(normalizedValue);
}

void Rafx2Plugin::setParameterByIndex(uint32_t index, double actualValue)
{
	if (!pluginCore) return;
	pluginCore->getPluginParameterByIndex(index)->setControlValue(actualValue);
}

void Rafx2Plugin::setParameterNormalizedByControlID(uint32_t controlID, double normalizedValue)
{
	if (!pluginCore) return;
	pluginCore->getPluginParameterByControlID(controlID)->setControlValueNormalized(normalizedValue);
}

void Rafx2Plugin::setParameterByControlID(uint32_t controlID, double actualValue)
{
	if (!pluginCore) return;
	pluginCore->getPluginParameterByControlID(controlID)->setControlValue(actualValue);
}

double Rafx2Plugin::getParameterNormalizedByIndex(uint32_t index)
{
	if (!pluginCore) return 0.0;
	return pluginCore->getPluginParameterByIndex(index)->getControlValueNormalized();
}

double Rafx2Plugin::getParameterByIndex(uint32_t index)
{
	if (!pluginCore) return 0.0;
	return pluginCore->getPluginParameterByIndex(index)->getControlValue();
}

double Rafx2Plugin::getParameterNormalizedByControlID(uint32_t controlID)
{
	if (!pluginCore) return 0.0;
	return pluginCore->getPluginParameterByControlID(controlID)->getControlValueNormalized();
}

double Rafx2Plugin::getParameterByControlID(uint32_t controlID)
{
	if (!pluginCore) return 0.0;
	return pluginCore->getPluginParameterByControlID(controlID)->getControlValue();
}

AuxParameterAttribute* Rafx2Plugin::getAuxParameterAttributeByIndex(uint32_t index, uint32_t attributeID)
{
	if (!pluginCore) return nullptr;
	return pluginCore->getPluginParameterByIndex(index)->getAuxAttribute(attributeID);
}

AuxParameterAttribute* Rafx2Plugin::getAuxParameterAttributeByControlID(uint32_t controlID, uint32_t attributeID)
{
	if (!pluginCore) return nullptr;
	return pluginCore->getPluginParameterByControlID(controlID)->getAuxAttribute(attributeID);
}

uint32_t Rafx2Plugin::getDefaultChannelIOConfigForChannelCount(uint32_t channelCount)
{
	if (!pluginCore) return 0;
	return pluginCore->getDefaultChannelIOConfigForChannelCount(channelCount);
}

bool Rafx2Plugin::setVectorJoystickParameters(const VectorJoystickData& vectorJoysickData)
{
	if (!pluginCore) return false;
	return pluginCore->setVectorJoystickParameters(vectorJoysickData);
}

bool Rafx2Plugin::hasParameterWithIndex(uint32_t index)
{
	if (!pluginCore) return false;
	if (pluginCore->getPluginParameterByIndex(index)) return true;
	else return false;
}

bool Rafx2Plugin::hasParameterWithControlID(uint32_t controlID)
{
	if (!pluginCore) return false;
	if (pluginCore->getPluginParameterByControlID(controlID)) return true;
	else return false;
}

void Rafx2Plugin::setPluginHostConnector(IPluginHostConnector* _pluginHostConnector)
{
	if (pluginHostConnector)
		delete pluginHostConnector;

	pluginHostConnector = nullptr;

	// --- sets the parent connector for RAFX only
	pluginHostConnector = new PluginHostConnector(_pluginHostConnector);

	if (!pluginCore) return;
	return pluginCore->setPluginHostConnector(pluginHostConnector); // send them OUR interface
}

// --- GUI sync functions
void Rafx2Plugin::resyncGUIControl(uint32_t controlID)
{
	if (!pluginGUI) return;
	((VSTGUI::PluginGUI*)pluginGUI)->syncGUIControl(controlID);
}

void Rafx2Plugin::resyncGUI()
{
	if (!pluginGUI) return;

	for (unsigned int i = 0; i < pluginCore->getPluginParameterCount(); i++)
	{
		PluginParameter* piParam = pluginCore->getPluginParameterByIndex(i);
		if (piParam)
		{
			((VSTGUI::PluginGUI*)pluginGUI)->syncGUIControl(piParam->getControlID());
		}
	}

	return;
}

void Rafx2Plugin::setupTrackPad()
{
	// **--0x6E0E--**

	// --- RAFX Trackpad support
	joystick_X_Control.trackpadIndex = -1;
	joystick_X_Control.midiControl = false;
	joystick_X_Control.midiControlCommand = 176;
	joystick_X_Control.midiControlName = 16;
	joystick_X_Control.midiControlChannel = 0;
	joystick_X_Control.joystickValue = 0.0;
	joystick_X_Control.korgVectorJoystickOrientation = false;
	joystick_X_Control.enableParamSmoothing = false;
	joystick_X_Control.smoothingTimeInMs = 20.00;

	joystick_Y_Control.trackpadIndex = -1;
	joystick_Y_Control.midiControl = false;
	joystick_Y_Control.midiControlCommand = 176;
	joystick_Y_Control.midiControlName = 17;
	joystick_Y_Control.midiControlChannel = 0;
	joystick_Y_Control.joystickValue = 0.0;
	joystick_Y_Control.korgVectorJoystickOrientation = false;
	joystick_Y_Control.enableParamSmoothing = false;
	joystick_Y_Control.smoothingTimeInMs = 20.00;

	// **--0x8F8F--**
}
