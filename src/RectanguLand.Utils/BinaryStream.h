#pragma once

namespace Utils
{
	class Encoding
	{
	private:
		class BigEndianEncoding;
		class LittleEndianEncoding;
	protected:
		Encoding() {

		}

	public:
		template<typename T>
		void read(T& out, std::istream& stream) {
			if (!stream.read((char*) &out, sizeof(T))) {
				throw std::out_of_range("Stream reached end of file");
			}
		}

		virtual void read(int32& out, std::istream& stream) {
			read<int32>(out, stream);
		}

		virtual void read(int16& out, std::istream& stream) {
			read<int16>(out, stream);
		}

		virtual void read(int64& out, std::istream& stream) {
			read<int64>(out, stream);
		}

		virtual void read(uint32& out, std::istream& stream) {
			read<uint32>(out, stream);
		}

		virtual void read(uint16& out, std::istream& stream) {
			read<uint16>(out, stream);
		}

		virtual void read(uint64& out, std::istream& stream) {
			read<uint64>(out, stream);
		}

		virtual void read(float& out, std::istream& stream) {
			read<float>(out, stream);
		}

		static BigEndianEncoding BigEndian;
		static LittleEndianEncoding LittleEndian;
	};

	class Encoding::BigEndianEncoding : public Encoding
	{
	public:
		void read(uint32& out, std::istream& stream) {
			if (!stream.read((char*) &out, sizeof(uint32))) {
				throw std::out_of_range("Stream reached end of file");
			}

			out = (out >> 24) | ((out & 0x00FF0000) >> 8) | ((out & 0x0000FF00) << 8) | ((out & 0x000000FF) << 24);
		}

		void read(uint16& out, std::istream& stream) {
			if (!stream.read((char*) &out, sizeof(uint16))) {
				throw std::out_of_range("Stream reached end of file");
			}

			out = (out >> 8) | (out << 8);
		}

		void read(uint64& out, std::istream& stream) {
			if (!stream.read((char*) &out, sizeof(uint64))) {
				throw std::out_of_range("Stream reached end of file");
			}

			out = (out >> 56) | ((out & 0x00FF000000000000) >> 40) | ((out & 0x0000FF0000000000) >> 24) | ((out & 0x000000FF00000000) >> 8)
				| ((out & 0x00000000FF000000) << 8) | ((out & 0x0000000000FF0000) << 24) | ((out & 0x000000000000FF00) << 40) | (out << 56);
		}

		void read(int32& out, std::istream& stream) {
			if (!stream.read((char*) &out, sizeof(int32))) {
				throw std::out_of_range("Stream reached end of file");
			}

			out = (out >> 24) | ((out & 0x00FF0000) >> 8) | ((out & 0x0000FF00) << 8) | ((out & 0x000000FF) << 24);
		}

		void read(int16& out, std::istream& stream) {
			if (!stream.read((char*) &out, sizeof(int16))) {
				throw std::out_of_range("Stream reached end of file");
			}

			out = (out >> 8) | (out << 8);
		}

		void read(int64& out, std::istream& stream) {
			if (!stream.read((char*) &out, sizeof(int64))) {
				throw std::out_of_range("Stream reached end of file");
			}

			out = (out >> 56) | ((out & 0x00FF000000000000) >> 40) | ((out & 0x0000FF0000000000) >> 24) | ((out & 0x000000FF00000000) >> 8)
				| ((out & 0x00000000FF000000) << 8) | ((out & 0x0000000000FF0000) << 24) | ((out & 0x000000000000FF00) << 40) | (out << 56);
		}

		void read(float& out, std::istream& stream) {
			if (!stream.read((char*) &out, sizeof(float))) {
				throw std::out_of_range("Stream reached end of file");
			}

			uint32& uint = *(uint32*) &out;
			uint = (uint >> 24) | ((uint & 0x00FF0000) >> 8) | ((uint & 0x0000FF00) << 8) | ((uint & 0x000000FF) << 24);
		}
	};

	class Encoding::LittleEndianEncoding : public Encoding
	{
	public:
	};

	class BinaryStream
	{
		std::istream& mStream;
		Encoding* mEncoding;
	public:
		BinaryStream(std::istream& stream) : mStream(stream), mEncoding(&Encoding::LittleEndian) {

		}

		std::istream& getStream() { return mStream; }

		void setEncoding(Encoding& enc) {
			mEncoding = &enc;
		}

		void seek(int32 mod) {
			mStream.seekg(mod, std::ios::cur);
		}

		void seekAbs(uint32 pos) {
			mStream.seekg(pos, std::ios::beg);
		}

		template<typename T>
		T read() {
			T ret;
			read(ret);
			return ret;
		}

		template<typename T>
		void read(T& input) {
			mEncoding->read(input, mStream);
		}

		template<typename T>
		void read(std::vector<T>& input) {
			if (input.size() == 0) {
				return;
			}

			if (!mStream.read((char*) input.data(), input.size() * sizeof(T))) {
				throw std::out_of_range("Stream reached end of file");
			}
		}

		template<typename T, uint32 size>
		void read(T (&input)[size]) {
			static_assert(size != 0, "static_assert(size != 0), Cannot read to an empty array!");

			if (!mStream.read((char*) input, size * sizeof(T))) {
				throw std::out_of_range("Stream reached end of file");
			}
		}
	};
}