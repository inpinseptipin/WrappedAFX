#ifndef AuxPort_AudioEffect_H
#define AuxPort_AudioEffect_H
#pragma once
/*
*			WrappedAFX
			"I tried to make it easier" - inpinseptipin

			BSD 3-Clause License

			Copyright (c) 2024, AuxAudio Enterprises LLP
			All rights reserved.

			Redistribution and use in source and binary forms, with or without
			modification, are permitted provided that the following conditions are met:

			1. Redistributions of source code must retain the above copyright notice, this
			   list of conditions and the following disclaimer.

			2. Redistributions in binary form must reproduce the above copyright notice,
			   this list of conditions and the following disclaimer in the documentation
			   and/or other materials provided with the distribution.

			3. Neither the name of the copyright holder nor the names of its
			   contributors may be used to endorse or promote products derived from
			   this software without specific prior written permission.

			THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
			AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
			IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
			DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
			FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
			DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
			SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
			CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
			OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
			OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.


*/
#include <vector>
#include <math.h>
#include "plugincore.h"
#include "fxobjects.h"
#include <unordered_map>
namespace AuxPort
{
/*===================================================================================*/
/*
	[Class] Abstraction over AudioFilter class in FXObjects [Don't Mess with it]
*/
	template<class sample, class knob>
	class Filter
	{
	public:
		Filter() = default;
		Filter(const Filter& filter) = default;
		void setFilterType(const filterAlgorithm& type)
		{
			filterParameters.algorithm = type;
		}
		void setParameters(const knob& centerFrequency, const knob& QFactor)
		{
			filterParameters.Q = QFactor;
			filterParameters.fc = centerFrequency;
			filterParameters.boostCut_dB = 1;
			filter.setParameters(filterParameters);
		}
		sample process(const sample& frame)
		{
			return filter.processAudioSample(frame);
		}
		~Filter() = default;
	private:
		AudioFilter filter;
		AudioFilterParameters filterParameters;

	};

	
/*===================================================================================*/
/*
	[Struct] Simple Struct for handling two channel Frames [DON'T MESS WITH IT]
*/
	template<class sample>
	struct Frame
	{
		sample left;
		sample right;
	};

	template<class sample, class knob>
	class Effect
	{
	public:
/*===================================================================================*/
/*
	[Constructor] Safely Allocates the memory
*/
		Effect<sample, knob>() = default;
/*===================================================================================*/
/*
	[Destructor] Safely Deallocates the memory
*/
		~Effect<sample, knob>() = default;
/*===================================================================================*/
/*
	[Copy Constructor] Safely Copies memory from one Effect Object to another
*/
		Effect<sample, knob>(const Effect<sample, knob>& kernel) = default;
/*===================================================================================*/
/*
	[Function] This function pushes the parameters to the WrappedFX class (DONT MESS WITH IT)
*/		
		void push(const std::vector<PluginParameter*> parameterList)
		{
			std::pair<uint32_t, PluginParameter*> parameter;
			for (uint32_t i = 0; i < parameterList.size(); i++)
			{
				parameter.first = parameterList[i]->getControlID();
				parameter.second = parameterList[i];
				pluginParameterMap.insert(parameter);
			}
		}
/*===================================================================================*/
/*
	[Function] Use this to update your FX objects on a per sample basis [Heavy CPU Usage]
*/
		void updateParametersBySample()
		{
			
		}
/*===================================================================================*/
/*
	[Function] Use this to update your FX objects on a per buffer basis [Low CPU Usage]
*/
		void updateParametersByBuffer()
		{
			
		}

/*===================================================================================*/
/*
	[Function] Use this to Reset your FX objects when you switch songs or load the plugin
*/
		void reset(const uint32_t& sampleRate)
		{
			/*
				Reset FX objects
			*/
			this->sampleRate = sampleRate;
		}
/*===================================================================================*/
/*
	[Function] Implement your DSP Logic here
*/
		void run(Frame<sample>& frame)
		{	
			/*===================================================================================*/
			/*
				Making a copy of the input Frame (Dont Mess with it)
			*/
			/*===================================================================================*/

			sample leftChannel = frame.left;
			sample rightChannel = frame.right;
			/*===================================================================================*/
			/*===================================================================================*/
			/*
				Write DSP Algorithm here
			*/
			/*===================================================================================*/
			/*
				Start
			*/

			/*
				End
			*/
			/*===================================================================================*/
			/*===================================================================================*/
			/*
				Save your processed Samples back to the Frame (Dont Mess with it)
			*/
			/*===================================================================================*/
			frame.left = leftChannel;
			frame.right = rightChannel;
		}
	private:
/*
	Declare FX Objects here
*/

/*===================================================================================*/
/*
	[Function] Gets the Control from our nice dandy vector of pointers (DONT MESS WITH IT)
*/
		knob getParameter(const int& i)
		{
			auto x = pluginParameterMap.find(i);
			auto type = x->second->getBoundVariableType();
			switch (type)
			{
			case boundVariableType::kFloat:return static_cast<float>(x->second->getControlValue());
				break;
			case boundVariableType::kDouble:return static_cast<double>(x->second->getControlValue());
				break;
			case boundVariableType::kInt:return static_cast<int>(x->second->getControlValue());
				break;
			case boundVariableType::kUInt:return static_cast<uint32_t>(x->second->getControlValue());
				break;
			default: return 0;
			}
		}
/*===================================================================================*/
/*
	[Function] Use this function to Update your meters
*/
		void setMeterValue(const double& newValue,const int& i)
		{
			auto x = pluginParameterMap.find(i);
			x->second->setMeterValue(newValue, x->second->getBoundVariableType());
		}
		std::unordered_map<uint32_t, PluginParameter*> pluginParameterMap;
		uint32_t sampleRate;
		
	};

	


}
#endif