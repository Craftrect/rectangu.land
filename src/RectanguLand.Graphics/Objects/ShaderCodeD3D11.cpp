#include "Precompiled.h"
#include "ShaderCodeD3D11.h"

namespace po = boost::program_options;

namespace Graphics
{
	ShaderCodeD3D11::ShaderCodeD3D11(Device::GxDeviceD3D11Ptr device) {
		mDevice = device;
	}

	void ShaderCodeD3D11::loadFromBuffer(std::shared_ptr<std::istream> buffer) {
		parseGenericBuffer(*buffer);
	}

	void ShaderCodeD3D11::compileDomainShader(ID3D11DomainShader** dshader) {
		auto result = mDevice->getDevice()->CreateDomainShader(mShaderData.data(), mShaderData.size(), nullptr, dshader);
		if (FAILED(result)) {
			throw std::bad_function_call("Could not compile domain shader");
		}
	}

	void ShaderCodeD3D11::compileHullShader(ID3D11HullShader** hshader) {
		auto result = mDevice->getDevice()->CreateHullShader(mShaderData.data(), mShaderData.size(), nullptr, hshader);
		if (FAILED(result)) {
			throw std::bad_function_call("Could not compile hull shader");
		}
	}

	void ShaderCodeD3D11::compileGeometryShader(ID3D11GeometryShader** gshader) {
		auto result = mDevice->getDevice()->CreateGeometryShader(mShaderData.data(), mShaderData.size(), nullptr, gshader);
		if (FAILED(result)) {
			throw std::bad_function_call("Could not compile geometry shader");
		}
	}

	void ShaderCodeD3D11::compileVertexShader(ID3D11VertexShader** vshader) {
		auto result = mDevice->getDevice()->CreateVertexShader(mShaderData.data(), mShaderData.size(), nullptr, vshader);
		if (FAILED(result)) {
			throw std::bad_function_call("Could not compile vertex shader");
		}
	}

	void ShaderCodeD3D11::compilePixelShader(ID3D11PixelShader** pshader) {
		auto result = mDevice->getDevice()->CreatePixelShader(mShaderData.data(), mShaderData.size(), nullptr, pshader);
		if (FAILED(result)) {
			throw std::bad_function_call("Could not compile pixel shader");
		}
	}
}