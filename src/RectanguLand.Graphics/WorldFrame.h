#pragma once

#include "GxDevice.h"
#include "SceneManager.h"
#include "Camera.h"
#include "OrthoCamera.h"
#include "PerspCamera.h"
#include "GlobalBufferManager.h"
#include "SSAO.h"

namespace Graphics
{
	class WorldFrame : public Singleton<WorldFrame>
	{
		Device::GxDevicePtr mDevice;
		SceneManagerPtr mSceneManager;

		CameraPtr mActiveCamera;
		PerspCameraPtr mPerspCamera;
		OrthoCameraPtr mOrthoCamera;
		GlobalBufferManagerPtr mBufferMgr;

		SSAO mSsao;
		bool mSsaoUsed;

	public:
		WorldFrame();
		void init(RenderTargetEntryPtr target);
		void shutdown();

		void onFrame();

		void captureSSAO();
		void endCaptureSSAO();

		CameraPtr getActiveCamera() const { return mActiveCamera; }
		PerspCameraPtr getPerspectivicCamera() const { return mPerspCamera; }
		OrthoCameraPtr getOrthoCamera() const { return mOrthoCamera; }

		void setActiveCamera(CameraPtr camera);

		Device::GxDevicePtr getDevice() const { return mDevice; }

		GlobalBufferManagerPtr getGlobalBuffers() const { return mBufferMgr; }

		SceneManagerPtr getSceneManager() const { return mSceneManager; }
	};
}

#define sWorldFrame (Graphics::WorldFrame::getInstance())