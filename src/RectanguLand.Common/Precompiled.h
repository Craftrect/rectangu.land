#pragma once

#include <iostream>
#include <iomanip>

#include <string>
#include <sstream>
#include <tchar.h>

#include <fstream>

#include <memory>
#include <algorithm>
#include <functional>

#include <vector>
#include <map>
#include <list>
#include <set>
#include <queue>
#include <deque>
#include <unordered_map>

#include <mutex>
#include <thread>
#include <condition_variable>
#include <future>

#include <chrono>
#include <filesystem>

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#endif

#include <boost/any.hpp>
#include <boost/optional.hpp>
#include <boost/asio.hpp>
#include <boost/program_options.hpp>
#include <boost/iostreams/filtering_stream.hpp>
#include <boost/iostreams/filter/zlib.hpp>
#include <boost/iostreams/copy.hpp>
#include <boost/interprocess/streams/vectorstream.hpp>

typedef unsigned __int64 uint64;
typedef signed __int64 int64;
typedef unsigned __int32 uint32;
typedef signed __int32 int32;
typedef unsigned __int16 uint16;
typedef signed __int16 int16;
typedef unsigned __int8 uint8;
typedef signed __int8 int8;

namespace std
{
	typedef std::basic_string<TCHAR> tstring;
	typedef std::basic_stringstream<TCHAR> tstringstream;
	typedef std::basic_istringstream<TCHAR> tistringstream;
	typedef std::basic_ostringstream<TCHAR> tostringstream;
	typedef std::basic_fstream<TCHAR> tfstream;
	typedef std::basic_ifstream<TCHAR> tifstream;
	typedef std::basic_ofstream<TCHAR> tofstream;
};

#define WEAK_TYPE(T) typedef std::weak_ptr<T> T##WeakPtr
#define WEAK_FORWARD(T) class T; WEAK_TYPE(T)
#define SHARED_TYPE(T) typedef std::shared_ptr<T> T##Ptr
#define	SHARED_FORWARD(T) class T; SHARED_TYPE(T)

#include "ComPtr.h"

#undef min
#undef max