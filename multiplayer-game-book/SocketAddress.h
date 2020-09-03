// 소켓 래핑 클래스
class SocketAddress
{
	// 4바이트 IP주소와 포트 번호를 받는 생성자
	// 내부 sockAddr 구조체의 값으로 채움
	public:
	SocketAddress(uint32_t inAddress, uint16_t inPort)
	{
		// IPv4
		GetAsSockAddrIn()->sin_family = AF_INET;
		GetAsSockAddrIn()->sin_addr.S_un.S_addr = htonl(inAddress);
		GetAsSockAddrIn()->sin_port = htons(inPort);
	}

	// sockAddr 구조체를 받아 내부 mSockAddr 필드에 복사하는 생성자
	// 네트워크 API가 sockaddr을 리턴했을때 이를 SocketAddress 객체로 래핑하고 싶을때 유용함
	SocketAddress(const sockaddr& inSockAddr)
	{
		memcpy(&mSockAddr, &inSockAddr, sizeof(sockaddr));
	}

	// sockAddr 길이를 넘겨줘야할때 사용함
	size_t GetSize() const { return sizeof(sockaddr); }

	private:
	friend class UDPSocket;
	sockaddr mSockAddr;
	
	sockaddr_in* GetAsSockAddrIn()
	{
		return reinterpret_cast<sockaddr_in*>(&mSockAddr);
	}

};
// 여러 곳에서 소켓 주소를 공유해서 써야 할때, 메모리 정리를 신경 쓸 필요가 없도록 따로 선언해둠
using SocketAddressPtr = std::shared_ptr<SocketAddress>;
