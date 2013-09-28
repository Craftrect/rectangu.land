#pragma once

namespace Graphics
{
	enum class Semantic
	{
		Position,
		TexCoord,
		Normal
	};

	enum class ComponentType
	{
		Float,
		Byte
	};

	class VertexElement
	{
	private:
		Semantic mSemantic;
		uint32 mComponentCount;
		ComponentType mType;
		uint32 mIndex;

	public:
		VertexElement(Semantic semantic, uint32 componentCount, uint32 index = 0, ComponentType = ComponentType::Float);

		Semantic getSemantic() const { return mSemantic; }
		uint32 getComponentCount() const { return mComponentCount; }
		ComponentType getComponentType() const { return mType; }
		uint32 getSemanticIndex() const { return mIndex; }
	};

	SHARED_TYPE(VertexElement);
}