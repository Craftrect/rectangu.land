#include "Precompiled.h"
#include "Win32/Window.h"

BOOL WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR cmdLine, INT nShowCmd) {
	auto wnd = std::make_shared<Win32::Window>();
	sIoMgr->init(wnd);

	uint64 night = 'Nigh';
	night <<= 8;
	night |= 't';
	night = ~night;

	wnd->setVisible(true);

	sWorldFrame->init(wnd);

	sWorldManager->enterWorld(0, 0);

	sWorldFrame->getSceneManager()->getRoot()->addChild(sWorldManager);

	Utils::Dispatcher::main()->runLoop(
		[wnd]() {
			wnd->parseMessages();
			return !wnd->isQuit();
		},

		[]() {
			auto curState = sIoMgr->getKeyboard()->getState();
			BYTE keyState[256];
			GetKeyboardState(keyState);
			if (curState->isKeyPressed('w')) {
				sWorldFrame->getActiveCamera()->move(sWorldFrame->getActiveCamera()->getForward(), 0.1f);
			} 
			
			if (keyState['S'] & 0x80) {
				sWorldFrame->getActiveCamera()->move(sWorldFrame->getActiveCamera()->getForward(), -0.1f);
			} 
			
			if (keyState['A'] & 0x80)  {
				sWorldFrame->getActiveCamera()->move(sWorldFrame->getActiveCamera()->getRight(), -0.1f);
			} 
			
			if (keyState['D'] & 0x80)  {
				sWorldFrame->getActiveCamera()->move(sWorldFrame->getActiveCamera()->getRight(), 0.1f);
			} 
			
			if (keyState['Q'] & 0x80)  {
				sWorldFrame->getActiveCamera()->move(Utils::Vector3::UnitZ, 0.1f);
			} 
			
			if (keyState['E'] & 0x80)  {
				sWorldFrame->getActiveCamera()->move(Utils::Vector3::UnitZ, -0.1f);
			} 
			
			if (keyState['X'] & 0x80) {
				if (keyState[VK_SHIFT] & 0x80) {
					sWorldFrame->getActiveCamera()->yaw(1.0f);
				} else {
					sWorldFrame->getActiveCamera()->yaw(-1.0f);
				}
			} 
			
			if (keyState[' '] & 0x80) {
				if (keyState[VK_SHIFT] & 0x80) {
					sWorldFrame->getActiveCamera()->pitch(1.0f);
				} else {
					sWorldFrame->getActiveCamera()->pitch(-1.0f);
				}
			}
			sWorldFrame->onFrame();
			return true;
		}
	);

	sWorldFrame->shutdown();

	return 0;
}