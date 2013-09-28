#pragma once

namespace Content
{
	class TextureManager
	{
	private:
		std::map<uint32, Graphics::TextureWeakPtr> mTextureCache;

	public:
		Graphics::TexturePtr getTexture(const String& name);
	};
}