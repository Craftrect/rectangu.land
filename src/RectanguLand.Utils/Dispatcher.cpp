#include "PublicInclude.h"
#include "Dispatcher.h"

namespace Utils
{
	DispatcherPtr Dispatcher::gMainDispatcher = std::make_shared<Dispatcher>();

	Dispatcher::Dispatcher() {

	}

	void Dispatcher::runLoop(std::function<bool ()> messageCallback, std::function<bool ()> idleCallback) {
		MSG msg;
		bool isUp = true;
		Stopwatch sw;
		while (isUp) {

			sw.start();
			if (PeekMessage(&msg, nullptr, 0, 0, PM_NOREMOVE)) {
				isUp = messageCallback();
				if (isUp == false) {
					break;
				}
			}

			isUp = idleCallback();

			std::lock_guard<std::mutex> l(mItemLock);
			uint32 itemsProcessed = 0;

			if (mQueue.empty() == false) {
				do {
					auto& item = mQueue.top();
					item.execute();
					mQueue.pop();
					sw.stop();
					++itemsProcessed;

				} while (mQueue.empty() == false && sw.getMilliseconds() < 33 && itemsProcessed < 25);
			}
			Sleep(10);
		}
	}

	void Dispatcher::beginInvoke(std::function<void ()> callback, DispatcherPriority prio) {
		std::lock_guard<std::mutex> l(mItemLock);

		DispatcherWorkItem item(prio, callback);
		mQueue.push(item);
	}
}