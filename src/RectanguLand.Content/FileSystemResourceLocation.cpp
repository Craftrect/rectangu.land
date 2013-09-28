#include "Precompiled.h"
#include "FileSystemResourceLocation.h"

using namespace std::tr2::sys;

namespace Content
{
	FileSystemResourceLocation::FileSystemResourceLocation(const String& path) : mBasePath(path) {
		for (auto& c : mBasePath) {
			if (c == L'\\') {
				c = L'/';
			}
		}

		if (mBasePath.endsWith(L"/") == true) {
			mBasePath = mBasePath.substring(0, mBasePath.length() - 1);
		}

		enumFileSystem();
	}

	void FileSystemResourceLocation::enumFileSystem() {
		mSystemRoot = std::make_shared<DirectoryEntry>(mBasePath);

		loadDirectory(mSystemRoot, mBasePath);
	}

	void FileSystemResourceLocation::loadDirectory(DirectoryEntryPtr dir, const String& path) {
		directory_iterator itr(path.toMultibyte());
		directory_iterator end;

		while (itr != end) {
			auto entry = *itr;
			String partPath = entry.path().relative_path().string();
			
			if (entry.status().type() == file_type::directory_file) {
				
				String newPath = path;
				newPath += "/";
				newPath += partPath;

				DirectoryEntryPtr dentry = std::make_shared<DirectoryEntry>(partPath);
				dir->add(dentry);

				loadDirectory(dentry, newPath);
			} else {
				dir->add(std::make_shared<FileEntry>(partPath));
			}

			++itr;
		}
	}

	bool FileSystemResourceLocation::exists(const String& name) {
		std::list<String> parts;
		name.split(parts, L'/');

		if (parts.size() == 0) {
			return false;
		}

		String filePart = parts.front();
		parts.pop_front();

		DirectoryEntryPtr curDir = mSystemRoot;

		for (auto& part : parts) {
			curDir = curDir->getDirectory(part);
			if (curDir == nullptr) {
				return false;
			}
		}

		return curDir->getFile(filePart) != nullptr;
	}

	bool FileSystemResourceLocation::openResource(const String& name, std::shared_ptr<std::istream>& stream) {
		std::list<String> parts;
		name.split(parts, L'/');

		if (parts.size() == 0) {
			return false;
		}

		String filePart = parts.back();
		parts.pop_back();

		DirectoryEntryPtr curDir = mSystemRoot;

		for (auto& part : parts) {
			curDir = curDir->getDirectory(part);
			if (curDir == nullptr) {
				return false;
			}
		}

		

		bool hasFile = curDir->getFile(filePart) != nullptr;
		if (hasFile) {
			String fullPath = mBasePath;
			fullPath += "/";
			fullPath += name;

			stream = std::make_shared<std::ifstream>(fullPath.toMultibyte(), std::ios::binary);
		}

		return hasFile;
	}
}