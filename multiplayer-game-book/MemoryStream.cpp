#include "RoboCatPCH.h"

void OutputMemoryStream::ReallocBuffer(uint32_t inNewLength)
{
	mBuffer = static_cast<char*>(std::realloc(mBuffer, inNewLength));
	// realloc 호출이 실패할 경우 처리
	// ...
	mCapacity = inNewLength;
}

void OutputMemoryStream::Write(const void* inData, size_t inByteCount)
{
	// 일단 공간을 충분히 확보해둠
	uint32_t resultHead = mHead + static_cast<uint32_t>(inByteCount);
	if (resultHead > mCapacity)
	{
		// 데이터를 기록하는 도중 mBuffer의 용량이 모자랄 경우 확장
		ReallocBuffer(std::max(mCapacity * 2, resultHead));
	}
	// 버퍼의 제일 앞에 복사
	std::memcpy(mBuffer + mHead, inData, inByteCount);
	// mHead를 전진시켜 다음 기록에 대비
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