#pragma once

#include "Texture.h"
#include "../Native.h"
#include "../GxDeviceD3D11.h"

namespace Graphics
{
	SHARED_FORWARD(TextureD3D11);

	class TextureD3D11 : public Texture
	{
		static Utils::ComPtr<ID3D11Texture2D> gDefResource;
		static Utils::ComPtr<ID3D11ShaderResourceView> gDefView;

		Utils::ComPtr<ID3D11Texture2D> mTexture;
		Utils::ComPtr<ID3D11ShaderResourceView> mView;
		Device::GxDeviceD3D11WeakPtr mDevice;
		bool mIsDefault;
		D3D11_TEXTURE2D_DESC mDesc;
		uint32 mPitch;

		void loadDefaultTexture(Device::GxDeviceD3D11Ptr device);

	public:
		TextureD3D11(Device::GxDeviceD3D11Ptr device);

		void setRealData(Utils::ComPtr<ID3D11Texture2D> texture, bool cubemap = false, uint32 layers = 1);
		void generateMipmaps();

		Utils::ComPtr<ID3D11ShaderResourceView> getShaderView() const { return mView; }

		void updateData(const void* memory, uint32 size);
	};
}