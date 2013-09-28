#include "Precompiled.h"
#include "GlobalBufferManager.h"
#include "WorldFrame.h"

namespace Graphics
{
	void GlobalBufferManager::init(Device::GxDevicePtr device) {
		auto buffMat = device->createBuffer(BufferType::Const);

		buffMat->resize(sizeof(MatrixBuffer));
		buffMat->setBufferData(mMatrixBuffer);

		mBuffers[ShaderVariables::cbuff_Matrix] = buffMat;
		
		auto eventOrtho = std::bind(&GlobalBufferManager::cameraMatrixChanged, this, sWorldFrame->getOrthoCamera(), std::placeholders::_1);
		auto eventPersp = std::bind(&GlobalBufferManager::cameraMatrixChanged, this, sWorldFrame->getPerspectivicCamera(), std::placeholders::_1);

		sWorldFrame->getOrthoCamera()->ProjChanged += eventOrtho;
		sWorldFrame->getOrthoCamera()->ViewChanged += eventOrtho;

		sWorldFrame->getPerspectivicCamera()->ProjChanged += eventPersp;
		sWorldFrame->getPerspectivicCamera()->ViewChanged += eventPersp;
	}

	void GlobalBufferManager::cameraChanged(CameraPtr camera) {
		auto itr = mBuffers.find(ShaderVariables::cbuff_Matrix);
		if (itr == mBuffers.end()) {
			return;
		}

		camera->getProjection().transposed(mMatrixBuffer.Projection);
		camera->getView().transposed(mMatrixBuffer.View);

		itr->second->setBufferData(mMatrixBuffer);
	}

	void GlobalBufferManager::cameraMatrixChanged(CameraPtr camera, const Utils::Matrix& mat) {
		if (camera != sWorldFrame->getActiveCamera()) {
			return;
		}

		auto itr = mBuffers.find(ShaderVariables::cbuff_Matrix);
		if (itr == mBuffers.end()) {
			return;
		}

		camera->getProjection().transposed(mMatrixBuffer.Projection);
		camera->getView().transposed(mMatrixBuffer.View);

		itr->second->setBufferData(mMatrixBuffer);
	}

	void GlobalBufferManager::attachToEffect(EffectPtr eff)  {
		for (uint32 i = 0; i < (uint32) ShaderCodeType::MAX; ++i) {
			eff->getParamManager((ShaderCodeType) i)->setBuffer(ShaderVariables::cbuff_Matrix, mBuffers[ShaderVariables::cbuff_Matrix]);
		}
	}
}