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
		ReallocBuffer(std::max(mCapacity * 2, resultHead));
	}
	// 버퍼의 제일 앞에 복사
	std::memcpy(mBuffer + mHead, inData, inByteCount);
	// mHead를 전진시켜 다음 기록에 대비
	mHead = resultHead;
}