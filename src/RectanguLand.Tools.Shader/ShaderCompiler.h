#pragma once

class ShaderCompiler
{
public:
	class ShaderError
	{
	public:
		bool isWarning;
		String message;
		uint32 line;
	};

private:
	std::list<ShaderError> mLastErrors;
	std::vector<char> mLastCode;
	bool mLastSuccess;

public:
	void clear();
	void compile(const String& source, const String& entryPoint, const String& target);

	std::list<ShaderError>& getErrors() { return mLastErrors; }
	std::vector<char>& getCode() { return mLastCode; }
	bool succeeded() const { return mLastSuccess; }
};