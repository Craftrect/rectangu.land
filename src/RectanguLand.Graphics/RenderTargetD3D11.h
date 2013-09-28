#pragma once

#include "RenderTarget.h"
#include "Native.h"

namespace Graphics
{
	namespace Device
	{
		WEAK_FORWARD(GxDevice);
	}

	class RenderTargetD3D11 : public RenderTarget
	{
		Utils::ComPtr<ID3D11Texture2D> mResource;
		Utils::ComPtr<ID3D11RenderTargetView> mRTView;
		Utils::ComPtr<ID3D11ShaderResourceView> mSRView;

		Device::GxDeviceWeakPtr mDevice;

	public:
		RenderTargetD3D11(Device::GxDeviceWeakPtr device, RenderTargetType type, uint32 width, uint32 height);

		Utils::ComPtr<ID3D11RenderTargetView> getRTView() const { return mRTView; }
		Utils::ComPtr<ID3D11ShaderResourceView> getSRView() const { return mSRView; }

		void clear(const Utils::Vector4& color);
	};

	SHARED_TYPE(RenderTargetD3D11);
}