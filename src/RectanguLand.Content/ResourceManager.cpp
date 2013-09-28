#include "Precompiled.h"
#include "ResourceManager.h"
#include "ZipResourceLocation.h"
#include "FileSystemResourceLocation.h"

namespace Content
{
	ResourceLocationPtr ResourceManager::createZipLocation(const String& zipFile) {
		auto loc = std::make_shared<ZipResourceLocation>(zipFile);
		mLocations.push_back(loc);
		return loc;
	}

	ResourceLocationPtr ResourceManager::createDirectoryLocation(const String& zipFile) {
		auto loc = std::make_shared<FileSystemResourceLocation>(zipFile);
		mLocations.push_back(loc);
		return loc;
	}

	bool ResourceManager::hasResource(const String& resource) {
		for (auto& loc : mLocations) {
			if (loc->exists(resource)) {
				return true;
			}
		}

		return false;
	}

	bool ResourceManager::getResource(const String& resource, std::shared_ptr<std::istream>& stream) {
		for (auto& loc : mLocations) {
			if (loc->openResource(resource, stream)) {
				return true;
			}
		}

		return false;
	}

	void ResourceManager::initDefaultLocations() {
		try {
			createZipLocation("Content.zip");
		} catch (std::exception&) {
			sLog->notice("IO", "Unable to load Content.zip");
		}

		createDirectoryLocation("Content");
	}
}