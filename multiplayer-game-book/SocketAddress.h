// ���� ���� Ŭ����
class SocketAddress
{
	// 4����Ʈ IP�ּҿ� ��Ʈ ��ȣ�� �޴� ������
	// ���� sockAddr ����ü�� ������ ä��
	public:
	SocketAddress(uint32_t inAddress, uint16_t inPort)
	{
		// IPv4
		GetAsSockAddrIn()->sin_family = AF_INET;
		GetAsSockAddrIn()->sin_addr.S_un.S_addr = htonl(inAddress);
		GetAsSockAddrIn()->sin_port = htons(inPort);
	}

	// sockAddr ����ü�� �޾� ���� mSockAddr �ʵ忡 �����ϴ� ������
	// ��Ʈ��ũ API�� sockaddr�� ���������� �̸� SocketAddress ��ü�� �����ϰ� ������ ������
	SocketAddress(const sockaddr& inSockAddr)
	{
		memcpy(&mSockAddr, &inSockAddr, sizeof(sockaddr));
	}

	// sockAddr ���̸� �Ѱ�����Ҷ� �����
	size_t GetSize() const { return sizeof(sockaddr); }

	private:
	friend class UDPSocket;
	sockaddr mSockAddr;
	
	sockaddr_in* GetAsSockAddrIn()
	{
		return reinterpret_cast<sockaddr_in*>(&mSockAddr);
	}

};
// ���� ������ ���� �ּҸ� �����ؼ� ��� �Ҷ�, �޸� ������ �Ű� �� �ʿ䰡 ������ ���� �����ص�
using SocketAddressPtr = std::shared_ptr<SocketAddress>;
