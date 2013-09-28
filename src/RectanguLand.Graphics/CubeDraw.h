#pragma once

#include "Objects/VertexBuffer.h"
#include "InputGeometry.h"

namespace Graphics
{
	struct CubeVertex
	{
		CubeVertex() : x(0), y(0), z(0), unused(1) {

		}

		CubeVertex(float x, float y, float z) {
			this->x = x;
			this->y = y;
			this->z = z;

			unused = 1;
		}

		float x, y, z, unused;
	};

	class CubeChunkSection
	{
	private:
		uint32 mBatchType;
		uint32 mStartIndex;
		uint32 mEndIndex;
	public:
		CubeChunkSection(uint32 type, uint32 startIndex, uint32 endIndex) {
			mBatchType = type;
			mStartIndex = startIndex;
			mEndIndex = endIndex;
		}

		void setStart(uint32 start) {
			mStartIndex = start;
		}

		void resize(uint32 newSize) {
			mEndIndex = mStartIndex + newSize;
		}

		uint32 getSize() const {
			return mEndIndex - mStartIndex;
		}

		uint32 getBatchType() const {
			return mBatchType;
		}

		uint32 getStart() const {
			return mStartIndex;
		}

		uint32 getEnd() const {
			return mEndIndex;
		}
	};

	SHARED_TYPE(CubeChunkSection);

	class CubeDraw : public Singleton<CubeDraw>
	{
	private:
		std::unordered_map<uint32, std::list<CubeChunkSectionPtr>> mSections;
		std::unordered_map<uint32, std::vector<CubeVertex>> mVertices;
		std::unordered_map<uint32, VertexBufferPtr> mBuffers;

		InputGeometryPtr mCubeGeometry;

	public:
		CubeChunkSectionPtr pushNewChunk(uint32 type, const std::vector<CubeVertex>& vertices);
		void removeChunk(CubeChunkSectionPtr chunk);
		void removeFromChunk(CubeChunkSectionPtr chunk, uint32 startVertex, uint32 numVertices);

		void renderCubes();
	};
}