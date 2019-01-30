#pragma once

#include <vector>
#include <stdint.h>
#include "macros.h"

#if YG_PLATFORM == YG_PLATFORM_ANDROID
	#ifndef ntohll
		#define ntohll( x ) ( ( ( int64_t )( ntohl( ( int32_t )( ( x << 32 ) >> 32 ) ) ) << 32) | ( uint32_t )ntohl( ( ( int32_t )( x >> 32 ) ) ) )
		#define htonll( x ) ntohll( x )
	#endif
#endif 

class BinaryInputStream
{
public:
	BinaryInputStream(const void *data, size_t length)
	{
		mError = false;
		mOffset = 0;
		mData = static_cast<const uint8_t*>(data);
		mLength = length;
	}

	// readInt will generate an error for bool types
	template <class IntT>
	IntT readNumber()
	{
		IntT value = 0;
		read(&value);
		return value;
	}


	int32_t readInt()
	{
		auto num = readNumber<int32_t>();
		return ntohl(num);
	}

	uint32_t readUint()
	{
		auto num = readNumber<uint32_t>();
		return ntohl(num);
	}
    
	int16_t readShort()
	{
		auto num = readNumber<int16_t>();
		return ntohs(num);
	}
	
	uint16_t readUShort()
	{
		auto num = readNumber<uint16_t>();
		return ntohs(num);
	}

	int64_t readLong()
	{
		auto num = readNumber<int64_t>();
		return ntohll(num);
	}

	uint64_t readULong()
	{
		auto num = readNumber<uint64_t>();
		return ntohll(num);
	}

	bool readBool()
	{
		int value = 0;
		read(&value);
		return (value > 0);
	}

	void readBool(bool *outValue)
	{
		*outValue = readBool();
	}

	char readByte()
	{
		auto num = readNumber<char>();
		return num;
	}

	void readBytes(unsigned char outArray[], size_t count)
	{
		read<unsigned char>(outArray, count);
	}

	std::string readString(size_t length)
	{
		std::string outString;
		readString(&outString, length);
		return outString;
	}

	void readString(std::string *v, size_t length)
	{
		if (mError)
		{
			return;
		}

		size_t checkedOffset = mOffset;
		checkedOffset += length;

		if (mOffset + length > mLength)
		{
			mError = true;
			return;
		}

		v->assign(reinterpret_cast<const char *>(mData) + mOffset, length);
		mOffset = checkedOffset;
	}

	void skip(size_t length)
	{
		size_t checkedOffset = mOffset;
		checkedOffset += length;

		if ( mOffset + length > mLength)
		{
			mError = true;
			return;
		}

		mOffset = checkedOffset;
	}

	size_t offset() const
	{
		return mOffset;
	}

	bool error() const
	{
		return mError;
	}

	bool endOfStream() const
	{
		return mOffset == mLength;
	}

	const uint8_t *data()
	{
		return mData;
	}

private:
	bool mError;
	size_t mOffset;
	const uint8_t *mData;
	size_t mLength;

	template <typename T>
	void read(T *v, size_t num)
	{
		static_assert(std::is_fundamental<T>::value, "T must be a fundamental type.");

		size_t checkedLength = num;		
		checkedLength *= sizeof(T);
		
		size_t checkedOffset = mOffset;
		checkedOffset += checkedLength;

		if (checkedOffset > mLength)
		{
			mError = true;
			return;
		}

		memcpy(v, mData + mOffset, checkedLength);
		mOffset = checkedOffset;
	}

	template <typename T>
	void read(T *v)
	{
		read(v, 1);
	}

};

class BinaryOutputStream 
{
public:
	BinaryOutputStream();
	~BinaryOutputStream();

	void writeByte(char num)
	{
		write(&num, 1);
	}


	void writeUShort(uint16_t num)
	{
		auto data = htons(num);
		write(&data, 1);
	}


	void writeUint(uint32_t num)
	{
		auto data = htonl(num);
		write(&data, 1);
	}

	void writeUlong(uint64_t num)
	{
		auto data = htonll(num);
		write(&num, 1);
	}

	void writeShort(int16_t num)
	{
		auto data = htons(num);
		write(&data, 1);
	}

	void writeInt(uint32_t num)
	{
		auto data = htonl(num);
		write(&data, 1);
	}
	
	void writeLong(uint64_t num)
	{
		auto data = htonll(num);
		write(&data, 1);
	}

	void writeString(const std::string &v)
	{		
		write(v.c_str(), v.length());
	}

	void writeBytes(const unsigned char *bytes, size_t count)
	{
		write(bytes, count);
	}

	size_t length() const
	{
		return mData.size();
	}

	const void* data() const
	{
		return mData.size() ? &mData[0] : nullptr;
	}

private:
	std::vector<char> mData;

	template <typename T>
	void write(const T *v, size_t num)
	{
		static_assert(std::is_fundamental<T>::value, "T must be a fundamental type.");
		const char *asBytes = reinterpret_cast<const char*>(v);
		mData.insert(mData.end(), asBytes, asBytes + num * sizeof(T));
	}

};

inline BinaryOutputStream::BinaryOutputStream()
{
}

inline BinaryOutputStream::~BinaryOutputStream() = default;
