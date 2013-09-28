#pragma once

#include "Loader/TextureLoader.h"
#include "ResourceManager.h"
#include "EffectCollection.h"
#include <RectanguLand.Graphics/GxDevice.h>
#include <RectanguLand.Graphics/ShaderCode.h>
#include "TextureManager.h"

namespace Content
{
	class AssetNotFoundException : public std::exception
	{
		String mMessage;
	public:
		AssetNotFoundException(const String& asset) {
			mMessage = "Asset not found: ";
			mMessage += asset;
		}

		const char* what() const {
			return mMessage.toMultibyte().c_str();
		}
	};

	class AssetManager : public Singleton<AssetManager>
	{
		TextureManager mTexMgr;
		TextureLoaderPtr mTextureLoader;
		ResourceManagerPtr mResourceManager;
		Graphics::Device::GxDeviceWeakPtr mDevPtr;
		EffectCollection mEffects;

	public:
		void initAssets(Graphics::Device::GxDevicePtr device);
		void shutdown();

		TextureLoaderPtr getTextureLoader() const { return mTextureLoader; }

		Graphics::ShaderCodePtr getShaderCode(const String& resource);
		Graphics::TexturePtr loadTexture(const String& resource);
		Graphics::EffectPtr getEffect(Effects effect);
		void getAsset(const String& resource, std::shared_ptr<std::istream>& asset);

		bool hasAsset(const String& name) const { return mResourceManager->hasResource(name); }
	};
}

#define sAssets (Content::AssetManager::getInstance())