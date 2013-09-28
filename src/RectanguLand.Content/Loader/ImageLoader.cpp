#include "Precompiled.h"
#include "ImageLoader.h"
#include "dds.h"

namespace Content
{
	std::map<uint32, std::function<bool (ImageLoader*) >> ImageLoader::mSigToLoaderMap;
	std::map<uint32, std::function<bool (ImageLoader*) >> ImageLoader::mInterlaceLoaderMap;
	std::map<uint32, std::function<bool (ImageLoader*) >> ImageLoader::mColorLoaderMap;

	ImageLoader::ImageLoader(std::shared_ptr<std::istream> strm) : mStream(*strm) {
		mStream.seekAbs(0);
		uint32 sig = mStream.read<uint32>();

		if (mSigToLoaderMap.size() == 0) {
			mSigToLoaderMap['GNP\x89'] = std::bind(&ImageLoader::loadImagePng, std::placeholders::_1);
			mSigToLoaderMap[' SDD'] = std::bind(&ImageLoader::loadImageDDS, std::placeholders::_1);
		}

		auto itr = mSigToLoaderMap.find(sig);
		if (itr == mSigToLoaderMap.end()) {
			throw std::invalid_argument("Stream is no recognized image format");
		}

		if (itr->second(this) == false) {
			throw std::invalid_argument("Stream is not a valid image format or not supported.");
		}
	}

	bool ImageLoader::loadImagePng() {
		mStream.setEncoding(Utils::Encoding::BigEndian);

		uint32 sig = mStream.read<uint32>();
		if (sig != 0x0D0A1A0A) {
			return false;
		}

		auto watch = Utils::Stopwatch();
		watch.start();
		auto ret = parseImagePng();
		watch.stop();

		sLog->debug("PNG", "Image load time: ", watch.getMilliseconds());

		return ret;
	}

	bool ImageLoader::parseImagePng() {
		boost::iostreams::zlib_params params;

		auto hdr = getChunkPng();
		if (hdr.Signature != 'IHDR') {
			return false;
		}

		PngHeader header;
		header.Width       = mStream.read<uint32>();
		header.Height      = mStream.read<uint32>();
		header.BitDepth    = mStream.read<uint8>();
		header.ColorType   = mStream.read<uint8>();
		header.Compression = mStream.read<uint8>();
		header.Filter      = mStream.read<uint8>();
		header.Interlace   = mStream.read<uint8>();

		mPngHeader = header;

		// skip CRC
		mStream.seek(4);

		if (hdr.Size > 0x0D) {
			uint32 skip = hdr.Size - 0x0D;
			mStream.seek(skip);
		}

		// the image data decompressed is roughly about 4 bytes per pixel
		mPngData.resize(4 * (header.Width + 1) * header.Height);
		uint32 outputPos = 0;

		Utils::ZInflater inflater;
		inflater.begin();

		bool isAtEnd = false;
		while (isAtEnd == false) {
			PngChunkHeader cnk;
			try {
				cnk = getChunkPng();
			} catch (std::out_of_range&) {
				isAtEnd = true;
				continue;
			}

			switch (cnk.Signature) {
			case 'IEND':
				{
					isAtEnd = true;
					break;
				}

			case 'IDAT':
				{
					std::vector<char> curChunk(cnk.Size);
					mStream.read(curChunk);

					inflater.update(curChunk, mPngData, outputPos);
					break;
				}

			default:
				mStream.seek(cnk.Size);
				break;
			}

			// skip CRC
			mStream.seek(4);
		}

		inflater.end();

		

		if (mColorLoaderMap.size() == 0) {
			mColorLoaderMap[6] = std::bind(&ImageLoader::parseImagePng_32bppRGBA, std::placeholders::_1);
			mColorLoaderMap[0] = std::bind(&ImageLoader::parseImagePng_Grayscale, std::placeholders::_1);
		}

		auto itr = mColorLoaderMap.find(header.ColorType);
		if (itr == mColorLoaderMap.end()) {
			return false;
		}

		return itr->second(this);
	}

	bool ImageLoader::parseImagePng_Grayscale() {
		if (mPngHeader.BitDepth != 8) {
			return false;
		}

		mInfo.setLayerCount(1);
		auto& colorData = mInfo.getLayer(0);
		colorData.resize(mPngHeader.Width * mPngHeader.Height);

		for (uint32 i = 0; i < mPngHeader.Height; ++i) {
			auto start = mPngData.begin() + i * (mPngHeader.Width + 1);
			uint8 filter = *(start++);
			if (filter == 0) {

			} else if (filter == 1) {
				// sub filtering
				uint8* cur = (uint8*) &(*start);
				uint8 bpp = getPngBpp();

				for (uint32 j = 0; j < mPngHeader.Width; ++j) {
					uint8 prior = (j > bpp) ? *(cur - bpp) : 0;
					*cur += prior;
				}
			} else if (filter == 2) {
				uint8* cur = (uint8*) &(*start);
				for (uint32 j = 0; j < mPngHeader.Width; ++j) {
					uint8 prior = (i > 0) ? *(cur - mPngHeader.Width) : 0;
					*cur += prior;
				}
			} else if (filter == 3) {
				uint8* cur = (uint8*) &(*start);
				uint8 bpp = getPngBpp();
				for (uint32 j = 0; j < mPngHeader.Width; ++j) {
					uint8 above = (i > 0) ? *(cur - mPngHeader.Width) : 0;
					uint8 left = (j > bpp) ? *(cur - bpp) : 0;

					uint8 addend = (above + left) / 2;
					*cur += addend;
				}
			} else if (filter == 4) {
				return false;
			}

			for (uint32 j = 0; j < mPngHeader.Width; ++j) {
				uint32 val = *(start++);
				*(uint32*)&colorData[i * mPngHeader.Width * 4 + j * 4] = 0xFF000000 | (val << 16) | (val << 8) | (val << 0);
			}
		}

		mInfo.setDimension(mPngHeader.Width, mPngHeader.Height);

		return true;
	}

	bool ImageLoader::parseImagePng_32bppRGBA() {
		if (mPngHeader.BitDepth != 8) {
			return false;
		}

		mInfo.setLayerCount(1);
		auto& colorData = mInfo.getLayer(0);
		colorData.resize(mPngHeader.Width * mPngHeader.Height);
		for (uint32 i = 0; i < mPngHeader.Height; ++i) {
			auto start = mPngData.begin() + i * (4 * mPngHeader.Width + 1);
			uint8 filter = *(start++);
			if (filter == 0) {
				
			} else if (filter == 1) {
				// sub filtering
				uint8* cur = (uint8*)&(*start);
				uint8 bpp = getPngBpp();

				for (uint32 j = 0; j < 4 * mPngHeader.Width; ++j) {
					uint8 prior = (j > bpp) ? *(cur - bpp) : 0;
					*cur += prior;
				}
			} else if (filter == 2) {
				// add filtering
				uint8* cur = (uint8*) &(*start);
				for (uint32 j = 0; j < 4 * mPngHeader.Width; ++j) {
					uint8 prior = (i > 0) ? *(cur - 4 * mPngHeader.Width) : 0;
					*cur += prior;
				}
			} else if (filter == 3) {
				uint8* cur = (uint8*) &(*start);
				uint8 bpp = getPngBpp();
				for (uint32 j = 0; j < 4 * mPngHeader.Width; ++j) {
					uint8 above = (i > 0) ? *(cur - 4 * mPngHeader.Width) : 0;
					uint8 left = (j > bpp) ? *(cur - bpp) : 0;

					uint8 addend = (above + left) / 2;
					*cur += addend;
				}
			} else if (filter == 4) {
				//!++ TODO: Implement
				return false;
			}

			memcpy(colorData.data() + i * mPngHeader.Width, &(*start), 4 * mPngHeader.Width);
		}

		mInfo.setDimension(mPngHeader.Width, mPngHeader.Height);

		return true;
	}

	uint8 ImageLoader::getPngBpp() {
		uint8 bpp = 0;
		switch (mPngHeader.ColorType) {
		case 0:
			bpp = 1;
			break;

		case 2:
			bpp = 3;
			break;

		case 3:
			bpp = 1;
			break;

		case 4:
			bpp = 1;
			break;

		case 6:
			bpp = 4;
			break;
		}

		switch (mPngHeader.BitDepth) {
		case 1:
			bpp /= 8;
			break;

		case 2:
			bpp /= 4;
			break;

		case 4:
			bpp /= 2;
			break;

		case 8:
			bpp *= 1;
			break;

		case 16:
			bpp *= 2;
			break;
		}

		if (bpp == 0) {
			bpp = 1;
		}

		return bpp;
	}

	ImageLoader::PngChunkHeader ImageLoader::getChunkPng() {
		PngChunkHeader header;
		header.Size      = mStream.read<uint32>();
		header.Signature = mStream.read<uint32>();

		return header;
	}

	bool ImageLoader::loadImageDDS() {
		mDdsHeader = mStream.read<DDS_HEADER>();
		if (mDdsHeader.dwHeaderFlags & DDS_HEADER_FLAGS_VOLUME) {
			return false;
		}

		if ((mDdsHeader.dwHeaderFlags & DDS_HEADER_FLAGS_MIPMAP) == 0) {
			mDdsHeader.dwMipMapCount = 1;
		}

		if ((mDdsHeader.dwHeaderFlags & DDS_HEADER_FLAGS_TEXTURE) == 0) {
			return false;
		}

		mInfo.setDimension(mDdsHeader.dwWidth, mDdsHeader.dwHeight);

		if ((mDdsHeader.dwSurfaceFlags & DDS_SURFACE_FLAGS_CUBEMAP)) {
			return parseDDSCubemap();
		}

		switch (mDdsHeader.ddspf.dwFlags) {
		case DDS_FOURCC:
			return parseDDSDXT();

		case DDS_RGB:
			return parseDDSRGB();

		case DDS_RGBA:
			return parseDDSRGBA();

		default:
			return false;
		}
	}

	bool ImageLoader::parseDDSCubemap() {
		bool hasAllSides = (mDdsHeader.dwCubemapFlags & (DDS_CUBEMAP_ALLFACES)) == DDS_CUBEMAP_ALLFACES;
		if (hasAllSides == false) {
			return false;
		}

		mInfo.setCubemap(true);

		for (uint32 i = 0; i < 6; ++i) {
			switch (mDdsHeader.ddspf.dwFlags) {
			case DDS_FOURCC:
				if (parseDDSDXT() == false) {
					return false;
				}
				break;

			case DDS_RGB:
				if (parseDDSRGB() == false) {
					return false;
				}
				break;

			case DDS_RGBA:
				if (parseDDSRGBA() == false) {
					return false;
				}
				break;

			default:
				return false;
			}

			mInfo.addCubeFace(mInfo.getAllLayers());
		}

		return true;
	}

	bool ImageLoader::parseDDSDXT() {
		return false;
	}

	bool ImageLoader::parseDDSRGBA() {
		mInfo.setLayerCount(mDdsHeader.dwMipMapCount);

		uint32 curw = mDdsHeader.dwWidth;
		uint32 curh = mDdsHeader.dwHeight;

		uint8 uncompData[4];
		uint16 compData;

		for (uint32 i = 0; i < mDdsHeader.dwMipMapCount; ++i) {
			auto& curLayer = mInfo.getLayer(i);
			uint32 w = std::max(1u, curw >> i);
			uint32 h = std::max(1u, curh >> i);

			curLayer.resize(w * h * 4);
			uint8* curPtr = (uint8*) curLayer.data();

			for (uint32 y = 0; y < h; ++y) {
				for (uint32 x = 0; x < w; ++x) {
					if (mDdsHeader.ddspf.dwRGBBitCount == 32) {
						mStream.read(uncompData);

						*(curPtr++) = uncompData[2];
						*(curPtr++) = uncompData[1];
						*(curPtr++) = uncompData[0];
						*(curPtr++) = uncompData[3];

					} else if (mDdsHeader.ddspf.dwRGBBitCount == 16 && mDdsHeader.ddspf.dwABitMask == 0x8000) {
						mStream.read(compData);

						uint8 blue = (uint8) (((compData & 0x1F) / 31.0f) * 255.0f);
						uint8 green = (uint8) ((((compData >> 5) & 0x1F) / 31.0f) * 255.0f);
						uint8 red = (uint8) ((((compData >> 10) & 0x1F) / 31.0f) * 255.0f);
						uint8 alpha = (compData >> 15) > 0 ? 255 : 0;

						*(curPtr++) = red;
						*(curPtr++) = green;
						*(curPtr++) = blue;
						*(curPtr++) = alpha;

					} else if (mDdsHeader.ddspf.dwRGBBitCount == 16 && mDdsHeader.ddspf.dwABitMask == 0xF000) {
						mStream.read(compData);

						uint8 blue = (uint8) (((compData & 0x0F) / 15.0f) * 255.0f);
						uint8 green = (uint8) ((((compData >> 4) & 0x0F) / 15.0f) * 255.0f);
						uint8 red = (uint8) ((((compData >> 8) & 0x0F) / 15.0f) * 255.0f);
						uint8 alpha = (uint8) ((((compData >> 12) & 0x0F) / 15.0f) * 255.0f);

						*(curPtr++) = red;
						*(curPtr++) = green;
						*(curPtr++) = blue;
						*(curPtr++) = alpha;
					} else {
						return false;
					}
				}
			}
		}

		return true;
	}

	bool ImageLoader::parseDDSRGB() {
		mInfo.setLayerCount(mDdsHeader.dwMipMapCount + 1);

		uint32 curw = mDdsHeader.dwWidth;
		uint32 curh = mDdsHeader.dwHeight;

		uint8 fullData[4];
		uint8 uncompData[3];
		uint16 compData;

		for (uint32 i = 0; i < mDdsHeader.dwMipMapCount + 1; ++i) {
			auto& curLayer = mInfo.getLayer(i);
			uint32 w = std::max(1u, curw >> i);
			uint32 h = std::max(1u, curh >> i);

			curLayer.resize(w * h * 4);
			uint8* curPtr = (uint8*) curLayer.data();

			for (uint32 y = 0; y < h; ++y) {
				for (uint32 x = 0; x < w; ++x) {
					if (mDdsHeader.ddspf.dwRGBBitCount == 32) {
						mStream.read(fullData);

						*(curPtr++) = fullData[0];
						*(curPtr++) = fullData[1];
						*(curPtr++) = fullData[2];
						*(curPtr++) = fullData[3];

					} else if (mDdsHeader.ddspf.dwRGBBitCount == 24) {
						mStream.read(uncompData);
						
						*(curPtr++) = uncompData[0];
						*(curPtr++) = uncompData[1];
						*(curPtr++) = uncompData[2];
						*(curPtr++) = 0xFF;

					} else if (mDdsHeader.ddspf.dwRGBBitCount == 16) {
						mStream.read(compData);

						uint8 blue   = (uint8) (((compData & 0x1F) / 31.0f) * 255.0f);
						uint8 green = (uint8) ((((compData >> 5) & 0x3F) / 63.0f) * 255.0f);
						uint8 red  = (uint8) ((((compData >> 11) & 0x1F) / 31.0f) * 255.0f);

						*(curPtr++) = red;
						*(curPtr++) = green;
						*(curPtr++) = blue;
						*(curPtr++) = 0xFF;
					} else {
						return false;
					}
				}
			}
		}

		return true;
	}
}