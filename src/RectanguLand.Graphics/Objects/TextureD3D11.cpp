#include "Precompiled.h"
#include "TextureD3D11.h"

namespace Graphics
{
	Utils::ComPtr<ID3D11Texture2D> TextureD3D11::gDefResource;
	Utils::ComPtr<ID3D11ShaderResourceView> TextureD3D11::gDefView;

	TextureD3D11::TextureD3D11(Device::GxDeviceD3D11Ptr device) : Texture(ApiType::D3D11), mDevice(device) {
		if (gDefResource == nullptr) {
			loadDefaultTexture(device);
		}

		mIsDefault = true;

		mTexture = gDefResource;
		mView = gDefView;
	}

	void TextureD3D11::loadDefaultTexture(Device::GxDeviceD3D11Ptr device) {
		D3D11_TEXTURE2D_DESC desc = { 0 };
		desc.ArraySize = 1;
		desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
		desc.CPUAccessFlags = 0;
		desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		desc.Height = 1;
		desc.MipLevels = 1;
		desc.MiscFlags = 0;
		desc.SampleDesc.Count = 1;
		desc.SampleDesc.Quality = 0;
		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.Width = 1;

		D3D11_SUBRESOURCE_DATA initData = { 0 };
		uint32 color = 0xFF00FF00;
		initData.pSysMem = reinterpret_cast<void*>(&color);
		initData.SysMemPitch = 4;
		initData.SysMemSlicePitch = 0;

		auto result = device->getDevice()->CreateTexture2D(&desc, &initData, gDefResource);
		if (FAILED(result)) {
			throw GraphicsException("Unable to create default texture");
		}

		D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
		srvDesc.Texture2D.MipLevels = 1;
		srvDesc.Texture2D.MostDetailedMip = 0;
		srvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;

		result = device->getDevice()->CreateShaderResourceView(gDefResource, &srvDesc, gDefView);
		if (FAILED(result)) {
			throw GraphicsException("Unable to map view of texture");
		}
	}

	void TextureD3D11::setRealData(Utils::ComPtr<ID3D11Texture2D> texture, bool cubemap, uint32 layers) {
		mTexture = texture;

		texture->GetDesc(&mDesc);
		
		D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
		srvDesc.Texture2D.MipLevels = layers;
		srvDesc.Texture2D.MostDetailedMip = 0;
		srvDesc.Format = mDesc.Format;
		srvDesc.ViewDimension = cubemap == false ? D3D11_SRV_DIMENSION_TEXTURE2D : D3D11_SRV_DIMENSION_TEXTURECUBE;

		auto result = mDevice.lock()->getDevice()->CreateShaderResourceView(mTexture, &srvDesc, mView);
		if (FAILED(result)) {
			throw GraphicsException("Unable to map view of texture");
		}

		switch (mDesc.Format) {
		case DXGI_FORMAT_R8G8B8A8_UNORM:
			{
				mPitch = mDesc.Width * 4;
			}
			break;

		case DXGI_FORMAT_R32G32B32A32_FLOAT:
			{
				mPitch = mDesc.Width * 16;
			}
			break;

		default:
			throw GraphicsException(">> Implement this format!");
		}

		mIsDefault = false;
	}

	void TextureD3D11::generateMipmaps() {
		mDevice.lock()->getImmContext()->GenerateMips(mView);
	}

	void TextureD3D11::updateData(const void* memory, uint32 size) {
		if (mIsDefault == true) {
			throw GraphicsException("Unable to update textuer when still default texture");
		}

		mDevice.lock()->getImmContext()->UpdateSubresource(mTexture, 0, nullptr, memory, mPitch, 0);
	}
}