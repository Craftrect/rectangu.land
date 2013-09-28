#pragma once

#include "ResourceLocation.h"

namespace Content
{
	class ResourceManager
	{
	private:
		std::list<ResourceLocationPtr> mLocations;

	public:
		ResourceLocationPtr createZipLocation(const String& zipFile);
		ResourceLocationPtr createDirectoryLocation(const String& directory);

		bool hasResource(const String& resource);
		bool getResource(const String& resource, std::shared_ptr<std::istream>& stream);

		void initDefaultLocations();
	};

	SHARED_TYPE(ResourceManager);
}