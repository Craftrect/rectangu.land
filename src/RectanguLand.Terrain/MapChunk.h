#pragma once

namespace Terrain
{
	enum class BlockType : uint16
	{
		Invalid,
		Debug,
		Dirt,
		Stone,
	};

	class MapChunk : public Graphics::SceneNode
	{
	private:
		class CubeRow
		{
			uint32 mHeight;
			std::vector<BlockType> mTypes;

		public:
			void setHeight(uint32 height) { mHeight = height; mTypes.resize(height); }
			uint32 getHeight() const { return mHeight; }

			void setBlockType(uint32 index, BlockType type) { mTypes[index] = type; }
			BlockType getType(uint32 index) const { return mTypes[index]; }
		};

		struct CubeVertex
		{
			float x, y, z, unused;
			float occlusions[8];

			CubeVertex() : x(0), y(0), z(0), unused(1) {

			}

			CubeVertex(float _x, float _y, float _z) : x(_x), y(_y), z(_z), unused(1) {

			}
		};

		std::array<CubeRow, 256> mRows;

		uint32 mIndexX, mIndexY;
		bool mIsAsyncLoaded;
		bool mIsSyncLoaded;
		bool mVerticesDirty;

		std::vector<uint32> mIndices;
		std::vector<CubeVertex> mVertices;
	
		Graphics::VertexBufferPtr mVertexBuffer;
		Graphics::IndexBufferPtr mVisibleIndices;

		static Graphics::InputGeometryPtr mCubeGeometry;

		void initGeometry();

		void fillVisibleBlocks();

		float getOcclusion(bool side1, bool side2, bool corner);

		void bakeOcclusion();

	public:
		MapChunk(uint32 indexX, uint32 indexY);
		void generate();
		void saveToFile(const String& file);

		void asyncLoad();
		void syncLoad();
		void asyncUpdateLight();

		void render();
		void update();

		uint32 getIndexX() const { return mIndexX; }
		uint32 getIndexY() const { return mIndexY; }
		uint32 getHeight(int32 x, int32 y) { return mRows[y * 16 + x].getHeight(); }
	};

	SHARED_TYPE(MapChunk);
}