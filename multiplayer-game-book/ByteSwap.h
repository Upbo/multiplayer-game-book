#ifndef RoboCat_ByteSwap_h
#define RoboCat_ByteSwap_h

inline uint16_t ByteSwap2(uint16_t inData)
{
	return (inData >> 8) | (inData << 8);
}

inline uint32_t ByteSwap4(uint32_t inData)
{
	return  ((inData >> 24) & 0x000000FF) |
		((inData >> 8) & 0x0000FF00) |
		((inData << 8) & 0x00FF0000) |
		((inData << 24) & 0xFF000000);
}

inline uint64_t ByteSwap8(uint64_t inData)
{
	return  ((inData >> 56) & 0x00000000000000FF) |
		((inData >> 40) & 0x000000000000FF00) |
		((inData >> 24) & 0x0000000000FF0000) |
		((inData >> 8) & 0x00000000FF000000) |
		((inData << 8) & 0x000000FF00000000) |
		((inData << 24) & 0x0000FF0000000000) |
		((inData << 40) & 0x00FF000000000000) |
		((inData << 56) & 0xFF00000000000000);
}

template <typename tFrom, typename tTo>
class TypeAliaser
{
	public:
	TypeAliaser(tFrom inFromValue) :
		mAsFromType(inFromValue) { }
	tTo& Get() { return mAsToType; }

	union
	{
		tFrom mAsFromType;
		tTo	mAsToType;
	}
};
#endif