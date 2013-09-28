#include "Precompiled.h"
#include "GeometryLayoutD3D11.h"
#include "GxDeviceD3D11.h"

namespace Graphics
{
	GeometryLayoutD3D11::GeometryLayoutD3D11() : GeometryLayout(ApiType::D3D11) {

	}

	void GeometryLayoutD3D11::compile(Device::GxDeviceWeakPtr device, EffectPtr effect, std::vector<VertexElementPtr>& elements) {
		if (device.lock()->getType() != ApiType::D3D11) {
			throw std::invalid_argument("Invalid api type for geometry layout (device is not d3d11)");
		}

		auto shader = effect->getShader(ShaderCodeType::Vertex);
		if (shader == nullptr) {
			throw std::invalid_argument("Shader has no vertex shader, cannot compile a geometry layout for it.");
		}

		mInputElements.resize(elements.size());
		for (uint32 i = 0; i < elements.size(); ++i) {
			auto& curElem = elements[i];

			D3D11_INPUT_ELEMENT_DESC elemDesc = { 0 };
			elemDesc.AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
			
			DXGI_FORMAT fmt = calcFormat(curElem->getComponentCount(), curElem->getComponentType());
			elemDesc.Format = fmt;
			elemDesc.InputSlot = 0;
			elemDesc.InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
			elemDesc.InstanceDataStepRate = 0;
			elemDesc.SemanticIndex = curElem->getSemanticIndex();

			switch (curElem->getSemantic()) {
			case Semantic::Normal:
				elemDesc.SemanticName = "NORMAL";
				break;

			case Semantic::Position:
				elemDesc.SemanticName = "POSITION";
				break;

			case Semantic::TexCoord:
				elemDesc.SemanticName = "TEXCOORD";
				break;

			default:
				throw std::invalid_argument("Invalid semantic type for vertex element(s) found.");
			}

			mInputElements[i] = elemDesc;
		}

		auto dev11 = std::static_pointer_cast<Device::GxDeviceD3D11>(device.lock());
		
		auto result = dev11->getDevice()->CreateInputLayout(mInputElements.data(), mInputElements.size(), shader->getShaderCode()->getShaderData().data(),
			shader->getShaderCode()->getShaderData().size(), mLayout);

		if (FAILED(result)) {
			throw std::bad_function_call("Unable to create input layout, d3d11 failed.");
		}
	}

	DXGI_FORMAT GeometryLayoutD3D11::calcFormat(uint32 componentCount, ComponentType type) {
		switch (type) {
		case ComponentType::Float:
			{
				switch (componentCount) {
				case 4:
					return DXGI_FORMAT_R32G32B32A32_FLOAT;

				case 3:
					return DXGI_FORMAT_R32G32B32_FLOAT;

				case 2:
					return DXGI_FORMAT_R32G32_FLOAT;

				case 1:
					return DXGI_FORMAT_R32_FLOAT;

				default:
					throw std::invalid_argument("Invalid float attribute format value: 0 < componentCount <= 4");
				}
			}
			break;

		case ComponentType::Byte:
			{
				switch (componentCount)
				{
				case 4:
					return DXGI_FORMAT_R8G8B8A8_UNORM;

				case 2:
					return	DXGI_FORMAT_R8G8_UNORM;

				case 1:
					return DXGI_FORMAT_R8_UNORM;

				default:
					throw std::invalid_argument("Invalid byte attribute value: Allowed values are 1, 2, 4");
				}
			}
			break;

		default:
			throw std::invalid_argument("Invalid component type, allowed values are float and byte");
		}
	}
}