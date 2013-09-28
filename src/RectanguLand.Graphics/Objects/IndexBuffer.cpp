#include "Precompiled.h"
#include "IndexBuffer.h"

namespace Graphics
{
	IndexBuffer::IndexBuffer(ApiType type) : ApiObject(type) {
		mIndexType = IndexType::Int;
	}
}