#include "Precompiled.h"
#include "TextureManager.h"
#include "AssetManager.h"

namespace Content
{
	Graphics::TexturePtr TextureManager::getTexture(const String& name) {
		uint32 hash = name.toLower().hash();
		auto itr = mTextureCache.find(hash);
		if (itr != mTextureCache.end()) {
			return itr->second.lock();
		}

		auto loader = sAssets->getTextureLoader();
		std::shared_ptr<std::istream> res;
		sAssets->getAsset(name, res);

		auto ret = loader->loadTexture(res);
		mTextureCache[hash] = ret;
		return ret;
	}
}