#include "Precompiled.h"
#include "VertexElement.h"

namespace Graphics
{
	VertexElement::VertexElement(Semantic semantic, uint32 componentCount, uint32 index, ComponentType type) :
		mSemantic(semantic), mIndex(index), mComponentCount(componentCount), mType(type)
	{

	}
}