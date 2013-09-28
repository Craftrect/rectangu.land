#include "stdafx.h"
#include "MapChunk.h"
#include "TerrainNoise.h"
#include "ChunkManager.h"

namespace Terrain
{
	Graphics::InputGeometryPtr MapChunk::mCubeGeometry = nullptr;

	MapChunk::MapChunk(uint32 ix, uint32 iy) : mIndexX(ix), mIndexY(iy) {
		mIsAsyncLoaded = false;
		mIsSyncLoaded = false;
		mVerticesDirty = false;

		if (mCubeGeometry == nullptr) {
			initGeometry();
		}
	}

	void MapChunk::generate() {
		uint32 baseX = mIndexX * 16;
		uint32 baseY = mIndexY * 16;

		for (uint32 y = 0; y < 16; ++y) {
			for (uint32 x = 0; x < 16; ++x) {
				uint32 height = sTerrainNoise->sampleHeight(((baseX + x) * 0.1f), ((baseY + y) * 0.1f));

				auto& row = mRows[y * 16 + x];
				row.setHeight(height);

				for (uint32 i = 0; i < height; ++i) {
					uint32 p = (uint32) ((rand() / (float) RAND_MAX) * 3);
					if (p < 3) {
						row.setBlockType(i, BlockType::Dirt);
					} else {
						row.setBlockType(i, BlockType::Stone);
					}

					CubeVertex v;
					v.x = (mIndexX * 16.0f + x);
					v.y = (mIndexY * 16.0f + y);
					v.z = (float) i;

					mVertices.push_back(v);
				}
			}
		}
	}

	void MapChunk::saveToFile(const String& fileName) {
		std::stringstream strm;

		strm.write((const char*) &mIndexX, sizeof(uint32));
		strm.write((const char*) &mIndexY, sizeof(uint32));

		for (uint32 y = 0; y < 16; ++y) {
			for (uint32 x = 0; x < 16; ++x) {
				auto& row = mRows[y * 16 + x];

				uint16 height = (uint16) row.getHeight();
				strm.write((const char*) &height, sizeof(uint16));

				BlockType curBlock = BlockType::Invalid;
				uint16 curCount = 0;
				bool isFirst = true;

				for (uint16 i = 0; i < height; ++i) {
					++curCount;
					if (isFirst == true) {
						curBlock = row.getType(i);
						isFirst = false;
						continue;
					}

					if (i == (height - 1) || row.getType(i) != row.getType(i - 1)) {
						strm.write((const char*) &curCount, sizeof(uint16));
						strm.write((const char*) &curBlock, sizeof(uint16));
						curCount = 0;
						curBlock = row.getType(i);
					}
				}

				if (curCount != 0) {
					strm.write((const char*) &curCount, sizeof(uint16));
					strm.write((const char*) &curBlock, sizeof(uint16));
				}
			}
		}

		uint32 size = (uint32)strm.tellp();
		strm.seekp(0, std::ios::beg);
		strm.seekg(0, std::ios::beg);

		std::vector<char> compressed(size);
		std::vector<char> uncompressed(size);

		strm.read(uncompressed.data(), size);
		Utils::ZDeflater deflater;
		deflater.begin();
		uint32 out = 0;
		deflater.update(uncompressed, compressed, out);
		deflater.end();

		std::ofstream fileOut(fileName.toMultibyte(), std::ios::binary | std::ios::out);
		fileOut.write((const char*) &size, sizeof(uint32));
		fileOut.write((const char*) &out, sizeof(uint32));
		fileOut.write(compressed.data(), out);
		fileOut.close();
	}

	void MapChunk::initGeometry() {
		mCubeGeometry = std::make_shared<Graphics::InputGeometry>();

		mCubeGeometry->addVertexBuffer(nullptr);
		mCubeGeometry->setEffect(sAssets->getEffect(Content::Effects::Block));
		mCubeGeometry->setInputLayout(Graphics::InputLayout::Points);
		
		auto pos = std::make_shared<Graphics::VertexElement>(Graphics::Semantic::Position, 4, 0);
		auto face0 = std::make_shared<Graphics::VertexElement>(Graphics::Semantic::TexCoord, 4, 0);
		auto face1 = std::make_shared<Graphics::VertexElement>(Graphics::Semantic::TexCoord, 4, 1);
		mCubeGeometry->addVertexElement(pos);
		mCubeGeometry->addVertexElement(face0);
		mCubeGeometry->addVertexElement(face1);

		mCubeGeometry->finalize(sWorldFrame->getDevice());
	}

	void MapChunk::render() {
		if (mIsAsyncLoaded == false) {
			return;
		}

		if (mIsSyncLoaded == false) {
			syncLoad();
		}

		if (mVerticesDirty == true) {
			mVertexBuffer->setData(mVertices);
			mVerticesDirty = false;
		}

		mCubeGeometry->setVertexCount(mVertices.size());
		mCubeGeometry->setTriangleCount(mIndices.size());
		mCubeGeometry->setIndexBuffer(mVisibleIndices);
		mCubeGeometry->setVertexBuffer(0, mVertexBuffer);
		mCubeGeometry->getEffect()->getParamManager(Graphics::ShaderCodeType::Pixel)->setTexture(Graphics::ShaderVariables::tex_Block,
			sAssets->loadTexture("textures/Grass.dds"));
		
		sWorldFrame->getDevice()->getPipeline()->execute(mCubeGeometry);
	}

	void MapChunk::update() {

	}

	void MapChunk::syncLoad() {
		if (mIsSyncLoaded == true) {
			return;
		}

		mVertexBuffer = sWorldFrame->getDevice()->createVertexBuffer();
		mVertexBuffer->setStride(sizeof(CubeVertex));
		mVertexBuffer->setData(mVertices);

		mVisibleIndices = sWorldFrame->getDevice()->createIndexBuffer();
		mVisibleIndices->setIndexType(Graphics::IndexType::Int);
		mVisibleIndices->setData(mIndices);

		mIsSyncLoaded = true;
	}

	void MapChunk::asyncLoad() {
		static const String mapPrefix = "World/Maps";
		static const String continent = "Cromoland";

		if (mIsAsyncLoaded == true) {
			return;
		}

		std::wstringstream mapFileName;
		mapFileName << mapPrefix << L"/" << continent << L"/" << continent << L"_" << mIndexX << L"_" << mIndexY;

		bool hasAsset = sAssets->hasAsset(mapFileName.str());
		if (hasAsset == false) {
			generate();
			fillVisibleBlocks();
			mIsAsyncLoaded = true;
			return;
		}

		std::shared_ptr<std::istream> input;
		sAssets->getAsset(mapFileName.str(), input);

		uint32 decompressed = 0;
		uint32 compressed = 0;

		input->read((char*) &decompressed, sizeof(uint32));
		input->read((char*) &compressed, sizeof(uint32));

		std::vector<char> compData(compressed);
		std::vector<char> uncompData(decompressed);

		input->read(compData.data(), compressed);

		uint32 outPos = 0;

		Utils::ZInflater inflater;
		inflater.begin();
		inflater.update(compData, uncompData, outPos);
		inflater.end();

		std::stringstream strm;
		strm.write(uncompData.data(), uncompData.size());
		strm.seekp(0);
		strm.seekg(0);

		uint32 indexX, indexY;
		strm.read((char*) &indexX, sizeof(uint32));
		strm.read((char*) &indexY, sizeof(uint32));

		mVertices.reserve(16 * 16 * 512);

		for (uint32 y = 0; y < 16; ++y) {
			for (uint32 x = 0; x < 16; ++x) {
				uint16 height = 0;
				strm.read((char*) &height, sizeof(uint16));

				mRows[y * 16 + x].setHeight(height);
				for (uint32 h = 0; h < height; ++h) {
					uint16 count = 0;
					uint16 type = 0;
					strm.read((char*) &count, sizeof(uint16));
					strm.read((char*) &type, sizeof(uint16));

					for (uint32 c = 0; c < count; ++c, ++h) {
						mRows[y * 16 + x].setBlockType(h, (BlockType) type);
						CubeVertex v;
						v.x = (indexX * 16.0f + x);
						v.y = (indexY * 16.0f + y);
						v.z = (float)h;
						mVertices.push_back(v);
					}
				}
			}
		}

		mVertices.shrink_to_fit();

		fillVisibleBlocks();
		mIsAsyncLoaded = true;
	}

	void MapChunk::fillVisibleBlocks() {
		mIndices.reserve(16 * 16 * 512);

		uint32 counter = 0;
		for (uint32 y = 0; y < 16; ++y) {
			for (uint32 x = 0; x < 16; ++x) {
				auto& row = mRows[y * 16 + x];

				if (x == 0 || y == 0 || x == 15 || y == 15) {
					for (uint32 h = 0; h < row.getHeight(); ++h, ++counter) {
						mIndices.push_back(counter);
					}

					continue;
				}

				auto& lrow = mRows[y * 16 + x + 1];
				auto& rrow = mRows[y * 16 + x - 1];
				auto& trow = mRows[(y + 1) * 16 + x];
				auto& brow = mRows[(y - 1) * 16 + x];

				mIndices.push_back(counter++);

				for (uint32 h = 1; h < row.getHeight(); ++h, ++counter) {
					bool hasLeft = lrow.getHeight() > h;
					bool hasRight = rrow.getHeight() > h;
					bool hasFore = trow.getHeight() > h;
					bool hasBack = brow.getHeight() > h;
					bool hasBottom = true;
					bool hasTop = row.getHeight() > h + 1;

					if ((hasLeft && hasRight && hasFore && hasBack && hasBottom && hasTop) == false) {
						mIndices.push_back(counter);
					}
				}
			}
		}

		mIndices.shrink_to_fit();
	}

	void MapChunk::bakeOcclusion() {
		uint32 counter = 0;
		for (uint32 y = 0; y < 16; ++y) {
			for (uint32 x = 0; x < 16; ++x) {
				auto& row = mRows[y * 16 + x];

				int32 baseIndexX = mIndexX * 16 + x;
				int32 baseIndexY = mIndexY * 16 + y;

				bool noRight = x == 15;
				bool noLeft = x == 0;
				bool noFront = y == 15;
				bool noBack = y == 0;

				int32 irx = baseIndexX + 1;
				int32 iry = baseIndexY;
				int32 ilx = baseIndexX - 1;
				int32 ily = baseIndexY;
				int32 ifx = baseIndexX;
				int32 ify = baseIndexY + 1;
				int32 ibx = baseIndexX;
				int32 iby = baseIndexY - 1;

				for (uint32 h = 0; h < row.getHeight(); ++h, ++counter) {
					uint32 height = 0;

					bool side1 = sChunkMgr->getHeightByCoord(ilx, ily, height) && height > h + 1;
					bool side2 = sChunkMgr->getHeightByCoord(ibx, iby, height) && height > h + 1;
					bool corner = sChunkMgr->getHeightByCoord(ilx, iby, height) && height > h + 1;

					mVertices[counter].occlusions[7] = getOcclusion(side1, side2, corner); // 3

					side1 = sChunkMgr->getHeightByCoord(irx, iry, height) && height > h + 1;
					side2 = sChunkMgr->getHeightByCoord(ibx, iby, height) && height > h + 1;
					corner = sChunkMgr->getHeightByCoord(irx, iby, height) && height > h + 1;

					mVertices[counter].occlusions[6] = getOcclusion(side1, side2, corner); // 2

					side1 = sChunkMgr->getHeightByCoord(irx, iry, height) && height > h + 1;
					side2 = sChunkMgr->getHeightByCoord(ifx, ify, height) && height > h + 1;
					corner = sChunkMgr->getHeightByCoord(irx, ify, height) && height > h + 1;

					mVertices[counter].occlusions[2] = getOcclusion(side1, side2, corner); // 6

					side1 = sChunkMgr->getHeightByCoord(ilx, ily, height) && height > h + 1;
					side2 = sChunkMgr->getHeightByCoord(ifx, ify, height) && height > h + 1;
					corner = sChunkMgr->getHeightByCoord(ilx, ify, height) && height > h + 1;

					mVertices[counter].occlusions[3] = getOcclusion(side1, side2, corner); // 7

					side1 = sChunkMgr->getHeightByCoord(ilx, ily, height) && height > h - 1;
					side2 = sChunkMgr->getHeightByCoord(ibx, iby, height) && height > h - 1;
					corner = sChunkMgr->getHeightByCoord(ilx, iby, height) && height > h - 1;

					mVertices[counter].occlusions[5] = getOcclusion(side1, side2, corner);

					side1 = sChunkMgr->getHeightByCoord(irx, iry, height) && height > h - 1;
					side2 = sChunkMgr->getHeightByCoord(ibx, iby, height) && height > h - 1;
					corner = sChunkMgr->getHeightByCoord(irx, iby, height) && height > h - 1;

					mVertices[counter].occlusions[4] = getOcclusion(side1, side2, corner); // 1

					side1 = sChunkMgr->getHeightByCoord(irx, iry, height) && height > h - 1;
					side2 = sChunkMgr->getHeightByCoord(ifx, ify, height) && height > h - 1;
					corner = sChunkMgr->getHeightByCoord(irx, ify, height) && height > h - 1;

					mVertices[counter].occlusions[0] = getOcclusion(side1, side2, corner); // 5

					side1 = sChunkMgr->getHeightByCoord(ilx, ily, height) && height > h - 1;
					side2 = sChunkMgr->getHeightByCoord(ifx, ify, height) && height > h - 1;
					corner = sChunkMgr->getHeightByCoord(ilx, ify, height) && height > h - 1;

					mVertices[counter].occlusions[1] = getOcclusion(side1, side2, corner); // 4
				}
			}
		}
	}

	float MapChunk::getOcclusion(bool side1, bool side2, bool corner) {
		if (side1 && side2) {
			return 0.0f;
		}

		return (3.0f - (side1 ? 1 : 0) - (side2 ? 1 : 0) - (corner ? 1 : 0)) / 3.0f;
	}

	void MapChunk::asyncUpdateLight() {
		bakeOcclusion();
		mVerticesDirty = true;
	}
}