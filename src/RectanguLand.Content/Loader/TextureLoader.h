#pragma once

#include "ImageLoader.h"
#include <RectanguLand.Graphics/Objects/TextureD3D11.h>

namespace Content
{
	class TextureLoader
	{
	protected:
		class TexturePayloadData
		{
		private:
			std::shared_ptr<std::istream> mStream;
			std::function<void (ImageLoaderPtr)> mCallback;

		public:
			TexturePayloadData(std::shared_ptr<std::istream> stream, std::function<void (ImageLoaderPtr)> callback) : mStream(stream), mCallback(callback) {

			}

			std::shared_ptr<std::istream> getStream() { return mStream; }
			std::function<void (ImageLoaderPtr)> getCallback() const { return mCallback; }
		};
	private:
		class TextureFetchThread
		{
		private:
			std::thread mLoadThread;
			bool mIsRunning;
			std::queue<TexturePayloadData> mWorkItems;
			std::mutex mQueueLock;

			void threadCallback();

		public:
			TextureFetchThread();

			uint32 getQueueSize() const { return mWorkItems.size(); }

			void postWorkItem(TexturePayloadData& item);

			void shutdown();
		};

		SHARED_TYPE(TextureFetchThread);

		std::list<TextureFetchThreadPtr> mThreads;

	protected:
		TextureLoader(uint32 threadCount);

		void postWorkItem(TexturePayloadData& item);

	public:
		virtual void shutdownThreads();
		
		virtual Graphics::TexturePtr loadTexture(std::shared_ptr<std::istream> resource) = 0;
	};

	SHARED_TYPE(TextureLoader);
};