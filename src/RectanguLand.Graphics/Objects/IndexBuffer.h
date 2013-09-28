#pragma once

#include "ApiObject.h"

namespace Graphics
{
	enum IndexType
	{
		Short,
		Int
	};

	class IndexBuffer : public ApiObject
	{
	protected:
		IndexType mIndexType;

		virtual void setData(const void* data, uint32 numBytes) = 0;

		IndexBuffer(ApiType type);
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
		void setData(const T(&data)[size]) {
			setData(data, size * sizeof(T));
		}

		void setIndexType(IndexType type) { mIndexType = type; }
		IndexType getIndexType() const { return mIndexType; }
	};

	SHARED_TYPE(IndexBuffer);
}