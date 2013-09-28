#include "Precompiled.h"
#include "SocketStream.h"
#include "Socket.h"

namespace Network
{
	SocketStream::SocketStream(SocketPtr socket) {
		mSocket = socket;
		mOutputSize = 0;

		mTempBuffer.resize(ChunkSize);
		mOutputBuffer.resize(ChunkSize);

		char* end = mTempBuffer.data() + ChunkSize;
		setg(end, end, end);
	}

	std::streambuf::int_type SocketStream::underflow() {
		if (gptr() < egptr()) {
			return traits_type::to_int_type(*gptr());
		}

		int32 numRead = mSocket.lock()->read(mTempBuffer);
		if (numRead <= 0) {
			return traits_type::eof();
		}

		setg(mTempBuffer.data(), mTempBuffer.data(), mTempBuffer.data() + numRead);
		
		return traits_type::to_int_type(*gptr());
	}

	std::streambuf::int_type SocketStream::overflow(std::streambuf::int_type c) {
		if (mOutputSize + 1 > ChunkSize) {
			if (mSocket.lock()->write(mOutputBuffer, mOutputSize) == false) {
				return traits_type::eof();
			}

			mOutputSize = 0;
		}

		mOutputBuffer[mOutputSize] = std::streambuf::traits_type::to_char_type(c);
		++mOutputSize;

		return c;
	}

	int SocketStream::sync() {
		mSocket.lock()->write(mOutputBuffer, mOutputSize);

		mOutputSize = 0;
		return 0;
	}
}