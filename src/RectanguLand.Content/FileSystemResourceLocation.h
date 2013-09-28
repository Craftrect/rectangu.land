#pragma once

#include "ResourceLocation.h"

namespace Content
{
	class FileSystemResourceLocation : public ResourceLocation
	{
		SHARED_FORWARD(DirectoryEntry);
		SHARED_FORWARD(FileEntry);

		class DirectoryEntry
		{
			String mEntryName;
			std::unordered_map<String, DirectoryEntryPtr, String::StrHash> mSubDirectories;
			std::unordered_map<String, FileEntryPtr, String::StrHash> mFiles;

		public:
			DirectoryEntry(const String& name) {
				mEntryName = name;
			}

			const String& getName() const {
				return mEntryName;
			}

			void add(DirectoryEntryPtr dir) {
				mSubDirectories.insert(std::make_pair(dir->getName().toLower(), dir));
			}

			void add(FileEntryPtr file) {
				mFiles.insert(std::make_pair(file->getName().toLower(), file));
			}

			DirectoryEntryPtr getDirectory(const String& name) const {
				auto itr = mSubDirectories.find(name.toLower());

				if (itr == mSubDirectories.end()) {
					return nullptr;
				}

				return itr->second;
			}

			FileEntryPtr getFile(const String& name) const {
				auto itr = mFiles.find(name.toLower());

				if (itr == mFiles.end()) {
					return nullptr;
				}

				return itr->second;
			}
		};

		class FileEntry
		{
			String mEntryName;
		public:
			FileEntry(const String& name) {
				mEntryName = name;
			}

			const String& getName() const {
				return mEntryName;
			}
		};

		String mBasePath;
		DirectoryEntryPtr mSystemRoot;

		void enumFileSystem();
		void loadDirectory(DirectoryEntryPtr dir, const String& path);
	public:
		FileSystemResourceLocation(const String& basePath);
		
		bool exists(const String& resName);
		bool openResource(const String& resource, std::shared_ptr<std::istream>& stream);
	};
}