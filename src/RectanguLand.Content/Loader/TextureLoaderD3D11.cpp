#include "Precompiled.h"
#include "TextureLoaderD3D11.h"

using namespace Graphics;

namespace Content
{
	TextureLoaderD3D11::TextureLoaderD3D11(Device::GxDeviceD3D11Ptr device, uint32 threads) : TextureLoader(threads) {
		mDevice = device;

	}

	Graphics::TexturePtr TextureLoaderD3D11::loadTexture(std::shared_ptr<std::istream> texture) {
		Graphics::TextureD3D11Ptr ret = std::make_shared<Graphics::TextureD3D11>(mDevice);
		TexturePayloadData pld(texture, std::bind(&TextureLoaderD3D11::textureLoadComplete, this, ret, std::placeholders::_1));

		postWorkItem(pld);

		return ret;
	}

	void TextureLoaderD3D11::loadTextureCubemap(Graphics::TextureD3D11Ptr tex, ImageLoaderPtr loader) {
		std::vector<D3D11_SUBRESOURCE_DATA> initialData(6 * loader->getInfo().getLayerCount());

		for (uint32 i = 0; i < 6; ++i) {
			auto& curFace = loader->getInfo().getCubeFace(i);

			for (uint32 j = 0; j < loader->getInfo().getLayerCount(); ++j) {
				auto& layer = curFace[j];

				uint32 index = i * loader->getInfo().getLayerCount() + j;
				initialData[index].pSysMem = layer.data();
				initialData[index].SysMemPitch = std::max(1u, loader->getInfo().getWidth() >> j) * 4;
				initialData[index].SysMemSlicePitch = 0;
			}
		}

		D3D11_TEXTURE2D_DESC desc;
		desc.ArraySize = 6;
		desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
		desc.CPUAccessFlags = 0;
		desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		desc.Height = loader->getInfo().getHeight();
		desc.Width = loader->getInfo().getWidth();
		desc.MipLevels = loader->getInfo().getLayerCount();
		desc.MiscFlags = D3D11_RESOURCE_MISC_TEXTURECUBE;
		desc.SampleDesc.Count = 1;
		desc.SampleDesc.Quality = 0;
		desc.Usage = D3D11_USAGE_DEFAULT;

		Utils::Dispatcher::main()->beginInvoke(
			[this, tex, desc, loader, initialData]() {
				Utils::ComPtr<ID3D11Texture2D> texPtr;
				auto result = mDevice->getDevice()->CreateTexture2D(&desc, initialData.data(), texPtr);
				// if loading the texture fails dont bother, leave it at default texture
				if (SUCCEEDED(result)) {
					tex->setRealData(texPtr, true, desc.MipLevels);
				}

		}, Utils::DispatcherPriority::Low);
	}

	void TextureLoaderD3D11::textureLoadComplete(Graphics::TextureD3D11Ptr tex, ImageLoaderPtr loader) {
		// loader == nullptr -> image invalid -> keep default texture
		if (loader == nullptr) {
			return;
		}

		if (loader->getInfo().isCubemap()) {
			return loadTextureCubemap(tex, loader);
		}

		// the layer information can be prepared off-thread, its not bound to the
		// graphics API
		std::vector<D3D11_SUBRESOURCE_DATA> layerData(loader->getInfo().getLayerCount());
		for (uint32 i = 0; i < layerData.size(); ++i) {
			auto& layer = loader->getInfo().getLayer(i);

			layerData[i].pSysMem = layer.data();
			layerData[i].SysMemPitch = std::max(1u, loader->getInfo().getWidth() >> i) * 4;
			layerData[i].SysMemSlicePitch = 0;
		}

		D3D11_TEXTURE2D_DESC desc;
		desc.ArraySize = 1;
		desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
		desc.CPUAccessFlags = 0;
		desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		desc.Height = loader->getInfo().getHeight();
		desc.Width = loader->getInfo().getWidth();
		desc.MipLevels = loader->getInfo().getLayerCount();
		desc.MiscFlags = 0;
		desc.SampleDesc.Count = 1;
		desc.SampleDesc.Quality = 0;
		desc.Usage = D3D11_USAGE_DEFAULT;

		Utils::Dispatcher::main()->beginInvoke(
			[this, tex, desc, loader, layerData]() {
				Utils::ComPtr<ID3D11Texture2D> texPtr;
				auto result = mDevice->getDevice()->CreateTexture2D(&desc, layerData.data(), texPtr);
				// if loading the texture fails dont bother, leave it at default texture
				if (SUCCEEDED(result)) {
					tex->setRealData(texPtr, false, desc.MipLevels);
				}

		}, Utils::DispatcherPriority::Low);
	}

	void TextureLoaderD3D11::shutdownThreads() {
		TextureLoader::shutdownThreads();

		mDevice = nullptr;
	}
}