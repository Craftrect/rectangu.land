#include "Precompiled.h"
#include "SamplerStateD3D11.h"
#include "GxDeviceD3D11.h"

namespace Graphics
{
	SamplerStateD3D11::SamplerStateD3D11(Device::GxDeviceD3D11WeakPtr device) : SamplerState(ApiType::D3D11), mDevice(device) {
		if (device.lock()->getType() != ApiType::D3D11) {
			throw std::invalid_argument("Invalid device type, must be d3d11");
		}

		createDefault();
	}

	void SamplerStateD3D11::createDefault() {
		auto device = mDevice.lock();
		auto dev11 = std::static_pointer_cast<Device::GxDeviceD3D11>(device);

		memset(&mDescription, 0, sizeof(mDescription));

		mDescription.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
		mDescription.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
		mDescription.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;

		mDescription.BorderColor[0] = mDescription.BorderColor[1] = mDescription.BorderColor[2] = mDescription.BorderColor[3] = 0.0f;

		mDescription.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
		mDescription.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
		mDescription.MaxAnisotropy = 1;
		mDescription.MaxLOD = D3D11_FLOAT32_MAX;
		mDescription.MinLOD = 0;
		mDescription.MipLODBias = 0.0f;

		auto res = dev11->getDevice()->CreateSamplerState(&mDescription, mSamplerState);
		if (FAILED(res)) {
			throw GraphicsException("Failed to create sampler state");
		}
	}

	void SamplerStateD3D11::setAddressU(AddressMode mode) {
		switch (mode) {
		case AddressMode::Wrap:
			mDescription.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
			break;

		case AddressMode::Border:
			mDescription.AddressU = D3D11_TEXTURE_ADDRESS_BORDER;
			break;

		case AddressMode::Clamp:
			mDescription.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
			break;

		case AddressMode::Mirror:
			mDescription.AddressU = D3D11_TEXTURE_ADDRESS_MIRROR;
			break;

		default:
			throw std::invalid_argument("Invalid address mode");
		}

		auto device = mDevice.lock();
		auto dev11 = std::static_pointer_cast<Device::GxDeviceD3D11>(device);

		ID3D11SamplerState* state;
		auto res = dev11->getDevice()->CreateSamplerState(&mDescription, &state);
		if (FAILED(res)) {
			throw GraphicsException("Failed to create sampler state");
		}

		mSamplerState = state;
	}

	void SamplerStateD3D11::setAddressV(AddressMode mode) {
		switch (mode) {
		case AddressMode::Wrap:
			mDescription.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
			break;

		case AddressMode::Border:
			mDescription.AddressV = D3D11_TEXTURE_ADDRESS_BORDER;
			break;

		case AddressMode::Clamp:
			mDescription.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
			break;

		case AddressMode::Mirror:
			mDescription.AddressV = D3D11_TEXTURE_ADDRESS_MIRROR;
			break;

		default:
			throw std::invalid_argument("Invalid address mode");
		}

		auto device = mDevice.lock();
		auto dev11 = std::static_pointer_cast<Device::GxDeviceD3D11>(device);

		ID3D11SamplerState* state;
		auto res = dev11->getDevice()->CreateSamplerState(&mDescription, &state);
		if (FAILED(res)) {
			throw GraphicsException("Failed to create sampler state");
		}

		mSamplerState = state;
	}

	void SamplerStateD3D11::setAddressW(AddressMode mode) {
		switch (mode) {
		case AddressMode::Wrap:
			mDescription.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
			break;

		case AddressMode::Border:
			mDescription.AddressW = D3D11_TEXTURE_ADDRESS_BORDER;
			break;

		case AddressMode::Clamp:
			mDescription.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
			break;

		case AddressMode::Mirror:
			mDescription.AddressW = D3D11_TEXTURE_ADDRESS_MIRROR;
			break;

		default:
			throw std::invalid_argument("Invalid address mode");
		}

		auto device = mDevice.lock();
		auto dev11 = std::static_pointer_cast<Device::GxDeviceD3D11>(device);

		ID3D11SamplerState* state;
		auto res = dev11->getDevice()->CreateSamplerState(&mDescription, &state);
		if (FAILED(res)) {
			throw GraphicsException("Failed to create sampler state");
		}

		mSamplerState = state;
	}

	void SamplerStateD3D11::setFilter(FilterMode mode, float anisotropy) {
		switch (mode) {
		case FilterMode::Bilinear:
			mDescription.Filter = D3D11_FILTER_MIN_MAG_LINEAR_MIP_POINT;
			break;

		case FilterMode::Trilinear:
			mDescription.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
			break;

		case FilterMode::Anisotropic:
			mDescription.Filter = D3D11_FILTER_ANISOTROPIC;
			mDescription.MaxAnisotropy = (uint32) anisotropy;
			break;

		default:
			throw std::invalid_argument("Invalid texture filter");
		}

		auto device = mDevice.lock();
		auto dev11 = std::static_pointer_cast<Device::GxDeviceD3D11>(device);

		ID3D11SamplerState* state;
		auto res = dev11->getDevice()->CreateSamplerState(&mDescription, &state);
		if (FAILED(res)) {
			throw GraphicsException("Failed to create sampler state");
		}

		mSamplerState = state;
	}
}