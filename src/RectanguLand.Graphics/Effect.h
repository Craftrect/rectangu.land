#pragma once

#include "ShaderCode.h"
#include "GxDevice.h"
#include "ShaderObject.h"
#include "Objects/Buffer.h"
#include "Objects/Texture.h"
#include "SamplerState.h"
#include "RenderTarget.h"

namespace Graphics
{
	namespace Device
	{
		WEAK_FORWARD(GxDevice);
	}

	class Effect
	{
	public:
		class EffectParamManager
		{
		private:
			std::map<ShaderVariables, BufferPtr> mBuffers;
			std::map<ShaderVariables, TexturePtr> mTextures;
			std::map<ShaderVariables, RenderTargetPtr> mRTTextures;
			std::map<uint32, TexturePtr> mTexturesI;
			std::map<uint32, RenderTargetPtr> mRTTexturesI;
			std::map<uint32, BufferPtr> mBuffersI;
			std::map<uint32, SamplerStatePtr> mSamplers;

		public:
			void setBuffer(uint32 reg, BufferPtr buffer);
			void setBuffer(ShaderVariables var, BufferPtr buffer);
			void setTexture(uint32 reg, TexturePtr texture);
			void setTexture(ShaderVariables var, TexturePtr texture);
			void setTexture(uint32 reg, RenderTargetPtr rt);
			void setTexture(ShaderVariables var, RenderTargetPtr rt);
			void setSampler(uint32 index, SamplerStatePtr sampler);

			void apply(Device::GxDevicePtr device, ShaderObjectPtr object);
			void remove(Device::GxDevicePtr device, ShaderObjectPtr object);
		};

		SHARED_TYPE(EffectParamManager);

	protected:
		Device::GxDeviceWeakPtr mDevice;
		std::map<ShaderCodeType, ShaderObjectPtr> mShaders;
		EffectParamManagerPtr mManagers[(uint32)ShaderCodeType::MAX];

	public:
		Effect(Device::GxDeviceWeakPtr device);
		void setShader(ShaderCodeType type, const String& resName);

		EffectParamManagerPtr getParamManager(ShaderCodeType type) const;

		ShaderObjectPtr getShader(ShaderCodeType type) const;
		
		void begin();
		void end();
	};

	SHARED_TYPE(Effect);
}