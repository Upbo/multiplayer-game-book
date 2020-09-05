#include "RoboCatPCH.h"

void OutputMemoryBitStream::WriteBits(uint8_t inData, size_t inBitCount)
{
	uint32_t nextBitHead = mBitHead + static_cast<uint32_t>(inBitCount);
	if (nextBitHead > mBitCapacity)
	{
		ReallocBuffer(std::max(mBitCapacity * 2, nextBitHead));
	}
	// byteOffset : 비트 헤드를 8로 나눔
	// bitOffset : 8로 나눈 비트헤드의 나머지
	uint32_t byteOffset = mBitHead >> 3;
	uint32_t bitOffset = mBitHead & 0x7;

	// 현재 처리 중 바이트에서 몇 비트를 남길지 계산
	uint8_t currentMask = ~(0xFF << bitOffset);
	mBuffer[byteOffset] = (mBuffer[byteOffset] & currentMask) | (inData << bitOffset);

	// 기록할 바이트에 아직 몇 비트가 남아있나 계산
	uint32_t bitsFreeThisByte = 8 - bitOffset;

	// 비트가 모자라면 다음 바이트에 넘김
	if (bitsFreeThisByte < inBitCount)
	{
		// 다음 바이트에 나머지 비트를 기록
		mBuffer[byteOffset + 1] = inData >> bitsFreeThisByte;
	}
	
	mBitHead = nextBitHead;
}

void OutputMemoryBitStream::WriteBits(const void* inData, size_t inBitCount)
{
	const char* srcByte = static_cast<const char*>(inData);

	// 바이트를 하나씩 모두 기록
	while (inBitCount > 8)
	{
		WriteBits(*srcByte, 8);
		++srcByte;
		inBitCount -= 8;
	}
	// 아직 남은 비트를 기록
	if (inBitCount > 0)
	{
		WriteBits(*srcByte, inBitCount);
	}
}

void OutputMemoryBitStream::ReallocBuffer(uint32_t inNewBitCapacity)
{
	if (mBuffer == nullptr)
	{
		//just need to memset on first allocation
		mBuffer = static_cast<char*>(std::malloc(inNewBitCapacity >> 3));
		memset(mBuffer, 0, inNewBitCapacity >> 3);
	}
	else
	{
		//need to memset, then copy the buffer
		char* tempBuffer = static_cast<char*>(std::malloc(inNewBitCapacity >> 3));
		memset(tempBuffer, 0, inNewBitCapacity >> 3);
		memcpy(tempBuffer, mBuffer, mBitCapacity >> 3);
		std::free(mBuffer);
		mBuffer = tempBuffer;
	}

	//handle realloc failure
	//...
	mBitCapacity = inNewBitCapacity;
}
