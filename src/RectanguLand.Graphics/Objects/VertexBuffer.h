#pragma once

#include "ApiObject.h"

namespace Graphics
{
	class VertexBuffer : public ApiObject
	{
	protected:
		uint32 mStride;

		virtual void setData(const void* data, uint32 numBytes) = 0;

		VertexBuffer(ApiType type) : ApiObject(type) { }
	public:
		template<typename T>
		void setData(const std::vector<T>& data) {
			setData(data.data(), data.size() * sizeof(T));
		}

		template<typename _FwdItr>
		void setData(_FwdItr begin, _FwdItr end) {
			setData(&(*begin), (end - begin) * sizeof(_FwdItr::value_type))
		}

		template<typename T, uint32 size>
		void setData(const T (&data)[size]) {
			setData(data, size * sizeof(T));
		}

		virtual void setStride(uint32 stride) { mStride = stride; }
		virtual uint32 getStride() const { return mStride; }
	};

	SHARED_TYPE(VertexBuffer);
}