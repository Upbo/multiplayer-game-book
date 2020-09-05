#include "RoboCatPCH.h"

void OutputMemoryBitStream::WriteBits(uint8_t inData, size_t inBitCount)
{
	uint32_t nextBitHead = mBitHead + static_cast<uint32_t>(inBitCount);
	if (nextBitHead > mBitCapacity)
	{
		ReallocBuffer(std::max(mBitCapacity * 2, nextBitHead));
	}
	// byteOffset : ��Ʈ ��带 8�� ����
	// bitOffset : 8�� ���� ��Ʈ����� ������
	uint32_t byteOffset = mBitHead >> 3;
	uint32_t bitOffset = mBitHead & 0x7;

	// ���� ó�� �� ����Ʈ���� �� ��Ʈ�� ������ ���
	uint8_t currentMask = ~(0xFF << bitOffset);
	mBuffer[byteOffset] = (mBuffer[byteOffset] & currentMask) | (inData << bitOffset);

	// ����� ����Ʈ�� ���� �� ��Ʈ�� �����ֳ� ���
	uint32_t bitsFreeThisByte = 8 - bitOffset;

	// ��Ʈ�� ���ڶ�� ���� ����Ʈ�� �ѱ�
	if (bitsFreeThisByte < inBitCount)
	{
		// ���� ����Ʈ�� ������ ��Ʈ�� ���
		mBuffer[byteOffset + 1] = inData >> bitsFreeThisByte;
	}
	
	mBitHead = nextBitHead;
}

void OutputMemoryBitStream::WriteBits(const void* inData, size_t inBitCount)
{
	const char* srcByte = static_cast<const char*>(inData);

	// ����Ʈ�� �ϳ��� ��� ���
	while (inBitCount > 8)
	{
		WriteBits(*srcByte, 8);
		++srcByte;
		inBitCount -= 8;
	}
	// ���� ���� ��Ʈ�� ���
	if (inBitCount > 0)
	{
		WriteBits(*srcByte, inBitCount);
	}
}