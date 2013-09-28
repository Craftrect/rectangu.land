#include "Precompiled.h"
#include "FtpFileStream.h"

namespace Network
{
	namespace Ftp
	{
		FtpFileStream::FtpFileStream(FtpClientPtr client, SocketPtr sock) {
			mSocket = sock;
			mClient = client;
			mIsReadEnd = false;
			mReadProcessed = false;
			mReadBuffer.resize(DataChunkSize);
		}

		void FtpFileStream::asyncReadCallback(int32 numBytes) {
			{
				std::lock_guard<std::mutex> l(mLoadLock);

				if (numBytes <= 0) {
					mIsReadEnd = true;
					mReadProcessed = true;
					mReadEvent.notify_all();
					mClient->informReadComplete();
					return;
				}

				mPutbackBuffer.insert(mPutbackBuffer.end(), &mReadBuffer[0], &mReadBuffer[numBytes - 1] + 1);
				mReadProcessed = true;
				mReadEvent.notify_all();
			}

			mSocket->readAsync(mReadBuffer, std::bind(&FtpFileStream::asyncReadCallback, this, std::placeholders::_1));
		}

		bool FtpFileStream::beginRead() {
			auto numBytes = mSocket->read(mReadBuffer);
			if (numBytes <= 0) {
				return false;
			}

			mCurrentBuffer.insert(mCurrentBuffer.end(), std::begin(mReadBuffer), std::begin(mReadBuffer) + numBytes);
			setg(&mCurrentBuffer.front(), &mCurrentBuffer.front(), &mCurrentBuffer.back());

			mSocket->readAsync(mReadBuffer, std::bind(&FtpFileStream::asyncReadCallback, this, std::placeholders::_1));

			return true;
		}

		void FtpFileStream::flushPutback() {
			if (mPutbackBuffer.size() == 0) {
				return;
			}

			auto g = gptr();
			uint32 offset = g - eback();

			mCurrentBuffer.insert(mCurrentBuffer.end(), mPutbackBuffer.begin(), mPutbackBuffer.end());
			mPutbackBuffer.clear();

			setg(&mCurrentBuffer[0], &mCurrentBuffer[offset], &mCurrentBuffer[mCurrentBuffer.size() - 1]);
		}

		std::streambuf::int_type FtpFileStream::underflow() {
			{
				std::lock_guard<std::mutex> l(mLoadLock);
				
				if (gptr() >= egptr()) {
					flushPutback();
				}

				if (gptr() < egptr()) {
					return traits_type::to_int_type(*gptr());
				}

				if (mIsReadEnd) {
					return traits_type::eof();
				}

				mReadProcessed = false;
			}

			while (gptr() >= egptr() && mIsReadEnd == false) {
					std::unique_lock<std::mutex> ul(mReadLock);
					mReadEvent.wait(ul, [this]() { return mReadProcessed; });
			}

			std::lock_guard<std::mutex> l(mLoadLock);

			flushPutback();

			if (gptr() < egptr()) {
				return traits_type::to_int_type(*gptr());
			}

			return traits_type::eof();
		}
	}
}