#include "RoboCatPCH.h"

void OutputMemoryStream::ReallocBuffer(uint32_t inNewLength)
{
	mBuffer = static_cast<char*>(std::realloc(mBuffer, inNewLength));
	// realloc ȣ���� ������ ��� ó��
	// ...
	mCapacity = inNewLength;
}

void OutputMemoryStream::Write(const void* inData, size_t inByteCount)
{
	// �ϴ� ������ ����� Ȯ���ص�
	uint32_t resultHead = mHead + static_cast<uint32_t>(inByteCount);
	if (resultHead > mCapacity)
	{
		// �����͸� ����ϴ� ���� mBuffer�� �뷮�� ���ڶ� ��� Ȯ��
		ReallocBuffer(std::max(mCapacity * 2, resultHead));
	}
	// ������ ���� �տ� ����
	std::memcpy(mBuffer + mHead, inData, inByteCount);
	// mHead�� �������� ���� ��Ͽ� ���
	mHead = resultHead;
}

void InputMemoryStream::Read(void* outData, uint32_t inByteCount)
{
	uint32_t resultHead = mHead + inByteCount;
	if (resultHead > mCapacity)
	{
		//handle error, no data to read!
		//...
	}

	std::memcpy(outData, mBuffer + mHead, inByteCount);

	mHead = resultHead;
}