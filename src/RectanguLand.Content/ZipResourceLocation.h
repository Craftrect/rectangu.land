#pragma once

#include "ResourceLocation.h"

namespace Content
{
	class ZipResourceLocation : public ResourceLocation
	{
		void* mHandle;
	public:
		ZipResourceLocation(const String& zipFile);

		bool exists(const String& file);
		bool openResource(const String& file, std::shared_ptr<std::istream>& stream);
	};
}