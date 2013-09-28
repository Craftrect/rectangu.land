#pragma once

namespace Content
{
	class ResourceLocation
	{
	public:
		virtual bool openResource(const String& resource, std::shared_ptr<std::istream>& stream) = 0;
		virtual bool exists(const String& resource) = 0;
	};

	SHARED_TYPE(ResourceLocation);
}