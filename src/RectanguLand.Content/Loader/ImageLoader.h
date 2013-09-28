#pragma once

#include "dds.h"

namespace Content
{
	class ImageLoadInfo
	{
		bool mIsCubemap;

		uint32 mWidth;
		uint32 mHeight;
		uint32 mNumLayers;
		std::vector<std::vector<uint32>> mLayerData;

		std::vector<std::vector<std::vector<uint32>>> mCubemapData;
	public:
		ImageLoadInfo() : mIsCubemap(false) { }

		uint32 getWidth() const { return mWidth; }
		uint32 getHeight() const { return mHeight; }
		uint32 getLayerCount() const { return mNumLayers; }
		const std::vector<std::vector<uint32>>& getAllLayers() const { return mLayerData; }
		const std::vector<uint32>& getLayer(uint32 index) const { return mLayerData[index]; }
		std::vector<uint32>& getLayer(uint32 index) { return mLayerData[index]; }

		const std::vector<std::vector<uint32>>& getCubeFace(uint32 face) const { return mCubemapData[face]; }
		std::vector<std::vector<uint32>>& getCubeFace(uint32 face) { return mCubemapData[face]; }
		void addCubeFace(const std::vector<std::vector<uint32>>& faceData) { mCubemapData.push_back(faceData); }

		void setDimension(uint32 w, uint32 h) { mWidth = w; mHeight = h; }
		void setLayerCount(uint32 numLayers){ mNumLayers = numLayers; mLayerData.resize(numLayers); }
		void setCubemap(bool cubemap) { mIsCubemap = cubemap; }

		bool isCubemap() const { return mIsCubemap; }
	};

	class ImageLoader
	{
		struct PngChunkHeader
		{
			uint32 Size;
			uint32 Signature;
		};

		struct PngHeader
		{
			uint32 Width;
			uint32 Height;
			uint8 BitDepth;
			uint8 ColorType;
			uint8 Compression;
			uint8 Filter;
			uint8 Interlace;
		};

		ImageLoadInfo mInfo;
		Utils::BinaryStream mStream;
		PngHeader mPngHeader;
		std::vector<char> mPngData;

		DDS_HEADER mDdsHeader;

		static std::map<uint32, std::function<bool (ImageLoader*)>> mSigToLoaderMap;
		static std::map<uint32, std::function<bool (ImageLoader*) >> mInterlaceLoaderMap;
		static std::map<uint32, std::function<bool (ImageLoader*) >> mColorLoaderMap;

		PngChunkHeader getChunkPng();
		bool parseImagePng();
		bool parseImagePng_32bppRGBA();
		bool parseImagePng_GrayscaleAlpha();
		bool parseImagePng_Grayscale();
		bool parseImagePng_Palette();
		uint8 getPngBpp();

		bool parseDDSRGB();
		bool parseDDSRGBA();
		bool parseDDSDXT();

		bool parseDDSCubemap();

		bool loadImagePng();
		bool loadImageDDS();
	public:
		ImageLoader(std::shared_ptr<std::istream> strm);

		const ImageLoadInfo& getInfo() const { return mInfo; }
	};

	SHARED_TYPE(ImageLoader);
}