#pragma once

#include "GxDeviceD3D11.h"
#include "ShaderCode.h"

namespace Graphics
{
	class ShaderCodeD3D11 : public ShaderCode
	{
		Device::GxDeviceD3D11Ptr mDevice;

	public:
		ShaderCodeD3D11(Device::GxDeviceD3D11Ptr device);

		void loadFromBuffer(std::shared_ptr<std::istream> buff);

		void compilePixelShader(ID3D11PixelShader** pshader);
		void compileVertexShader(ID3D11VertexShader** vshader);
		void compileGeometryShader(ID3D11GeometryShader** gshader);
		void compileDomainShader(ID3D11DomainShader** dshader);
		void compileHullShader(ID3D11HullShader** hshader);
	};

	SHARED_TYPE(ShaderCodeD3D11);
}