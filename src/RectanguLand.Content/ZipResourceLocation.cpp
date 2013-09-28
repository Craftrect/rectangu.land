#include "Precompiled.h"
#include "ZipResourceLocation.h"

#include <zip/zip.h>

namespace Content
{
	class ZipIStream : public std::basic_istream<char>, public std::streambuf
	{
		std::vector<char> mData;
	protected:
		std::streambuf::int_type underflow() {
			if (gptr() < egptr()) {
				return std::streambuf::traits_type::to_int_type(*gptr());
			}

			return std::streambuf::traits_type::eof();
		}

	public:
		ZipIStream(std::vector<char>& data) : std::basic_istream<char>(this, false) {
			mData = data;
			setg(&mData.front(), &mData.front(), &mData.back());
		}
	};

	ZipResourceLocation::ZipResourceLocation(const String& zipFile) {
		mHandle = zip_open(zipFile.toMultibyte().c_str(), 0, nullptr);
		if (mHandle == nullptr) {
			throw std::invalid_argument("Not a valid zip-file");
		}
	}

	bool ZipResourceLocation::exists(const String& file) {
		struct zip_stat stat;
		
		zip_stat_init(&stat);
		return zip_stat((zip*) mHandle, file.toMultibyte().c_str(), ZIP_STAT_COMP_SIZE, &stat) == 0;
	}

	bool ZipResourceLocation::openResource(const String& resource, std::shared_ptr<std::istream>& stream) {
		struct zip_stat stat;

		zip_stat_init(&stat);
		bool hasFile = zip_stat((zip*) mHandle, resource.toMultibyte().c_str(), ZIP_STAT_COMP_SIZE, &stat) == 0;
		if (hasFile = false) {
			return false;
		}

		auto zipFile = zip_fopen((zip*) mHandle, resource.toMultibyte().c_str(), 0);
		if (zipFile == nullptr) {
			return false;
		}

		std::vector<char> data((size_t)stat.size);
		auto read = zip_fread(zipFile, data.data(), data.size());
		zip_fclose(zipFile);

		if (read == 0) {
			return false;
		}

		data.resize((size_t)read);

		stream = std::make_shared<ZipIStream>(data);
		return true;
	}
}