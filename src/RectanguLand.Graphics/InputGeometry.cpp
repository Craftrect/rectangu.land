#include "Precompiled.h"
#include "InputGeometry.h"
#include "GeometryLayoutD3D11.h"

namespace Graphics
{
	InputGeometry::InputGeometry() {
		mLayout = InputLayout::Triangles;
		mVertexCount = 0;
		mTriangleCount = 0;
		mIndexBuffer = nullptr;
	}

	void InputGeometry::clearVertexBuffers() {
		mVertexBuffers.clear();
	}

	void InputGeometry::addVertexBuffer(VertexBufferPtr vbuff) {
		mVertexBuffers.push_back(vbuff);
	}

	void InputGeometry::setVertexBuffer(uint32 index, VertexBufferPtr vbuff) {
		if (index > mVertexBuffers.size()) {
			throw std::invalid_argument("index for vertex buffer out of range");
		}

		if (isValidVertexBufferIndex(index) == false) {
			throw std::invalid_argument("index for vertex buffer out of GPU range");
		}

		if (index == mVertexBuffers.size()) {
			addVertexBuffer(vbuff);
		} else {
			mVertexBuffers[index] = vbuff;
		}
	}

	void InputGeometry::setInputLayout(InputLayout layout) {
		mLayout = layout;
	}

	void InputGeometry::setTriangleCount(uint32 triangles) {
		mTriangleCount = triangles;
	}

	void InputGeometry::setVertexCount(uint32 vcount) {
		mVertexCount = vcount;
	}

	void InputGeometry::addVertexElement(VertexElementPtr elem) {
		mElements.push_back(elem);

		onElementAdded(elem);
	}

	void InputGeometry::setIndexBuffer(IndexBufferPtr ibuff) {
		mIndexBuffer = ibuff;
	}

	void InputGeometry::setEffect(EffectPtr effect) {
		mEffect = effect;
	}

	uint32 InputGeometry::getIndexCount() const {
		switch (mLayout) {
		case InputLayout::Points:
			return mTriangleCount;

		case InputLayout::Triangles:
			return mTriangleCount * 3;

		case InputLayout::TriangleStrip:
			return mTriangleCount + 2;

		default:
			return 0;
		}
	}

	void InputGeometry::finalize(Device::GxDeviceWeakPtr device) {
		auto dev = device.lock();

		switch (dev->getType()) {
		case ApiType::D3D11:
			{
				mGeomLayout = std::make_shared<GeometryLayoutD3D11>();
				mGeomLayout->compile(device, mEffect, mElements);
			}
			break;

		default:
			throw std::invalid_argument("Api type for input geometry not supported.");
		}
	}

	GeometryLayoutPtr InputGeometry::getGeometryLayout() const {
		return mGeomLayout;
	}
}