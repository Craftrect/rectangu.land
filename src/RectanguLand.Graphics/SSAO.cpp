#include "Precompiled.h"
#include "SSAO.h"
#include <RectanguLand.Content/AssetManager.h>
#include "WorldFrame.h"

namespace Graphics
{
	SSAO::SSAO() : mIsCapturing(false) {

	}

	void SSAO::init(Device::GxDevicePtr device) {
		mSsaoTarget = device->createRenderTarget(RenderTargetType::RGBA);
		mBufferTarget = device->createRenderTarget(RenderTargetType::Float4);
		mOcclusionTarget = device->createRenderTarget(RenderTargetType::Float4);
		mDevice = device;

		mRandTexture = device->createFloat4Texture(4, 4);
		std::vector<Utils::Vector4> noise(16);
		for (uint32 i = 0; i < 16; ++i) {
			Utils::Vector3 cnoise((float) sRandom->nextDouble(-1, 1), (float) sRandom->nextDouble(-1, 1), 0);
			cnoise.normalize();

			noise[i] = Utils::Vector4(cnoise, 0);
		}

		mRandTexture->updateData(noise);

		mGeometry = std::make_shared<InputGeometry>();
		mGeometry->setEffect(sAssets->getEffect(Content::Effects::SSAO));
		mEffect = mGeometry->getEffect();
		mEffectPass2 = sAssets->getEffect(Content::Effects::SSAO2);

		mGeometry->setIndexBuffer(nullptr);
		mGeometry->setVertexCount(6);
		mGeometry->setTriangleCount(2);
		mGeometry->setInputLayout(InputLayout::Triangles);

		mInfoBuffer = device->createBuffer(BufferType::Const);
		mInfoBuffer->resize(sizeof(SSAOBuffer));

		uint32 bbw, bbh;
		device->getBackbufferDimension(bbw, bbh);

		mSSAOInfo.noiseScaleX = bbw / 4.0f;
		mSSAOInfo.noiseScaleY = bbh / 4.0f;

		initKernel();

		mSSAOInfo.invProj = sWorldFrame->getActiveCamera()->getInvProj();
		mSSAOInfo.proj = sWorldFrame->getActiveCamera()->getProjection();
		mSSAOInfo.radius = 10.69f;

		mInfoBuffer->setBufferData(mSSAOInfo);

		mEffect->getParamManager(ShaderCodeType::Pixel)->setBuffer(0, mInfoBuffer);

		sWorldFrame->getPerspectivicCamera()->ProjChanged += std::bind(&SSAO::matrixChanged, this, sWorldFrame->getPerspectivicCamera(), std::placeholders::_1);
		sWorldFrame->getOrthoCamera()->ProjChanged += std::bind(&SSAO::matrixChanged, this, sWorldFrame->getOrthoCamera(), std::placeholders::_1);
	}

	void SSAO::initKernel() {
		for (uint32 i = 0; i < KernelSize; ++i) {
			Utils::Vector3 v((float) sRandom->nextDouble(-1, 1), (float) sRandom->nextDouble(-1, 1), (float) sRandom->nextDouble(0, 1));
			v.normalize();

			v *= (float) sRandom->nextDouble(0, 1);
			float scale = (float) i / (float) KernelSize;
			float fac = scale * scale;
			scale = 0.1f + fac * 0.9f;

			v *= scale;
			mSSAOInfo.sampleKernel[i] = Utils::Vector4(v, 0);
		}
	}

	void SSAO::beginCapture() {
		/*auto dev = mDevice.lock();
		if (dev == nullptr) {
			throw GraphicsException("SSAO not initialized yet");
		}

		mSsaoTarget->clear(Utils::Vector4(0, 0, 0, 0));
		mBufferTarget->clear(Utils::Vector4(0, 0, 0, 0));
		mOcclusionTarget->clear(Utils::Vector4(1, 1, 1, 1));

		mIsCapturing = true;

		dev->setRenderTarget(0, mSsaoTarget);
		dev->setRenderTarget(1, mBufferTarget);*/
	}

	void SSAO::endCapture() {
		/*auto dev = mDevice.lock();
		if (dev == nullptr) {
			throw GraphicsException("SSAO not initialized yet");
		}

		mIsCapturing = false;

		dev->removeRenderTarget(0);
		dev->removeRenderTarget(1);*/
	}

	void SSAO::output() {
		/*mEffect->getParamManager(ShaderCodeType::Pixel)->setTexture(0, mBufferTarget);
		mEffect->getParamManager(ShaderCodeType::Pixel)->setTexture(1, mRandTexture);

		auto dev = mDevice.lock();
		dev->disableDepthWrite();

		mGeometry->setEffect(mEffect);

		dev->setRenderTarget(0, mOcclusionTarget);

		dev->getPipeline()->execute(mGeometry);

		dev->clearCustomRenderTargets();

		mGeometry->setEffect(mEffectPass2);

		mEffectPass2->getParamManager(ShaderCodeType::Pixel)->setTexture(0, mSsaoTarget);
		mEffectPass2->getParamManager(ShaderCodeType::Pixel)->setTexture(1, mOcclusionTarget);

		dev->getPipeline()->execute(mGeometry);

		dev->enableDepthWrite();*/
	}

	void SSAO::matrixChanged(CameraPtr camera, const Utils::Matrix& proj) {
		if (camera != sWorldFrame->getActiveCamera()) {
			return;
		}

		mSSAOInfo.proj = proj;
		mSSAOInfo.invProj = camera->getInvProj();

		mInfoBuffer->setBufferData(mSSAOInfo);
	}
}