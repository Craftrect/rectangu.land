#include "Precompiled.h"
#include "WorldFrame.h"
#include <RectanguLand.Content/AssetManager.h>
#include "SceneRoot.h"

INIT_SINGLETON(Graphics::WorldFrame);

namespace Graphics
{
	WorldFrame::WorldFrame() {
		mSsaoUsed = false;
		mDevice = Device::GxDevice::createDevice(ApiType::D3D11);
		mBufferMgr = std::make_shared<GlobalBufferManager>();
	}

	void WorldFrame::init(RenderTargetEntryPtr target) {
		mDevice->initDevice(target, false);

		mSceneManager = std::make_shared<SceneManager>();
		mSceneManager->setSceneRoot(std::make_shared<SceneRoot>());

		float bbwidth = boost::any_cast<float>(target->getProperty(RenderTargetEntry::Properties::Width));
		float bbheight = boost::any_cast<float>(target->getProperty(RenderTargetEntry::Properties::Height));

		mOrthoCamera = std::make_shared<OrthoCamera>(bbwidth, bbheight);
		mPerspCamera = std::make_shared<PerspCamera>(55.0f, bbwidth / bbheight);

		mBufferMgr->init(mDevice);

		// has to be called AFTER mBufferMgr->init to have any effect
		setActiveCamera(mPerspCamera);

		sAssets->initAssets(mDevice);

		mSsao.init(mDevice);
	}

	void WorldFrame::shutdown() {
		sAssets->shutdown();
	}

	void WorldFrame::onFrame() {
		mDevice->beginScene();

		mSsaoUsed = false;

		mSceneManager->render();

		if (mSsao.isCapturing() == true) {
			throw GraphicsException("Started capturing SSAO but never ended capturing until the end of the frame.");
		}

		if (mSsaoUsed == true) {
			mSsao.output();
		}

		mDevice->endScene();
		mDevice->present();
	}

	void WorldFrame::setActiveCamera(CameraPtr camera) {
		if (camera != mActiveCamera) {
			mActiveCamera = camera;
			mBufferMgr->cameraChanged(camera);
		}
	}

	void WorldFrame::captureSSAO() {
		mSsao.beginCapture();
		mSsaoUsed = true;
	}

	void WorldFrame::endCaptureSSAO() {
		mSsao.endCapture();
	}
}