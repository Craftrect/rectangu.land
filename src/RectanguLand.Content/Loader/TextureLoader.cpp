#include "Precompiled.h"
#include "TextureLoader.h"

namespace Content
{
	TextureLoader::TextureLoader(uint32 threadCount) {
		if (threadCount == 0) {
			throw std::invalid_argument("TextureLoader cannot work with 0 loading threads.");
		}

		for (uint32 i = 0; i < threadCount; ++i) {
			TextureFetchThreadPtr fetcher = std::make_shared<TextureFetchThread>();
			mThreads.push_back(fetcher);
		}
	}

	void TextureLoader::shutdownThreads() {
		for (auto& thread : mThreads) {
			thread->shutdown();
		}
	}

	void TextureLoader::postWorkItem(TexturePayloadData& workItem) {
		auto lazyThread = std::min_element(mThreads.begin(), mThreads.end(),
			[](TextureFetchThreadPtr t1, TextureFetchThreadPtr t2) {
				return t1->getQueueSize() < t2->getQueueSize();
			}
		);

		(*lazyThread)->postWorkItem(workItem);
	}

	TextureLoader::TextureFetchThread::TextureFetchThread() {
		mIsRunning = true;
		mLoadThread = std::thread(std::bind(&TextureFetchThread::threadCallback, this));
	}

	void TextureLoader::TextureFetchThread::postWorkItem(TexturePayloadData& workItem) {
		std::lock_guard<std::mutex> l(mQueueLock);
		mWorkItems.push(workItem);
	}

	void TextureLoader::TextureFetchThread::threadCallback() {
		while (mIsRunning == true) {
			{
				mQueueLock.lock();

				if (mWorkItems.size() > 0) {
					TexturePayloadData data = mWorkItems.front();
					mWorkItems.pop();
					mQueueLock.unlock();

					try {
						ImageLoaderPtr loader = std::make_shared<ImageLoader>(data.getStream());
						data.getCallback()(loader);
					} catch (std::exception&) {
						data.getCallback()(nullptr);
					}
				} else {
					mQueueLock.unlock();
				}

				std::this_thread::sleep_for(std::chrono::milliseconds(30));
			}
		}
	}

	void TextureLoader::TextureFetchThread::shutdown() {
		mIsRunning = false;
		mLoadThread.join();
	}
}