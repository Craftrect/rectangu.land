#pragma once

#include "SamplerState.h"
#include "Native.h"

namespace Graphics
{
	namespace Device
	{
		WEAK_FORWARD(GxDeviceD3D11);
	}

	class SamplerStateD3D11 : public SamplerState
	{
		Device::GxDeviceD3D11WeakPtr mDevice;
		Utils::ComPtr<ID3D11SamplerState> mSamplerState;

		D3D11_SAMPLER_DESC mDescription;

		void createDefault();
	public:
		SamplerStateD3D11(Device::GxDeviceD3D11WeakPtr device);

		void setAddressU(AddressMode mode);
		void setAddressV(AddressMode mode);
		void setAddressW(AddressMode mode);

		void setFilter(FilterMode filter, float anisotropy);

		Utils::ComPtr<ID3D11SamplerState> getStatePtr() const { return mSamplerState; }
	};

	SHARED_TYPE(SamplerStateD3D11);
}