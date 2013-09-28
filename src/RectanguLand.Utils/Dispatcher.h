#pragma once

namespace Utils
{
	SHARED_FORWARD(Dispatcher);

	enum class DispatcherPriority
	{
		Background = 3,
		Low = 2,
		Normal = 1,
		High = 0,
	};

	class Dispatcher
	{
		static DispatcherPtr gMainDispatcher;

		class DispatcherWorkItem
		{
		private:
			DispatcherPriority mPriority;
			std::function<void ()> mWorkItem;

		public:
			DispatcherWorkItem(DispatcherPriority prio, std::function < void ()> callback) {
				mPriority = prio;
				mWorkItem = callback;
			}

			bool operator < (const DispatcherWorkItem& other) const {
				return (uint32) mPriority < (uint32) other.mPriority;
			}

			void execute() { mWorkItem(); }
		};

		std::mutex mItemLock;
		std::priority_queue<DispatcherWorkItem> mQueue;

	public:
		Dispatcher();

		void runLoop(std::function<bool ()> messageCallback, std::function<bool ()> idleCallback);
		void beginInvoke(std::function<void ()> callback, DispatcherPriority prio = DispatcherPriority::Normal);

		static DispatcherPtr main() { return gMainDispatcher; }
	};
}