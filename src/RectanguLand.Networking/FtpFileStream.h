#pragma once

#include "Socket.h"
#include "FTP/FtpClient.h"

namespace Network
{
	namespace Ftp
	{
		class FtpFileStream : public std::streambuf
		{
		private:
			static const uint32 DataChunkSize = 0x8000;

			std::mutex mLoadLock;
			std::mutex mReadLock;
			SocketPtr mSocket;
			FtpClientPtr mClient;
			std::vector<char> mCurrentBuffer;
			std::vector<char> mReadBuffer;
			std::vector<char> mPutbackBuffer;
			bool mIsReadEnd;
			bool mReadProcessed;
			std::condition_variable mReadEvent;

			void asyncReadCallback(int32 numBytes);
			void flushPutback();

		protected:
			int_type underflow();

		public:
			FtpFileStream(FtpClientPtr client, SocketPtr dataSocket);
			bool beginRead();
		};
	};
};