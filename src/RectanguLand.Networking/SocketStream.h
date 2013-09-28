#pragma once

namespace Network
{
	SHARED_FORWARD(Socket);
	WEAK_TYPE(Socket);

	class SocketStream : public std::streambuf
	{
		friend class Socket;

		static const int ChunkSize = 0x400;

		SocketWeakPtr mSocket;
		std::vector<char> mTempBuffer;
		std::vector<char> mOutputBuffer;
		uint32 mOutputSize;

	public:
		SocketStream(SocketPtr socket);

	protected:
		std::streambuf::int_type underflow();
		std::streambuf::int_type overflow(std::streambuf::int_type c);
		int sync();
	};

	SHARED_TYPE(SocketStream);
};