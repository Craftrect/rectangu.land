#pragma once

#include "VertexElement.h"
#include "Objects/VertexBuffer.h"
#include "Objects/IndexBuffer.h"
#include "GeometryLayout.h"

namespace Graphics
{
	SHARED_FORWARD(Effect);
	SHARED_FORWARD(GeometryLayout);

	namespace Device
	{
		WEAK_FORWARD(GxDevice);
	}

	enum class InputLayout
	{
		Points,
		Triangles,
		TriangleStrip,
	};

	class InputGeometry
	{
	protected:
		std::vector<VertexElementPtr> mElements;
		uint32 mVertexCount;
		uint32 mTriangleCount;
		InputLayout mLayout;
		GeometryLayoutPtr mGeomLayout;

		std::vector<VertexBufferPtr> mVertexBuffers;
		IndexBufferPtr mIndexBuffer;

		EffectPtr mEffect;

		virtual bool isValidVertexBufferIndex(uint32 index) { return true; }
		virtual void onElementAdded(VertexElementPtr elem) { }

	public:
		InputGeometry();

		uint32 getVertexCount() const { return mVertexCount; }
		uint32 getTriangleCount() const { return mTriangleCount; }
		InputLayout getLayout() const { return mLayout; }
		const std::vector<VertexBufferPtr>& getVertexBuffers() const { return mVertexBuffers; }
		const std::vector<VertexElementPtr>& getVertexElements() const { return mElements; }
		IndexBufferPtr getIndexBuffer() const { return mIndexBuffer; }
		EffectPtr getEffect() const { return mEffect; }

		GeometryLayoutPtr getGeometryLayout() const;

		void clearVertexBuffers();
		void addVertexBuffer(VertexBufferPtr buffer);
		void setVertexBuffer(uint32 index, VertexBufferPtr buffer);

		void setIndexBuffer(IndexBufferPtr ibuff);

		void setEffect(EffectPtr effect);

		void setVertexCount(uint32 vertices);
		void setTriangleCount(uint32 numTriangles);

		void setInputLayout(InputLayout layout);
		
		void addVertexElement(VertexElementPtr elem);

		uint32 getIndexCount() const;

		virtual void finalize(Device::GxDeviceWeakPtr device);
	};

	SHARED_TYPE(InputGeometry);
}