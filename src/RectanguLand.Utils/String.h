#pragma once

class String
{
	friend class StrHash;

private:
	std::wstring mString;

	static std::string toAscii(const std::wstring& str);
	static std::wstring toUnicode(const std::string& str);

public:
	String();
	String(const char* str);
	String(const wchar_t* str);
	String(const std::wstring& str);
	String(const std::string& str);
	String(const String& other);
	String(const icu::UnicodeString& str);

	~String();

	String& operator = (const String& other);

	bool endsWith(const String& other) const;
	bool startsWith(const String& other) const;

	String trim() const;
	String trimLeft() const;
	String trimRight() const;

	void split(std::list<String>& elems, wchar_t delimeter) const;
	String substring(std::wstring::iterator start) const;
	String substring(std::wstring::iterator start, std::wstring::iterator end) const;
	String substring(uint32 start) const;
	String substring(uint32 start, uint32 length) const;

	uint32 hash() const;

	String toLower() const;
	String toUpper() const;

	std::wstring::size_type find(const String& pattern) const;
	std::wstring::size_type find(const wchar_t& pattern) const;

	std::string toMultibyte() const { return toAscii(mString); }

	operator const std::wstring& () const { return mString; }
	operator std::wstring() const { return mString; }
	operator const std::wstring() const { return mString; }

	operator std::string() const { return toAscii(mString); }
	operator const std::string() const { return toAscii(mString); }
	operator const wchar_t* () const { return mString.c_str(); }

	std::wstring* operator -> () { return &mString; }
	const std::wstring* operator -> () const { return &mString; }

	bool operator == (const String& other) const;
	bool operator == (const std::wstring& other) const;
	bool operator == (const std::string& other) const;
	bool operator == (const char* other) const;
	bool operator == (const wchar_t* other) const;

	bool operator < (const String& other) const;

	template<typename T>
	String& operator += (const T& value) {
		mString += value;
		return *this;
	}

	template<>
	String& operator += <String> (const String& value) {
		mString += value.mString;
		return *this;
	}

	String& operator += (const char* value) {
		if (value == nullptr) {
			return *this;
		}

		mString += toUnicode(value);
		return *this;
	}

	template<typename T>
	String operator + (const T& value) const {
		std::wstring newStr = mString;
		newStr += value;
		return String(newStr);
	}

	String operator + (const char* value) const {
		if (value == nullptr) {
			return String(mString);
		}

		auto str = mString;
		str += toUnicode(value);
		return String(mString);
	}

	template<>
	String operator + <String>(const String& other) const {
		std::wstring newStr = mString;
		newStr += other.mString;
		return String(newStr);
	}

	template<>
	String operator + <std::wstring>(const std::wstring& other) const {
		std::wstring newStr = mString;
		newStr += other;
		return String(newStr);
	}

	template<>
	String operator + <std::string>(const std::string& other) const {
		std::wstring newStr = mString;
		newStr += toUnicode(other);
		return String(newStr);
	}

	uint32 length() const { return mString.length(); }
	bool empty() const { return mString.empty(); }

	std::wstring::const_iterator begin() const { return mString.begin(); }
	std::wstring::const_iterator end() const { return mString.end(); }

	std::wstring::iterator begin() { return mString.begin(); }
	std::wstring::iterator end() { return mString.end(); }

	class StrHash
	{
	public:
		int32 operator () (const String& str) const {
			static std::hash<std::wstring> hash;
			return hash(str.mString);
		}
	};
};

std::wstring::const_iterator begin(const String& str);
std::wstring::const_iterator end(const String& str);
std::wstring::iterator begin(String& str);
std::wstring::iterator end(String& str);

std::ostream& operator << (std::ostream& strm, const String& str);
std::wostream& operator << (std::wostream& strm, const String& str);