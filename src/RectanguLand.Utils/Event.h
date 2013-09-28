#pragma once

namespace Utils
{
	template<typename T>
	class Event
	{

	};

	template<typename Res, typename... Args>
	class Event<Res(Args...)>
	{
	public:
		class EventDelegate
		{
			friend class Event<Res (Args...)>;
			std::function<Res (Args...)> mCallback;
			uint64 mToken;
		public:
			EventDelegate() : mToken(~(uint64) 0) {

			}

			EventDelegate(uint64 token, std::function<Res (Args...)> callback) : mToken(token), mCallback(callback) {

			}

			std::function<Res (Args...)> getCallback() const { return mCallback; }
			uint64 getToken() const { return mToken; }
		};

	private:
		std::map<uint64, EventDelegate> mDelegates;
		std::mutex mEventLock;
		uint64 mLastToken;

		std::queue<uint64> mTokenQueue;

		Event(const Event&) { }
		void operator = (const Event&) { }

	public:
		Event() : mLastToken(0) {

		}

		void clearListeners() {
			std::lock_guard<std::mutex> l(mEventLock);
			mDelegates.clear();
		}

		EventDelegate operator += (std::function<Res (Args...)> callback) {
			std::lock_guard<std::mutex> l(mEventLock);

			uint64 token = 0;
			if (mTokenQueue.size() > 0) {
				uint64 value = mTokenQueue.front();
				mTokenQueue.pop();
			} else {
				token = mLastToken++;
			}

			EventDelegate del(token, callback);

			mDelegates[token] = del;
			return del;
		}

		void operator -= (const EventDelegate& del) {
			std::lock_guard<std::mutex> l(mEventLock);

			auto itr = mDelegates.find(del.getToken());
			if (itr == mDelegates.end()) {
				return;
			}

			mDelegates.erase(itr);
		}

		void operator () (const Args&... args) {
			std::lock_guard<std::mutex> l(mEventLock);

			for (auto& pair : mDelegates) {
				pair.second.getCallback()(args...);
			}
		}
	};
}