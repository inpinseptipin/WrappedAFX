#ifndef __RAFX2PLUGIN__
#define __RAFX2PLUGIN__

// --- normal includes
#include "Rafx2PluginBase.h"
#include "..\PluginKernel\PluginCore.h"


// --- forward decl
class GUIPluginConnector;
class PluginHostConnector;

// --- our plugin object, derived from Rafx2PluginBase
class Rafx2Plugin : public Rafx2PluginBase
{
public:
	Rafx2Plugin();
	virtual ~Rafx2Plugin();

	// --- process functions
	virtual bool processRafx2Message(const Rafx2MessageInfo& messageInfo);
	virtual bool prepareForPlay(ResetInfo& info);
	virtual bool initialize(PluginInfo& _pluginInfo);
	virtual bool processAudioBuffers(ProcessBufferInfo& processInfo);
	virtual bool processMIDIEvent(midiEvent& event);

	// --- parameter functions: setters
	virtual bool updatePluginParameter(uint32_t controlID, double actualValue); /// specifically for RAFX2
	virtual bool updatePluginParameterNormalized(uint32_t controlID, double normalizedValue, bool applyTaper = true, bool updateBoundVariable = false); /// specifically for RAFX2
	virtual void setParameterNormalizedByIndex(uint32_t index, double normalizedValue);
	virtual void setParameterByIndex(uint32_t index, double actualValue);
	virtual void setParameterNormalizedByControlID(uint32_t controlID, double normalizedValue);
	virtual void setParameterByControlID(uint32_t controlID, double actualValue);
	virtual bool setVectorJoystickParameters(const VectorJoystickData& vectorJoysickData);

	// --- parameter functions: getters
	virtual double getParameterNormalizedByIndex(uint32_t index);
	virtual double getParameterByIndex(uint32_t index);
	virtual double getParameterNormalizedByControlID(uint32_t controlID);
	virtual double getParameterByControlID(uint32_t controlID);
	virtual bool hasParameterWithIndex(uint32_t index);
	virtual bool hasParameterWithControlID(uint32_t controlID);
	virtual void setupTrackPad();

	// --- aux attributes
	virtual AuxParameterAttribute* getAuxParameterAttributeByIndex(uint32_t index, uint32_t attributeID);
	virtual AuxParameterAttribute* getAuxParameterAttributeByControlID(uint32_t controlID, uint32_t attributeID);

	// --- miscellaneous
	virtual uint32_t getDefaultChannelIOConfigForChannelCount(uint32_t channelCount);
	virtual void setPluginHostConnector(IPluginHostConnector* _pluginHostConnector);

protected:
	// --- PluginKernel
	PluginBase* pluginCore = nullptr;
	void* pluginGUI = nullptr;

	// --- Plugin Interfaces
	GUIPluginConnector* guiPluginConnector = nullptr;		// --- GUI --> plugin
	PluginHostConnector* pluginHostConnector = nullptr;		// --- plugin --> host

	// --- GUI sync functions; internally used only
	void resyncGUI();
	void resyncGUIControl(uint32_t controlID);
};

// --- container for a custom view pointer
class CustomViewController : public ICustomView
{
public:
	// --- constructor
	CustomViewController(ICustomView* _customViewIF) { customViewIF = _customViewIF; }

	// --- the ICustomView IF --- these are safe function calls for overrides
	virtual void updateView()
	{
		if (customViewIF)
			customViewIF->updateView();
	}

	virtual void pushDataValue(double data)
	{
		if (customViewIF)
			customViewIF->pushDataValue(data);
	}

	virtual void sendMessage(void* data)
	{
		if (customViewIF)
			customViewIF->sendMessage(data);
	}

	// --- set/get a new pointer
	void setCustomViewPtr(ICustomView* _customViewIF) { customViewIF = _customViewIF; }
	const ICustomView* getCustomViewPtr() { return customViewIF; }

	// --- clear pointer
	void clearCustomViewPtr() { customViewIF = nullptr; }


private:
	ICustomView* customViewIF = nullptr;
};


// --- IGUIPluginConnector object
class GUIPluginConnector : public IGUIPluginConnector
{
public:
	GUIPluginConnector(PluginBase* _plugin) { plugin = _plugin; }

	virtual ~GUIPluginConnector()
	{
		
		for (customViewControllerMap::const_iterator it = customSubControllerMap.begin(), end = customSubControllerMap.end(); it != end; ++it)
		{
			delete it->second;
		}
		for (customViewControllerMap::const_iterator it = customViewMap.begin(), end = customViewMap.end(); it != end; ++it)
		{
			delete it->second;
		}
	}

	void setParentConnector(IGUIPluginConnector* _parentConnector) { parentConnector = _parentConnector; }

	virtual void parameterChanged(int32_t controlID, double actualValue, double normalizedValue)
	{
		if (parentConnector)
			parentConnector->parameterChanged(controlID, actualValue, normalizedValue);

		if (plugin)
			plugin->guiParameterChanged(controlID, actualValue);
	}

	virtual double getNormalizedPluginParameter(int32_t controlID)
	{
		if (parentConnector)
			return parentConnector->getNormalizedPluginParameter(controlID);

		return 0.0;
	}

	virtual double getActualPluginParameter(int32_t controlID)
	{
		if (parentConnector)
			return parentConnector->getActualPluginParameter(controlID);

		return 0.0;
	}

	virtual bool registerSubcontroller(std::string subcontrollerName, ICustomView* customViewConnector)
	{
		// --- do we have this in our map already?
		CustomViewController* pCVC = nullptr;

		customViewControllerMap::const_iterator it = customSubControllerMap.find(subcontrollerName);
		if (it != customSubControllerMap.end())
		{
			pCVC = it->second;
			pCVC->setCustomViewPtr(customViewConnector);
		}
		else
		{
			pCVC = new CustomViewController(customViewConnector);
			customSubControllerMap.insert(std::make_pair(subcontrollerName, pCVC));
		}

		MessageInfo info(PLUGINGUI_REGISTER_SUBCONTROLLER);
		info.inMessageString = subcontrollerName;
		info.inMessageData = pCVC;

		if (plugin && plugin->processMessage(info))
			return true;

		return false;
	}

	virtual bool deRregisterSubcontroller(ICustomView* customViewConnector)
	{
		CustomViewController* pCVC = getCustomSubController(customViewConnector);
		if (pCVC)
		{
			pCVC->clearCustomViewPtr();

			MessageInfo info(PLUGINGUI_DE_REGISTER_SUBCONTROLLER);
			info.inMessageString = "";
			info.inMessageData = pCVC;

			if (plugin && plugin->processMessage(info))
				return true;
		}

		return false;
	}

	virtual bool registerCustomView(std::string customViewName, ICustomView* customViewConnector)
	{
		// --- do we have this in our map already?
		CustomViewController* pCVC = nullptr;

		customViewControllerMap::const_iterator it = customViewMap.find(customViewName);
		if (it != customViewMap.end())
		{
			pCVC = it->second;
			pCVC->setCustomViewPtr(customViewConnector);
		}
		else
		{
			pCVC = new CustomViewController(customViewConnector);
			customViewMap.insert(std::make_pair(customViewName, pCVC));
		}

		MessageInfo info(PLUGINGUI_REGISTER_CUSTOMVIEW);
		info.inMessageString = customViewName;
		info.inMessageData = pCVC;// customViewConnector;

		if (plugin && plugin->processMessage(info))
			return true;

		return false;
	}

	virtual bool deRegisterCustomView(ICustomView* customViewConnector)
	{
		CustomViewController* pCVC = getCustomViewController(customViewConnector);
		if (pCVC)
		{
			pCVC->clearCustomViewPtr();

			MessageInfo info(PLUGINGUI_DE_REGISTER_CUSTOMVIEW);
			info.inMessageString = "";
			info.inMessageData = pCVC;

			if (plugin && plugin->processMessage(info))
				return true;
		}

		return false;
	}

	virtual bool guiDidOpen()
	{
		if (!plugin) return false;
		MessageInfo info(PLUGINGUI_DIDOPEN);
		return plugin->processMessage(info);
	}

	virtual bool guiWillClose()
	{
		if (!plugin) return false;
		for (customViewControllerMap::const_iterator it = customViewMap.begin(), end = customViewMap.end(); it != end; ++it)
		{
			it->second->clearCustomViewPtr();
		}

		MessageInfo info(PLUGINGUI_WILLCLOSE);
		return plugin->processMessage(info);
	}

	virtual bool guiTimerPing()
	{
		if (!plugin) return false;
		MessageInfo info(PLUGINGUI_TIMERPING);
		return plugin->processMessage(info);
	}

	virtual bool checkNonBoundValueChange(int tag, float normalizedValue)
	{
		if (!plugin) return false;

		// --- do any additional stuff here
		// --- dispatch non-bound value changes

		return false;
	}

protected:
	PluginBase* plugin;
	IGUIPluginConnector* parentConnector = nullptr;

	typedef std::map<std::string, CustomViewController*> customViewControllerMap;
	customViewControllerMap customViewMap;

	CustomViewController* getCustomViewController(ICustomView* customViewConnector)
	{
		CustomViewController* pCVC = nullptr;
		for (customViewControllerMap::const_iterator it = customViewMap.begin(), end = customViewMap.end(); it != end; ++it)
		{
			if (it->second->getCustomViewPtr() == customViewConnector)
				return it->second;
		}

		return nullptr;
	}

	customViewControllerMap customSubControllerMap;
	CustomViewController* getCustomSubController(ICustomView* customViewConnector)
	{
		CustomViewController* pCVC = nullptr;
		for (customViewControllerMap::const_iterator it = customSubControllerMap.begin(), end = customSubControllerMap.end(); it != end; ++it)
		{
			if (it->second->getCustomViewPtr() == customViewConnector)
				return it->second;
		}

		return nullptr;
	}

};

// --- IPluginHostConnector object
class PluginHostConnector : public IPluginHostConnector
{
public:
	PluginHostConnector(IPluginHostConnector* _parentConnector)
	{
		parentConnector = _parentConnector;
	}

	virtual ~PluginHostConnector() {}

	virtual void sendHostMessage(const HostMessageInfo& hostMessageInfo)
	{
		switch (hostMessageInfo.hostMessage)
		{
			case sendGUIUpdate:
			{
				GUIUpdateData guiUpdateData = hostMessageInfo.guiUpdateData;

				HostMessageInfo parentInfo;
				parentInfo.hostMessage = sendGUIUpdate;

				for (unsigned int i = 0; i < guiUpdateData.guiParameters.size(); i++)
				{
					GUIParameter guiParam = guiUpdateData.guiParameters[i];

					parentInfo.guiParameter.controlID = guiParam.controlID;
					parentInfo.guiParameter.actualValue = guiParam.actualValue;

					if (parentConnector)
						parentConnector->sendHostMessage(parentInfo);
				}

				// --- clean up
				for (unsigned int i = 0; i < guiUpdateData.guiParameters.size(); i++)
					guiUpdateData.guiParameters.pop_back();

				break;
			}

			case sendRAFXStatusWndText:
			{
				if (parentConnector)
					parentConnector->sendHostMessage(hostMessageInfo);
				break;
			}

			default:
				break;
		}
	}

protected:
	IPluginHostConnector* parentConnector = nullptr;
};
#endif //#ifndef