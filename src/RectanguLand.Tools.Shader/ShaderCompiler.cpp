#include "StdAfx.h"
#include "ShaderCompiler.h"

#include <d3dcompiler.h>
#include <Shlwapi.h>

void ShaderCompiler::compile(const String& source, const String& entryPoint, const String& target) {
	mLastErrors.clear();
	String src = source;
	src += "\n";
	Utils::ComPtr<ID3DBlob> code, errors;
	auto res = D3DCompile(src.toMultibyte().c_str(), src.length(), "RectanguLand.Tools.Shader", nullptr, nullptr, entryPoint.toMultibyte().c_str(), target.toMultibyte().c_str(),
		/*D3DCOMPILE_SKIP_OPTIMIZATION | D3DCOMPILE_DEBUG*/ D3DCOMPILE_OPTIMIZATION_LEVEL3, 0, code, errors);

	mLastSuccess = SUCCEEDED(res);

	if (FAILED(res)) {
		String str;
		std::vector<char> data(errors->GetBufferSize() + 1);
		memcpy(data.data(), errors->GetBufferPointer(), data.size() - 1);
		str = data.data();

		std::list<String> errorLines;
		str.split(errorLines, L'\n');

		for (auto& line : errorLines) {
			ShaderError err;
			err.isWarning = false;
			
			wchar_t* file = _wcsdup(((std::wstring)line).c_str());
			PathStripPath(file);
			String strip = file;
			free(file);

			std::wstring strw = strip;
			auto pos = strw.find(L": error ");
			if (pos == std::wstring::npos) {
				err.message = strw;
				err.line = 0;
				auto itr = std::find_if(mLastErrors.begin(), mLastErrors.end(), [&err](ShaderError e) { return e.line == err.line; });
				if (itr != mLastErrors.end()) {
					(*itr).message += "\n";
					(*itr).message += err.message;
				}

				mLastErrors.push_back(err);
				continue;
			}

			auto errStart = strw.rfind(L'(', pos);
			auto errEnd = strw.find(L',', errStart);
			std::wstring linePart = strw.substr(errStart + 1, (errEnd - errStart - 1));
			std::wstringstream strm;
			strm << linePart;
			
			strm >> err.line;
			err.isWarning = false;
			err.message = strw.substr(pos + 8);

			auto itr = std::find_if(mLastErrors.begin(), mLastErrors.end(), [&err](ShaderError e) { return e.line == err.line; });
			if (itr != mLastErrors.end()) {
				(*itr).message += "\n";
				(*itr).message += err.message;
				continue;
			}

			mLastErrors.push_back(err);
		}
	} else {
		mLastCode.resize(code->GetBufferSize());
		memcpy(mLastCode.data(), code->GetBufferPointer(), mLastCode.size());
	}
}