#ifndef AuxPort_AudioEffect_H
#define AuxPort_AudioEffect_H
#pragma once
/*
*			WrappedFX
			"I tried to make it easier" - inpinseptipin

			BSD 3-Clause License

			Copyright (c) 2022, Satyarth Arora, Graduate Teaching Assistant, University of Miami
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
		[Class] A Custom Audio Delay class with Linear Interpolation
	*/
	class Delay
	{
	public:
		Delay() = default;
		~Delay() = default;
		Delay(const Delay& delay) = default;
		void setMaxDelay(const uint32_t& sampleRate, const uint32_t& maxDelay)
		{
			buffer.resize(sampleRate * maxDelay);
			this->sampleRate = sampleRate;
			std::fill(buffer.begin(), buffer.end(), 0);
			feedbackSample = 0;
			writeIndex = 0;
			delayHead = 0;
			delaySmooth = 0;
			delay = 0;
			dry = 0;
			wet = 0;
			feedback = 0;
			delayInSamples = 0;

		}
		void setParameters(const float& delay, const float& drywet, const float& feedback)
		{
			if (this->dry != drywet)
				this->dry = drywet;
			if (this->wet != 1 - drywet)
				this->wet = 1 - drywet;
			if (this->feedback != feedback / 100)
				this->feedback = feedback / 100;
			if (this->delay != delay)
			{
				this->delay = this->delaySmooth;
				this->delaySmooth = delay;
			}
		}
		float processAudioSample(const float& sample)
		{
			this->delayInSamples = sampleRate * this->delaySmooth;
			this->delaySmooth = this->delaySmooth - 0.0001 * (this->delaySmooth - this->delay);
			buffer[writeIndex++] = sample + feedbackSample;
			writeIndex %= buffer.size();
			delayHead = writeIndex - delayInSamples;
			if (delayHead < 0)
				delayHead += buffer.size();
			float polated = doPolation();
			feedbackSample = polated * this->feedback;
			return this->dry * sample + this->wet * polated;
		}
	private:
		float doPolation()
		{
			int32_t readIndex_0 = int32_t(delayHead);
			int32_t readIndex_1 = readIndex_0 + 1;
			if (readIndex_0 >= buffer.size())
				readIndex_0 -= buffer.size();
			if (readIndex_1 >= buffer.size())
				readIndex_1 -= buffer.size();
			return (buffer[readIndex_0] + buffer[readIndex_1]) / 2;
		}
		std::vector<float> buffer;
		int32_t writeIndex;
		float delayHead;
		float delayInSamples;
		uint32_t sampleRate;
		float dry;
		float wet;
		float feedback;
		float feedbackSample;
		float delay;
		float delaySmooth = { 0 };
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
		void push(void* parameterAddress, const boundVariableType& dataType,int controlNumber)
		{
			_controls.push_back({parameterAddress,dataType,controlNumber});
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
/*===================================================================================*/
/*
	[Function] Gets the Control from our nice dandy vector of pointers (DONT MESS WITH IT)
*/
		knob getParameter(const int& i)
		{
			Parameters* para;
			for (size_t j = 0; j < _controls.size(); j++)
			{
				para = &_controls[j];
				if (para->controlNumber == i)
				{
					if (para->_dataType == boundVariableType::kFloat)
						return *static_cast<float*>(para->_parameterAddress);
					if (para->_dataType == boundVariableType::kDouble)
						return *static_cast<double*>(para->_parameterAddress);
					if (para->_dataType == boundVariableType::kInt)
						return *static_cast<int*>(para->_parameterAddress);
					if (para->_dataType == boundVariableType::kUInt)
						return *static_cast<uint32_t*>(para->_parameterAddress);
				}
			}
		}
/*===================================================================================*/
/*
	[Function] Use this function to Update your meters
*/
		void setParameter(const double& newValue,const int& i)
		{
			Parameters* para;
			for (size_t j = 0; j < _controls.size(); j++)
			{
				para = &_controls[j];
				if (para->controlNumber == i)
				{
					if (para->_dataType == boundVariableType::kFloat)
						*static_cast<float*>(para->_parameterAddress) = static_cast<float>(newValue);
					else if (para->_dataType == boundVariableType::kDouble)
						*static_cast<double*>(para->_parameterAddress) = newValue;
					else if (para->_dataType == boundVariableType::kInt)
						*static_cast<int*>(para->_parameterAddress) = static_cast<int>(newValue);
					else if (para->_dataType == boundVariableType::kUInt)
						*static_cast<uint32_t*>(para->_parameterAddress) = static_cast<uint32_t>(newValue);
					break;
				}

			}		
		}

		struct Parameters
		{
			void* _parameterAddress;
			boundVariableType _dataType;
			int controlNumber;
		};
		std::vector<Parameters> _controls;
		uint32_t sampleRate;
		
	};
}
#endif
