#pragma once

namespace Utils
{
	class InflateException : public std::exception
	{
		String mMessage;
		std::string mAnsiMsg;
	public:
		InflateException(const String& message) : mMessage(message) {
			mAnsiMsg = message.toMultibyte();
		}

		const char* what() const { return mAnsiMsg.c_str(); }
	};

	class ZInflater : boost::noncopyable
	{
		class InflateImpl;

		InflateImpl* mInflater;
	public:
		ZInflater();
		~ZInflater();

		void begin();
		void update(std::vector<char>& input, std::vector<char>& output, uint32& outputInsert);
		void end();
	};
}