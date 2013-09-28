#pragma once

#include "ApiObject.h"

namespace Graphics
{
	SHARED_FORWARD(Texture);

	class Texture : public ApiObject
	{
	protected:
		static TexturePtr gDefaultTexture;

		Texture(ApiType type) : ApiObject(type) {

		}

		virtual void updateData(const void* memory, uint32 size) = 0;

	public:
		static TexturePtr getDefaultTexture() { return gDefaultTexture; }

		virtual void generateMipmaps() = 0;
		
		template<typename T>
		void updateData(const std::vector<T>& data) {
			updateData(data.data(), data.size() * sizeof(T));
		}
	};

	WEAK_TYPE(Texture);
}