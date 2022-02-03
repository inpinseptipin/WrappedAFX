#ifndef AuxPort_AudioEffect_H
#define AuxPort_AudioEffect_H
#pragma once
/*
*			AuxPort Wrapper over Rackfx
			"I tried to make it easier" - inpinseptipin

			BSD 3-Clause License

			Copyright (c) 2022, Satyarth Arora, Teaching Assistant, University of Miami
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
	template<class bufferType, class effectType>
	class Effect
	{
	public:
/*===================================================================================*/
/*
	[Constructor] Safely Allocates the memory
*/
		Effect<bufferType, effectType>() = default;
/*===================================================================================*/
/*
	[Destructor] Safely Deallocates the memory
*/
		~Effect<bufferType, effectType>() = default;
/*===================================================================================*/
/*
	[Copy Constructor] Safely Copies memory from one Volume Object to another
*/
		Effect<bufferType, effectType>(const Effect<bufferType, effectType>& kernel) = default;
/*===================================================================================*/
/*
	[Function] Set your Control Addresses (DONT MESS WITH IT)
*/
		void push(void* parameterAddress, const boundVariableType& dataType)
		{
			_controls.push_back({parameterAddress,dataType});
		}
/*===================================================================================*/
/*
	[Function] Implement your Frame DSP Logic here
*/
		bufferType run(const bufferType& frameValue) 
		{
			/*===================================================================================*/
			/*
				Write DSP Algorithm here
			*/
			/*===================================================================================*/
			float preGain = getControl(controlID::PreGain);
			float dcOffset = getControl(controlID::DcOffset);
			float postGain = getControl(controlID::PostGain);
			int distortionType = getControl(controlID::distortionType);
			float reluctanceThreshold = getControl(controlID::Reluctance);
			float clip = getControl(controlID::clip);
			if (distortionType == 0)
			{
				return postGain * (preGain * frameValue);
			}
			if (distortionType == 1)
			{
				return postGain * ((preGain * frameValue) + dcOffset);

				
			}
			if (distortionType == 2)
			{
				if (abs(frameValue) < reluctanceThreshold)
					return 0;
				else
					return postGain * ((preGain * frameValue) + dcOffset);
			}
			if (distortionType == 3)
			{
				if ((preGain * frameValue) > clip)
					return postGain * clip;

				else if ((preGain * frameValue) < (-1 * clip))
					return postGain * -1 * clip;

				else
					return postGain * (preGain * frameValue);
			}
			if (distortionType == 4)
			{
				return (2 * kPi) * atan(preGain * frameValue);
			}

			if (distortionType == 5)
			{
				return postGain * atan(preGain * frameValue);
			}
			
			if (distortionType == 6)
			{
				return postGain * tanh(preGain * frameValue);
			}

			if (distortionType == 7)
			{
				return tanhWaveShaper(frameValue, preGain);
			}

			if (distortionType == 8)
			{
				if(frameValue < 0)
					return postGain * (preGain * -1*frameValue);
				else
					return postGain * (preGain * frameValue);
			}	

			if (distortionType == 9)
			{
				if (frameValue < 0)
					return postGain * (preGain * -1 * frameValue);
				else
					return (2 * kPi) * atan(preGain * frameValue);
			
			}
			
		}
	private:
/*===================================================================================*/
/*
	[Function] Gets the Control from our nice dandy vector of pointers (DONT MESS WITH IT)
*/
		effectType getControl(const int& i)
		{
			Parameters para = _controls[i];
			if (para._dataType == boundVariableType::kFloat)
				return *static_cast<float*>(para._parameterAddress);
			if (para._dataType == boundVariableType::kDouble)
				return *static_cast<double*>(para._parameterAddress);
			if (para._dataType == boundVariableType::kInt)
				return *static_cast<int*>(para._parameterAddress);
			if (para._dataType == boundVariableType::kUInt)
				return *static_cast<uint32_t*>(para._parameterAddress);
		}

		struct Parameters
		{
			void* _parameterAddress;
			boundVariableType _dataType;
		};

		std::vector<Parameters> _controls;

	};

}
#endif
