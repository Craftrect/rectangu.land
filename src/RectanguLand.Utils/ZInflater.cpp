#include "PublicInclude.h"
#include "ZInflater.h"

#include <zlib/zlib.h>

namespace Utils
{
	class ZInflater::InflateImpl : boost::noncopyable
	{
		z_stream mStream;
		bool mIsInitialized;
	public:
		InflateImpl() : mIsInitialized(false) {
			memset(&mStream, 0, sizeof(mStream));
		}

		~InflateImpl() {
			if (mIsInitialized) {
				inflateEnd(&mStream);
			}
		}

		void init() {
			if (mIsInitialized == true) {
				throw std::bad_function_call("inflater stream has to be finished before it is initialized again.");
			}

			mStream.zalloc = Z_NULL;
			mStream.zfree = Z_NULL;
			mStream.opaque = Z_NULL;
			mStream.avail_in = 0;
			mStream.next_in = Z_NULL;

			auto err = inflateInit(&mStream);
			if (err != Z_OK) {
				throw InflateException("ZLib error on inflateInit");
			}

			mIsInitialized = true;
		}

		void finalize() {
			auto error = inflateEnd(&mStream);
			if (error != Z_OK) {
				throw InflateException("ZLib error on inflateEnd");
			}

			mIsInitialized = false;
		}

		//! Inflates the content of input into output
		/*!
			@param	input		Input data compressed using ZLib deflate
			@param	output		Output buffer filled with inflated data. If decompressed data does not fit into
								the buffer it is expanded. Use outputInsert to start inserting at a specific position
			@param	outputPos	Position to start inserting decompressed data

			@throws	InflateException	Unable to inflate the data (not enough memory, invalid data, ...)
					bad_function_call	Init has not been called
		*/
		void inflate(std::vector<char>& input, std::vector<char>& output, uint32& outputPos) {
			if (mIsInitialized == false) {
				throw std::bad_function_call("inflater is not yet initialized");
			}

			if (outputPos >= output.size()) {
				throw std::invalid_argument("Start index is beyond the output buffers end");
			}

			mStream.avail_in = input.size();
			mStream.next_in = (unsigned char*) input.data();

			std::vector<char> processBuffer(0x10000);
			bool useAuxBuffer = false;

			do {
				if (useAuxBuffer == true) {
					mStream.avail_out = processBuffer.size();
					mStream.next_out = (unsigned char*) processBuffer.data();
				} else {
					mStream.avail_out = output.size() - outputPos;
					if (mStream.avail_out == 0) {
						useAuxBuffer = true;
						continue;
					}

					mStream.next_out = (unsigned char*) &output[outputPos];
				}

				auto error = ::inflate(&mStream, Z_NO_FLUSH);
				if (error != Z_OK && error != Z_STREAM_END) {
					throw InflateException("ZLib error on inflate. Probably invalid compression stream");
				}

				uint32 max = useAuxBuffer ? processBuffer.size() : (output.size() - outputPos);

				int32 have = max - mStream.avail_out;
				if (useAuxBuffer == true) {
					output.insert(output.end(), processBuffer.begin(), processBuffer.begin() + have);
				} 

				outputPos += have;
			} while (mStream.avail_out == 0);
		}
	};

	ZInflater::ZInflater() {
		mInflater = new InflateImpl();
	}

	ZInflater::~ZInflater() {
		delete mInflater;
	}

	void ZInflater::begin() {
		mInflater->init();
	}

	void ZInflater::end() {
		mInflater->finalize();
	}

	void ZInflater::update(std::vector<char>& input, std::vector<char>& output, uint32& outputInsert) {
		mInflater->inflate(input, output, outputInsert);
	}
};