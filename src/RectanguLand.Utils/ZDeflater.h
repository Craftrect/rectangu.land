#pragma once

namespace Utils
{
	class DeflateException : public std::exception
	{
		String mMessage;
		std::string mAnsiMsg;
	public:
		DeflateException(const String& message) : mMessage(message) {
			mAnsiMsg = message.toMultibyte();
		}

		const char* what() const { return mAnsiMsg.c_str(); }
	};

	class ZDeflater : boost::noncopyable
	{
		class DeflateImpl;

		DeflateImpl* mInflater;
	public:
		ZDeflater();
		~ZDeflater();

		void begin();
		void update(std::vector<char>& input, std::vector<char>& output, uint32& outputInsert);
		void end();
	};
}