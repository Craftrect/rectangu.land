#include "Precompiled.h"
#include "Socket.h"

namespace Network
{
	namespace io = boost::asio;
	using io::ip::tcp;

	class IOServiceWrap
	{
		io::io_service mService;
		std::thread mIOThread;
	public:
		IOServiceWrap() {

		}

		~IOServiceWrap() {

		}

		io::io_service& getService(){ return mService; }
		const io::io_service& getService() const{ return mService; }
	};

	static IOServiceWrap gIoService;

	tcp::resolver Socket::gResolver(gIoService.getService());

	Socket::Socket() : mSocket(gIoService.getService()) {

	}

	void Socket::connect(const String& ipOrHost, uint16 port) {
		static tcp::resolver::iterator end;

		std::stringstream portStrm;
		portStrm << port;

		boost::system::error_code code;
		auto endpoint = gResolver.resolve(tcp::resolver::query(ipOrHost.toMultibyte(), portStrm.str()), code);

		if (endpoint == end) {
			throw std::invalid_argument(code.message().c_str());
		}

		code = boost::asio::error::host_not_found;

		while (endpoint != end && code) {
			mSocket.connect(*endpoint, code);

			++endpoint;
		}

		if (code) {
			throw std::invalid_argument(code.message().c_str());
		}

		onSocketConnected();
	}

	void Socket::asyncConnect(const String& ipOrHost, uint16 port, std::function<void (boost::system::error_code)> callback) {
		static tcp::resolver::iterator end;

		tcp::resolver resolver(gIoService.getService());

		std::stringstream portStrm;
		portStrm << port;

		boost::system::error_code code;


		gResolver.async_resolve(tcp::resolver::query(ipOrHost.toMultibyte(), portStrm.str()), std::bind(&Socket::asyncConnect_resolveCallback, this, callback, 
			std::placeholders::_1, std::placeholders::_2));
	}

	void Socket::asyncConnect_resolveCallback(
		std::function<void (boost::system::error_code)> callback,
		const boost::system::error_code& code,
		tcp::resolver::iterator endpoint) {
	
		static tcp::resolver::iterator end;

		if (code || endpoint == end) {
			callback(code);
		}

		boost::system::error_code errcode = boost::asio::error::host_not_found;

		while (endpoint != end && errcode) {
			mSocket.connect(*endpoint, errcode);

			++endpoint;
		}

		if (!errcode) {
			onSocketConnected();
		}

		callback(errcode);
	}

	int32 Socket::read(std::vector<char>& data) {
		boost::system::error_code code;
		int32 numBytes = (int32)mSocket.read_some(boost::asio::buffer(data), code);

		if (code) {
			return -1;
		}

		return numBytes;
	}

	bool Socket::write(const std::vector<char>& data, uint32 numBytes) {
		uint32 numWritten = 0;
		boost::system::error_code code;

		while (numWritten < numBytes) {
			auto written = mSocket.write_some(boost::asio::buffer(&data[numWritten], numBytes - numWritten), code);
			
			if (code == false) {
				return false;
			}

			numWritten += written;
		}

		return true;
	}

	void Socket::onSocketConnected() {
		mStream = std::make_shared<SocketStream>(shared_from_this());

		mInputStream = std::make_shared<std::istream>(mStream.get(), false);
		mOutputStream = std::make_shared<std::ostream>(mStream.get(), false);
	}

	void Socket::readToEnd(std::vector<char>& data) {
		__declspec(thread) static char chunk[0x4000];

		boost::system::error_code code;
		int32 numRead = mSocket.read_some(boost::asio::buffer(chunk, 0x4000), code);

		while (numRead > 0 && !code) {
			data.insert(data.end(), (char*) chunk, chunk + numRead);
			int32 numRead = mSocket.read_some(boost::asio::buffer(chunk, 0x4000), code);
		}
	}

	void Socket::readAsync(std::vector<char>& data, std::function<void (int32)> callback) {
		auto asyncCallback = [&data, callback](boost::system::error_code code, size_t size) {
			if (code != false) {
				callback(-1);
			} else {
				callback((int32) size);
			}
		};

		std::async(std::launch::async, [this, &data, asyncCallback]() {
			boost::system::error_code code;
			auto numRet = mSocket.read_some(boost::asio::buffer(data), code);
			asyncCallback(code, numRet);
		});
	}
}