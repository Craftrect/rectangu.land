#include "PublicInclude.h"
#include "String.h"

std::ostream& operator << (std::ostream& strm, const String& str) {
	strm << str.toMultibyte();
	return strm;
}

std::wostream& operator << (std::wostream& strm, const String& str) {
	strm << (std::wstring)str;
	return strm;
}

std::wstring::const_iterator begin(const String& str) {
	return str.begin();
}

std::wstring::const_iterator end(const String& str) {
	return str.end();
}

std::wstring::iterator begin(String& str) {
	return str.begin();
}

std::wstring::iterator end(String& str) {
	return str.end();
}

std::wstring String::toUnicode(const std::string& str) {
	if (str.length() == 0) {
		return std::wstring();
	}

	std::vector<wchar_t> unibuffer;
	uint32 unisize = 0;

#ifdef _WIN32
	unisize = MultiByteToWideChar(CP_UTF8, 0, str.c_str(), str.length(), nullptr, 0);
	unibuffer.resize(unisize + 1);

	MultiByteToWideChar(CP_UTF8, 0, str.c_str(), str.length(), unibuffer.data(), unisize);

	return std::wstring(unibuffer.data());
#endif
}

std::string String::toAscii(const std::wstring& str) {
	if (str.length() == 0) {
		return std::string();
	}

	std::vector<char> ansibuffer;
	uint32 ansisize = 0;

#ifdef _WIN32
	ansisize = WideCharToMultiByte(CP_UTF8, 0, str.c_str(), str.length(), nullptr, 0, nullptr, nullptr);
	ansibuffer.resize(ansisize + 1);
	ansisize = WideCharToMultiByte(CP_UTF8, 0, str.c_str(), str.length(), ansibuffer.data(), ansisize, nullptr, nullptr);

	return std::string(ansibuffer.data());
#endif
}

String::String() {

}

String::String(const char* str) {
	if (str != nullptr) {
		mString = toUnicode(str);
	}
}

String::String(const wchar_t* str) {
	if (str != nullptr) {
		mString = std::wstring(str);
	}
}

String::String(const std::string& str) {
	mString = toUnicode(str);
}

String::String(const std::wstring& str) : mString(str) {

}

String::String(const String& other) {
	mString = other.mString;
}

String::String(const icu::UnicodeString& ucs) {
	if(ucs.isEmpty() == false && ucs.length() > 0) {
		auto start = ucs.getBuffer();
		mString.assign(start, start + ucs.length());
	}
}

String::~String() {

}

String& String::operator = (const String& other) {
	if (this == &other) {
		return *this;
	}

	mString = other.mString;
	return *this;
}

void String::split(std::list<String>& elems, wchar_t delim) const {
	std::wstringstream ss(mString);
	std::wstring item;
	while (std::getline(ss, item, delim)) {
		elems.push_back(item);
	}
}

bool String::endsWith(const String& str) const {
	return mString.rfind(str.mString) == (mString.length() - str.length());
}

bool String::startsWith(const String& str) const {
	return mString.find(str.mString) == 0;
}

String String::substring(std::wstring::iterator itr) const {
	return String(mString.substr(itr - mString.begin()));
}

String String::substring(std::wstring::iterator itr, std::wstring::iterator end) const {
	return String(mString.substr(itr - mString.begin(), end - itr));
}

String String::substring(uint32 start) const {
	return String(mString.substr(start));
}

String String::substring(uint32 start, uint32 length) const {
	return String(mString.substr(start, length));
}

String String::trim() const {
	return trimLeft().trimRight();
}

String String::trimLeft() const {
	auto copy = mString;
	copy.erase(copy.begin(), std::find_if(copy.begin(), copy.end(), std::not1(std::ptr_fun<int, int>(std::isspace))));
	return String(copy);
}

String String::trimRight() const {
	auto copy = mString;
	copy.erase(std::find_if(copy.rbegin(), copy.rend(), std::not1(std::ptr_fun<int, int>(std::isspace))).base(), copy.end());
	return String(copy);
}

String String::toLower() const {
	auto copy = mString;
	std::transform(copy.begin(), copy.end(), copy.begin(), std::towlower);
	return String(copy);
}

String String::toUpper() const {
	auto copy = mString;
	std::transform(copy.begin(), copy.end(), copy.begin(), std::towupper);
	return String(copy);
}

std::wstring::size_type String::find(const String& pattern) const {
	return mString.find(pattern.mString);
}

std::wstring::size_type String::find(const wchar_t& pattern) const {
	return mString.find(pattern);
}

bool String::operator == (const String& other) const {
	return mString == other.mString;
}

bool String::operator == (const std::wstring& other) const {
	return mString == other;
}

bool String::operator == (const std::string& other) const {
	return mString == toUnicode(other);
}

bool String::operator == (const char* other) const {
	if (other == nullptr) {
		return empty();
	}

	return mString == toUnicode(other);
}

bool String::operator == (const wchar_t* other) const {
	if (other == nullptr) {
		return empty();
	}

	return mString == other;
}

bool String::operator < (const String& other) const {
	return mString < other.mString;
}

uint32 String::hash() const {
	static std::hash<std::wstring> hasher;
	return hasher(mString);
}