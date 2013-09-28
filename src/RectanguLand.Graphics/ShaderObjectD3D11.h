#pragma once

#include "ShaderObject.h"

#include "Native.h"

#include "GxDevice.h"

namespace Graphics
{
	class ShaderObjectD3D11 : public ShaderObject
	{
		union {
			ID3D11PixelShader* mPixelShader;
			ID3D11VertexShader* mVertexShader;
			ID3D11DomainShader* mDomainShader;
			ID3D11GeometryShader* mGeometryShader;
			ID3D11HullShader* mHullShader;
		};

	public:
		void load(ShaderCodePtr code);
		void apply(Device::GxDevicePtr device);
		void remove(Device::GxDevicePtr device);
		void setBuffer(Device::GxDevicePtr device, ShaderVariables var, BufferPtr buffer);
		void setBuffer(Device::GxDevicePtr device, uint32 reg, BufferPtr buffer);
		void setTexture(Device::GxDevicePtr device, ShaderVariables var, TexturePtr tex);
		void setTexture(Device::GxDevicePtr device, uint32 reg, TexturePtr texture);
		void setSampler(Device::GxDevicePtr device, uint32 index, SamplerStatePtr sampler);
		void setRTTexture(Device::GxDevicePtr device, ShaderVariables var, RenderTargetPtr rendertarget);
		void setRTTexture(Device::GxDevicePtr device, uint32 reg, RenderTargetPtr rendertarget);
	};
}