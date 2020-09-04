#include <cstdlib>
#include <cstdint>
#include <type_traits>

#define STREAM_ENDIANNESS 0
#define PLATFORM_ENDIANNESS 0

class GameObject;
class LinkingContext;

class OutputMemoryStream
{
	public:
	OutputMemoryStream() :
		mBuffer(nullptr), mHead(0), mCapacity(0)
	{
		ReallocBuffer(32);
	}

	~OutputMemoryStream() 
	{ 
		std::free(mBuffer); 
	}

	// 스트림의 데이터 시작 위치 포인터를 구함
	const char* GetBufferPtr() const { return mBuffer; }
	uint32_t GetLength() const { return mHead; }

	void Write(const void* inData, size_t inByteCount);
	void Write(uint32_t inData) { Write(&inData, sizeof(inData)); }
	void Write(int32_t inData) { Write(&inData, sizeof(inData)); }

	private:
	void ReallocBuffer(uint32_t inNewLength);
	
	char* mBuffer;
	uint32_t mHead;
	uint32_t mCapacity;
	
};