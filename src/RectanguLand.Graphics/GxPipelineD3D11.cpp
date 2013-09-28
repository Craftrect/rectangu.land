#include "Precompiled.h"
#include "GxPipelineD3D11.h"
#include "Objects/IndexBufferD3D11.h"
#include "Objects/VertexBufferD3D11.h"
#include "Effect.h"
#include "GeometryLayoutD3D11.h"

namespace Graphics
{
	GxPipelineD3D11::GxPipelineD3D11(Device::GxDeviceD3D11WeakPtr device) {
		mDevice = device;
	}

	void GxPipelineD3D11::execute(InputGeometryPtr geometry) {
		if (geometry->getEffect() == nullptr) {
			throw std::invalid_argument("geometry missing effect");
		}

		if (geometry->getVertexCount() == 0 || geometry->getTriangleCount() == 0) {
			return;
		}

		if (geometry->getGeometryLayout() && geometry->getGeometryLayout()->getApiType() != ApiType::D3D11) {
			throw std::invalid_argument("invalid api type for geometry layout");
		}

		if (geometry->getVertexBuffers().size() > D3D11_IA_VERTEX_INPUT_RESOURCE_SLOT_COUNT) {
			throw std::invalid_argument("too many vertex buffers.");
		}

		if (geometry->getIndexBuffer() != nullptr) {
			if (geometry->getIndexBuffer()->getApiType() != ApiType::D3D11) {
				throw std::invalid_argument("index buffer api doesnt match pipeline api (D3D11)");
			}
		}
		
		auto ibuff = geometry->getIndexBuffer();

		ID3D11Buffer* indexBuff = ibuff != nullptr ? std::static_pointer_cast<IndexBufferD3D11>(ibuff)->getBufferPtr() : nullptr;

		auto context = mDevice.lock()->getImmContext();
		auto device = mDevice.lock()->getDevice();

		context->IASetIndexBuffer(indexBuff, ibuff ? mapIndexFormat(ibuff->getIndexType()) : DXGI_FORMAT_UNKNOWN, 0);

		static ID3D11Buffer* vertexBuffers[D3D11_IA_VERTEX_INPUT_RESOURCE_SLOT_COUNT];
		static uint32 vertexBufferStrides[D3D11_IA_VERTEX_INPUT_RESOURCE_SLOT_COUNT];
		static uint32 vertexBufferOffsets[D3D11_IA_VERTEX_INPUT_RESOURCE_SLOT_COUNT];

		if (geometry->getVertexBuffers().size() == 0) {
			memset(vertexBuffers, 0, D3D11_IA_VERTEX_INPUT_RESOURCE_SLOT_COUNT * sizeof(ID3D11Buffer*));
			memset(vertexBufferStrides, 0, D3D11_IA_VERTEX_INPUT_RESOURCE_SLOT_COUNT * sizeof(uint32));
			memset(vertexBufferOffsets, 0, D3D11_IA_VERTEX_INPUT_RESOURCE_SLOT_COUNT * sizeof(uint32));
		} else {
			for (uint32 i = 0; i < geometry->getVertexBuffers().size(); ++i) {
				vertexBufferStrides[i] = geometry->getVertexBuffers()[i]->getStride();
				vertexBuffers[i] = std::static_pointer_cast<VertexBufferD3D11>(geometry->getVertexBuffers()[i])->getBufferPtr();
			}

			uint32 numRemain = D3D11_IA_VERTEX_INPUT_RESOURCE_SLOT_COUNT - geometry->getVertexBuffers().size();

			memset(vertexBuffers + geometry->getVertexBuffers().size(), 0, numRemain * sizeof(ID3D11Buffer*));
			memset(vertexBufferStrides + geometry->getVertexBuffers().size(), 0, numRemain * sizeof(uint32));
			memset(vertexBufferOffsets, 0, D3D11_IA_VERTEX_INPUT_RESOURCE_SLOT_COUNT * sizeof(uint32));
		}

		context->IASetVertexBuffers(0, D3D11_IA_VERTEX_INPUT_RESOURCE_SLOT_COUNT, vertexBuffers, vertexBufferStrides, vertexBufferOffsets);

		switch (geometry->getLayout()) {
		case InputLayout::Triangles:
			context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
			break;

		case InputLayout::Points:
			context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);
			break;

		case InputLayout::TriangleStrip:
			context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
			break;

		default:
			throw std::invalid_argument("Invalid input layout");
		}

		auto geomLayout = std::static_pointer_cast<GeometryLayoutD3D11>(geometry->getGeometryLayout());

		if (geomLayout != nullptr) {
			context->IASetInputLayout(geomLayout->getLayout());
		} else {
			context->IASetInputLayout(nullptr);
		}

		geometry->getEffect()->begin();

		if (ibuff != nullptr) {
			context->DrawIndexed(geometry->getIndexCount(), 0, 0);
		} else {
			context->Draw(geometry->getVertexCount(), 0);
		}

		geometry->getEffect()->end();
	}

	void GxPipelineD3D11::disableDepth() {

	}

	void GxPipelineD3D11::enableDepth() {

	}

	void GxPipelineD3D11::setBlend(BlendMode src, BlendMode dest) {

	}

	DXGI_FORMAT GxPipelineD3D11::mapIndexFormat(IndexType type) {
		switch (type) {
		case IndexType::Short:
			return DXGI_FORMAT_R16_UINT;

		case IndexType::Int:
			return DXGI_FORMAT_R32_UINT;

		default:
			throw std::invalid_argument("invalid index type");
		}
	}
};