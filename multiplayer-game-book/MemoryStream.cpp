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
		ReallocBuffer(std::max(mCapacity * 2, resultHead));
	}
	// ������ ���� �տ� ����
	std::memcpy(mBuffer + mHead, inData, inByteCount);
	// mHead�� �������� ���� ��Ͽ� ���
	mHead = resultHead;
}