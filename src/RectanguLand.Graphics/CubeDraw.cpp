#include "Precompiled.h"
#include "CubeDraw.h"
#include "WorldFrame.h"

namespace Graphics
{
	CubeChunkSectionPtr CubeDraw::pushNewChunk(uint32 type, const std::vector<CubeVertex>& newvertices) {
		auto itr = mVertices.find(type);
		if (itr == mVertices.end()) {
			mVertices.insert(std::make_pair(type, std::vector<CubeVertex>()));
			mSections.insert(std::make_pair(type, std::list<CubeChunkSectionPtr>()));
			mBuffers.insert(std::make_pair(type, sWorldFrame->getDevice()->createVertexBuffer()));
		}

		auto& vertices = mVertices[type];

		uint32 startIndex = vertices.size();
		uint32 endIndex = startIndex + newvertices.size();

		vertices.insert(vertices.end(), newvertices.begin(), newvertices.end());

		auto section = std::make_shared<CubeChunkSection>(type, startIndex, endIndex);

		mBuffers[type]->setData(vertices);

		return section;
	}

	void CubeDraw::removeChunk(CubeChunkSectionPtr chunk) {
		uint32 type = chunk->getBatchType();

		if (mSections.find(type) == mSections.end() || mVertices.find(type) == mVertices.end() || mBuffers.find(type) == mBuffers.end()) {
			return;
		}

		auto& sections = mSections[type];
		auto chunkItr = std::find(sections.begin(), sections.end(), chunk);
		if (chunkItr == sections.end()) {
			return;
		}

		sections.erase(chunkItr);

		auto& vertices = mVertices[type];
		auto itrStart = vertices.begin() + chunk->getStart();
		auto itrEnd = vertices.begin() + chunk->getEnd();

		vertices.erase(itrStart, itrEnd);

		for (auto& echunk : mSections[type]) {
			if (echunk->getStart() >= chunk->getStart()) {
				uint32 oldStart = echunk->getStart();
				oldStart -= (chunk->getEnd() - chunk->getStart());
				echunk->setStart(oldStart);
			}
		}

		mBuffers[type]->setData(vertices);
	}

	void CubeDraw::removeFromChunk(CubeChunkSectionPtr chunk, uint32 start, uint32 numVertices) {
		uint32 type = chunk->getBatchType();

		if (mSections.find(type) == mSections.end() || mVertices.find(type) == mVertices.end() || mBuffers.find(type) == mBuffers.end()) {
			return;
		}

		auto& sections = mSections[type];
		auto chunkItr = std::find(sections.begin(), sections.end(), chunk);
		if (chunkItr == sections.end()) {
			return;
		}

		uint32 chunkVertCount = chunk->getEnd() - chunk->getStart();
		if ((start + numVertices) > chunkVertCount || numVertices == 0) {
			throw std::invalid_argument("invalid chunk indices");
		}

		auto& vertices = mVertices[type];
		auto itrStart = vertices.begin() + chunk->getStart() + start;
		auto itrEnd = vertices.begin() + chunk->getStart() + start + numVertices;

		chunk->resize(chunk->getSize() - numVertices);

		vertices.erase(itrStart, itrEnd);

		for (auto& echunk : mSections[type]) {
			if (echunk->getStart() >= chunk->getStart()) {
				uint32 oldStart = echunk->getStart();
				oldStart -= numVertices;
				echunk->setStart(oldStart);
			}
		}
	}

	void CubeDraw::renderCubes() {

	}
}