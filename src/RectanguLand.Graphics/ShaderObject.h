#pragma once

#include "ShaderCode.h"

namespace Graphics
{
	namespace Device
	{
		SHARED_FORWARD(GxDevice);
	}

	SHARED_FORWARD(Buffer);
	SHARED_FORWARD(Texture);
	SHARED_FORWARD(SamplerState);
	SHARED_FORWARD(RenderTarget);

	class ShaderObject
	{
	protected:
		ShaderCodeType mType;
		ShaderCodePtr mCode;
		bool mIsLoaded;

	public:
		ShaderObject();

		virtual void load(ShaderCodePtr code) = 0;
		virtual void apply(Device::GxDevicePtr device) = 0;
		virtual void remove(Device::GxDevicePtr device) = 0;
		virtual void setBuffer(Device::GxDevicePtr device, ShaderVariables var, BufferPtr buffer) = 0;
		virtual void setBuffer(Device::GxDevicePtr device, uint32 reg, BufferPtr buffer) = 0;
		virtual void setTexture(Device::GxDevicePtr device, ShaderVariables var, TexturePtr texture) = 0;
		virtual void setTexture(Device::GxDevicePtr device, uint32 reg, TexturePtr texture) = 0;
		virtual void setSampler(Device::GxDevicePtr device, uint32 index, SamplerStatePtr sampler) = 0;
		virtual void setRTTexture(Device::GxDevicePtr device, ShaderVariables var, RenderTargetPtr rendertarget) = 0;
		virtual void setRTTexture(Device::GxDevicePtr device, uint32 reg, RenderTargetPtr rendertarget) = 0;

		ShaderCodePtr getShaderCode() const { return mCode; }
	};

	SHARED_TYPE(ShaderObject);
}