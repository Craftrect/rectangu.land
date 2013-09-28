#pragma once

#include <RectanguLand.Graphics/GxDeviceD3D11.h>
#include <RectanguLand.Graphics/Objects/TextureD3D11.h>
#include "TextureLoader.h"

namespace Content
{
	class TextureLoaderD3D11 : public TextureLoader
	{
		Graphics::Device::GxDeviceD3D11Ptr mDevice;
		void textureLoadComplete(Graphics::TextureD3D11Ptr texture, ImageLoaderPtr data);
		void loadTextureCubemap(Graphics::TextureD3D11Ptr texture, ImageLoaderPtr data);
	public:
		TextureLoaderD3D11(Graphics::Device::GxDeviceD3D11Ptr device, uint32 threadCount = 4);

		Graphics::TexturePtr loadTexture(std::shared_ptr<std::istream> resource);

		void shutdownThreads();
	};
}