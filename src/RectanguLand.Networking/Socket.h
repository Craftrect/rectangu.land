#pragma once

#include "SocketStream.h"

namespace Network
{
	class Socket : public std::enable_shared_from_this<Socket>
	{
		static boost::asio::ip::tcp::resolver gResolver;

		boost::asio::ip::tcp::socket mSocket;

		SocketStreamPtr mStream;

		std::shared_ptr<std::istream> mInputStream;
		std::shared_ptr<std::ostream> mOutputStream;

		void asyncConnect_resolveCallback(
			std::function<void (boost::system::error_code)> callback,
			const boost::system::error_code& code, 
			boost::asio::ip::tcp::resolver::iterator endpoint
		);

	protected:
		void onSocketConnected();

	public:
		Socket();

		void connect(const String& hostOrIp, uint16 port);
		void asyncConnect(const String& hostOrIp, uint16 port, std::function<void (boost::system::error_code)> callback);

		void readToEnd(std::vector<char>& data);
		int32 read(std::vector<char>& data);
		void readAsync(std::vector<char>& data, std::function<void (int32)> callback);
		bool write(const std::vector<char>& data, uint32 numBytes);

		std::istream& getInputStream() { return *mInputStream.get(); }
		std::ostream& getOutputStream() { return *mOutputStream.get(); }
	};

	SHARED_TYPE(Socket);
	WEAK_TYPE(Socket);
}