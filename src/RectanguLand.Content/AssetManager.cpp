#include "Precompiled.h"
#include "AssetManager.h"

#include "Loader/TextureLoaderD3D11.h"

INIT_SINGLETON(Content::AssetManager);

namespace Content
{
	void AssetManager::initAssets(Graphics::Device::GxDevicePtr device) {
		switch (device->getType()) {
		case Graphics::ApiType::D3D11:
			{
				mTextureLoader = std::make_shared<TextureLoaderD3D11>(std::dynamic_pointer_cast<Graphics::Device::GxDeviceD3D11>(device));
			}
			break;
		}

		mResourceManager = std::make_shared<ResourceManager>();
		mResourceManager->initDefaultLocations();

		mDevPtr = device;

		mEffects.initEffects();
	}

	void AssetManager::shutdown() {
		mTextureLoader->shutdownThreads();
	}

	Graphics::ShaderCodePtr AssetManager::getShaderCode(const String& resource) {
		std::shared_ptr<std::istream> resStrm;
		bool found = mResourceManager->getResource(resource, resStrm);
		if (found == false) {
			throw AssetNotFoundException(resource);
		}

		auto code = mDevPtr.lock()->createShaderCode();
		code->loadFromBuffer(resStrm);

		return code;
	}

	Graphics::TexturePtr AssetManager::loadTexture(const String& tex) {
		return mTexMgr.getTexture(tex);
	}

	Graphics::EffectPtr AssetManager::getEffect(Effects effect) {
		try {
			auto eff = mEffects.getEffect(effect);
			return eff;
		} catch (std::invalid_argument&) {
			throw AssetNotFoundException("Effect");
		}
	}

	void AssetManager::getAsset(const String& name, std::shared_ptr<std::istream>& stream) {
		bool exists = mResourceManager->getResource(name, stream);
		if (exists == false) {
			throw AssetNotFoundException(name);
		}
	}
}