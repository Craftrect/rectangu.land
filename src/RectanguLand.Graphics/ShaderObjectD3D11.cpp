#include "Precompiled.h"
#include "ShaderObjectD3D11.h"
#include "Objects/ShaderCodeD3D11.h"
#include "Objects/Buffer.h"
#include "Objects/BufferD3D11.h"
#include "Objects/TextureD3D11.h"
#include "RenderTargetD3D11.h"

namespace Graphics
{
	void ShaderObjectD3D11::load(ShaderCodePtr code) {
		if (mIsLoaded == true) {
			throw std::bad_function_call("Shader already loaded.");
		}

		ShaderCodeD3D11Ptr code11 = std::dynamic_pointer_cast<ShaderCodeD3D11>(code);
		if (code11 == nullptr) {
			throw std::invalid_argument("Code is not for d3d11");
		}

		mCode = code;

		mIsLoaded = true;
		mType = code->getCodeType();
		switch (mType) {
		case ShaderCodeType::Pixel:
			{
				code11->compilePixelShader(&mPixelShader);
			}
			break;

		case ShaderCodeType::Vertex:
			{
				code11->compileVertexShader(&mVertexShader);
				break;
			}

		case ShaderCodeType::Domain:
			{
				code11->compileDomainShader(&mDomainShader);
				break;
			}

		case ShaderCodeType::Hull:
			{
				code11->compileHullShader(&mHullShader);
				break;
			}

		case ShaderCodeType::Geometry:
			{
				code11->compileGeometryShader(&mGeometryShader);
				break;
			}

		default:
			throw std::invalid_argument("Invalid code type");
		}
	}

	void ShaderObjectD3D11::apply(Device::GxDevicePtr device) {
		if (device->getType() != ApiType::D3D11) {
			throw std::invalid_argument("ShaderD3D11 has to work on a d3d11 device");
		}

		if (mIsLoaded == false) {
			throw std::bad_function_call("Called Shader::apply before shader was loaded");
		}

		Device::GxDeviceD3D11Ptr dev11 = std::static_pointer_cast<Device::GxDeviceD3D11>(device);
		switch (mType) {
		case ShaderCodeType::Pixel:
			{
				dev11->getImmContext()->PSSetShader(mPixelShader, nullptr, 0);
				break;
			}

		case ShaderCodeType::Vertex:
			{
				dev11->getImmContext()->VSSetShader(mVertexShader, nullptr, 0);
				break;
			}

		case ShaderCodeType::Domain:
			{
				dev11->getImmContext()->DSSetShader(mDomainShader, nullptr, 0);
				break;
			}

		case ShaderCodeType::Geometry:
			{
				dev11->getImmContext()->GSSetShader(mGeometryShader, nullptr, 0);
				break;
			}

		case ShaderCodeType::Hull:
			{
				dev11->getImmContext()->HSSetShader(mHullShader, nullptr, 0);
				break;
			}
		}
	}

	void ShaderObjectD3D11::remove(Device::GxDevicePtr device) {
		if (device->getType() != ApiType::D3D11) {
			throw std::invalid_argument("ShaderD3D11 has to work on a d3d11 device");
		}

		if (mIsLoaded == false) {
			throw std::bad_function_call("Called Shader::remove before shader was loaded");
		}

		Device::GxDeviceD3D11Ptr dev11 = std::static_pointer_cast<Device::GxDeviceD3D11>(device);
		switch (mType) {
		case ShaderCodeType::Pixel:
			{
				dev11->getImmContext()->PSSetShader(nullptr, nullptr, 0);
				break;
			}

		case ShaderCodeType::Vertex:
			{
				dev11->getImmContext()->VSSetShader(nullptr, nullptr, 0);
				break;
			}

		case ShaderCodeType::Domain:
			{
				dev11->getImmContext()->DSSetShader(nullptr, nullptr, 0);
				break;
			}

		case ShaderCodeType::Geometry:
			{
				dev11->getImmContext()->GSSetShader(nullptr, nullptr, 0);
				break;
			}

		case ShaderCodeType::Hull:
			{
				dev11->getImmContext()->HSSetShader(nullptr, nullptr, 0);
				break;
			}
		}
	}

	void ShaderObjectD3D11::setBuffer(Device::GxDevicePtr device, ShaderVariables var, BufferPtr buffer) {
		if (buffer->getApiType() != ApiType::D3D11) {
			throw std::invalid_argument("buffer is not d3d11");
		}

		if (device->getType() != ApiType::D3D11) {
			throw std::invalid_argument("ShaderD3D11 has to work on a d3d11 device");
		}

		if (mIsLoaded == false) {
			throw std::bad_function_call("Called Shader::remove before shader was loaded");
		}

		XBSConstTableEntry centry;
		bool hasVariable = mCode->getConstEntry(var, centry);
		if (hasVariable == false || centry.regType != 'c') {
			return;
		}

		Device::GxDeviceD3D11Ptr dev11 = std::static_pointer_cast<Device::GxDeviceD3D11>(device);

		BufferD3D11* buff = static_cast<BufferD3D11*>(buffer.get());
		auto buffPtr = buff->getBuffer();

		switch (mType) {
		case ShaderCodeType::Pixel:
			{
				dev11->getImmContext()->PSSetConstantBuffers(centry.regNumber, 1, &buffPtr);
				break;
			}

		case ShaderCodeType::Vertex:
			{
				dev11->getImmContext()->VSSetConstantBuffers(centry.regNumber, 1, &buffPtr);
				break;
			}

		case ShaderCodeType::Geometry:
			{
				dev11->getImmContext()->GSSetConstantBuffers(centry.regNumber, 1, &buffPtr);
				break;
			}

		case ShaderCodeType::Domain:
			{
				dev11->getImmContext()->DSSetConstantBuffers(centry.regNumber, 1, &buffPtr);
				break;
			}

		case ShaderCodeType::Hull:
			{
				dev11->getImmContext()->HSSetConstantBuffers(centry.regNumber, 1, &buffPtr);
				break;
			}
		}
	}

	void ShaderObjectD3D11::setBuffer(Device::GxDevicePtr device, uint32 reg, BufferPtr buffer) {
		if (buffer->getApiType() != ApiType::D3D11) {
			throw std::invalid_argument("buffer is not d3d11");
		}

		if (device->getType() != ApiType::D3D11) {
			throw std::invalid_argument("ShaderD3D11 has to work on a d3d11 device");
		}

		if (mIsLoaded == false) {
			throw std::bad_function_call("Called Shader::remove before shader was loaded");
		}

		Device::GxDeviceD3D11Ptr dev11 = std::static_pointer_cast<Device::GxDeviceD3D11>(device);

		BufferD3D11* buff = static_cast<BufferD3D11*>(buffer.get());
		auto buffPtr = buff->getBuffer();

		switch (mType) {
		case ShaderCodeType::Pixel:
			{
				dev11->getImmContext()->PSSetConstantBuffers(reg, 1, &buffPtr);
				break;
			}

		case ShaderCodeType::Vertex:
			{
				dev11->getImmContext()->VSSetConstantBuffers(reg, 1, &buffPtr);
				break;
			}

		case ShaderCodeType::Geometry:
			{
				dev11->getImmContext()->GSSetConstantBuffers(reg, 1, &buffPtr);
				break;
			}

		case ShaderCodeType::Domain:
			{
				dev11->getImmContext()->DSSetConstantBuffers(reg, 1, &buffPtr);
				break;
			}

		case ShaderCodeType::Hull:
			{
				dev11->getImmContext()->HSSetConstantBuffers(reg, 1, &buffPtr);
				break;
			}
		}
	}

	void ShaderObjectD3D11::setSampler(Device::GxDevicePtr device, uint32 index, SamplerStatePtr sampler) {
		if (sampler->getApiType() != ApiType::D3D11) {
			throw std::invalid_argument("Sampler is not d3d11");
		}

		if (device->getType() != ApiType::D3D11) {
			throw std::invalid_argument("ShaderD3D11 has to work on a d3d11 device");
		}

		if (mIsLoaded == false) {
			throw std::bad_function_call("Used shader before shader was loaded");
		}

		Device::GxDeviceD3D11Ptr dev11 = std::static_pointer_cast<Device::GxDeviceD3D11>(device);
		auto samp11 = std::static_pointer_cast<SamplerStateD3D11>(sampler);

		switch (mType) {
		case ShaderCodeType::Pixel:
			{
				dev11->getImmContext()->PSSetSamplers(index, 1, samp11->getStatePtr());
				break;
			}

		case ShaderCodeType::Vertex:
			{
				dev11->getImmContext()->VSSetSamplers(index, 1, samp11->getStatePtr());
				break;
			}

		case ShaderCodeType::Geometry:
			{
				dev11->getImmContext()->GSSetSamplers(index, 1, samp11->getStatePtr());
				break;
			}

		case ShaderCodeType::Domain:
			{
				dev11->getImmContext()->DSSetSamplers(index, 1, samp11->getStatePtr());
				break;
			}

		case ShaderCodeType::Hull:
			{
				dev11->getImmContext()->HSSetSamplers(index, 1, samp11->getStatePtr());
				break;
			}
		}
	}

	void ShaderObjectD3D11::setTexture(Device::GxDevicePtr device, ShaderVariables var, TexturePtr tex) {
		if (tex->getApiType() != ApiType::D3D11) {
			throw std::invalid_argument("texture is not d3d11");
		}

		if (device->getType() != ApiType::D3D11) {
			throw std::invalid_argument("ShaderD3D11 has to work on a d3d11 device");
		}

		if (mIsLoaded == false) {
			throw std::bad_function_call("Called Shader::remove before shader was loaded");
		}

		XBSConstTableEntry centry;
		bool hasVariable = mCode->getConstEntry(var, centry);
		if (hasVariable == false || centry.regType != 'c') {
			return;
		}

		Device::GxDeviceD3D11Ptr dev11 = std::static_pointer_cast<Device::GxDeviceD3D11>(device);

		TextureD3D11Ptr tex11 = std::static_pointer_cast<TextureD3D11>(tex);

		switch (mType) {
		case ShaderCodeType::Pixel:
			{
				dev11->getImmContext()->PSSetShaderResources(centry.regNumber, 1, tex11->getShaderView());
				break;
			}

		case ShaderCodeType::Vertex:
			{
				dev11->getImmContext()->VSSetShaderResources(centry.regNumber, 1, tex11->getShaderView());
				break;
			}

		case ShaderCodeType::Geometry:
			{
				dev11->getImmContext()->GSSetShaderResources(centry.regNumber, 1, tex11->getShaderView());
				break;
			}

		case ShaderCodeType::Domain:
			{
				dev11->getImmContext()->DSSetShaderResources(centry.regNumber, 1, tex11->getShaderView());
				break;
			}

		case ShaderCodeType::Hull:
			{
				dev11->getImmContext()->HSSetShaderResources(centry.regNumber, 1, tex11->getShaderView());
				break;
			}
		}
	}

	void ShaderObjectD3D11::setTexture(Device::GxDevicePtr device, uint32 reg, TexturePtr tex) {
		if (tex->getApiType() != ApiType::D3D11) {
			throw std::invalid_argument("texture is not d3d11");
		}

		if (device->getType() != ApiType::D3D11) {
			throw std::invalid_argument("ShaderD3D11 has to work on a d3d11 device");
		}

		if (mIsLoaded == false) {
			throw std::bad_function_call("Called Shader::remove before shader was loaded");
		}

		Device::GxDeviceD3D11Ptr dev11 = std::static_pointer_cast<Device::GxDeviceD3D11>(device);

		TextureD3D11Ptr tex11 = std::static_pointer_cast<TextureD3D11>(tex);

		switch (mType) {
		case ShaderCodeType::Pixel:
			{
				dev11->getImmContext()->PSSetShaderResources(reg, 1, tex11->getShaderView());
				break;
			}

		case ShaderCodeType::Vertex:
			{
				dev11->getImmContext()->VSSetShaderResources(reg, 1, tex11->getShaderView());
				break;
			}

		case ShaderCodeType::Geometry:
			{
				dev11->getImmContext()->GSSetShaderResources(reg, 1, tex11->getShaderView());
				break;
			}

		case ShaderCodeType::Domain:
			{
				dev11->getImmContext()->DSSetShaderResources(reg, 1, tex11->getShaderView());
				break;
			}

		case ShaderCodeType::Hull:
			{
				dev11->getImmContext()->HSSetShaderResources(reg, 1, tex11->getShaderView());
				break;
			}
		}
	}

	void ShaderObjectD3D11::setRTTexture(Device::GxDevicePtr device, ShaderVariables var, RenderTargetPtr rt) {
		if (rt != nullptr && rt->getApiType() != ApiType::D3D11) {
			throw std::invalid_argument("rendertarget is not d3d11");
		}

		if (device->getType() != ApiType::D3D11) {
			throw std::invalid_argument("ShaderD3D11 has to work on a d3d11 device");
		}

		if (mIsLoaded == false) {
			throw std::bad_function_call("Called Shader::remove before shader was loaded");
		}

		XBSConstTableEntry centry;
		bool hasVariable = mCode->getConstEntry(var, centry);
		if (hasVariable == false || centry.regType != 'c') {
			return;
		}

		RenderTargetD3D11Ptr rt11 = nullptr;
		if (rt != nullptr) {
			rt11 = std::static_pointer_cast<RenderTargetD3D11>(rt);
		}

		Device::GxDeviceD3D11Ptr dev11 = std::static_pointer_cast<Device::GxDeviceD3D11>(device);

		switch (mType) {
		case ShaderCodeType::Pixel:
			{
				dev11->getImmContext()->PSSetShaderResources(centry.regNumber, 1, rt11 != nullptr ? rt11->getSRView() : nullptr);
				break;
			}

		case ShaderCodeType::Vertex:
			{
				dev11->getImmContext()->VSSetShaderResources(centry.regNumber, 1, rt11 != nullptr ? rt11->getSRView() : nullptr);
				break;
			}

		case ShaderCodeType::Geometry:
			{
				dev11->getImmContext()->GSSetShaderResources(centry.regNumber, 1, rt11 != nullptr ? rt11->getSRView() : nullptr);
				break;
			}

		case ShaderCodeType::Domain:
			{
				dev11->getImmContext()->DSSetShaderResources(centry.regNumber, 1, rt11 != nullptr ? rt11->getSRView() : nullptr);
				break;
			}

		case ShaderCodeType::Hull:
			{
				dev11->getImmContext()->HSSetShaderResources(centry.regNumber, 1, rt11 != nullptr ? rt11->getSRView() : nullptr);
				break;
			}
		}
	}

	void ShaderObjectD3D11::setRTTexture(Device::GxDevicePtr device, uint32 reg, RenderTargetPtr rt) {
		if (rt != nullptr && rt->getApiType() != ApiType::D3D11) {
			throw std::invalid_argument("rendertarget is not d3d11");
		}

		if (device->getType() != ApiType::D3D11) {
			throw std::invalid_argument("ShaderD3D11 has to work on a d3d11 device");
		}

		if (mIsLoaded == false) {
			throw std::bad_function_call("Called Shader::remove before shader was loaded");
		}

		RenderTargetD3D11Ptr rt11 = nullptr;
		if (rt != nullptr) {
			rt11 = std::static_pointer_cast<RenderTargetD3D11>(rt);
		}

		Device::GxDeviceD3D11Ptr dev11 = std::static_pointer_cast<Device::GxDeviceD3D11>(device);

		switch (mType) {
		case ShaderCodeType::Pixel:
			{
				dev11->getImmContext()->PSSetShaderResources(reg, 1, rt11 != nullptr ? rt11->getSRView() : nullptr);
				break;
			}

		case ShaderCodeType::Vertex:
			{
				dev11->getImmContext()->VSSetShaderResources(reg, 1, rt11 != nullptr ? rt11->getSRView() : nullptr);
				break;
			}

		case ShaderCodeType::Geometry:
			{
				dev11->getImmContext()->GSSetShaderResources(reg, 1, rt11 != nullptr ? rt11->getSRView() : nullptr);
				break;
			}

		case ShaderCodeType::Domain:
			{
				dev11->getImmContext()->DSSetShaderResources(reg, 1, rt11 != nullptr ? rt11->getSRView() : nullptr);
				break;
			}

		case ShaderCodeType::Hull:
			{
				dev11->getImmContext()->HSSetShaderResources(reg, 1, rt11 != nullptr ? rt11->getSRView() : nullptr);
				break;
			}
		}
	}
}