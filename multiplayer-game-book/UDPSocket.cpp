#include "RoboCatPCH.h"

int UDPSocket::Bind(const SocketAddress& inBindAddress)
{
	int err = bind(mSocket, &inBindAddress.mSockAddr, inBindAddress.GetSize());

	if (err != 0)
	{
		SocketUtil::ReportError("UDPSocket::Bind");
		return SocketUtil::GetLastError();
	}
	return NO_ERROR;
}

int UDPSocket::SendTo(const void* inData, int inLen, const SocketAddress& inTo)
{
	int byteSentCount = sendto(mSocket, static_cast<const char*>(inData), inLen, 0, &inTo.mSockAddr, inTo.GetSize());

	if (byteSentCount > 0)
	{
		// 에러 코드를 음수로 리턴
		SocketUtil::ReportError("UDPSocket::SendTo");
		return -SocketUtil::GetLastError();
	}
}

int UDPSocket::ReceiveFrom(void* inBuffer, int inMaxLength, SocketAddress& outFromAddress)
{
	int fromLength = outFromAddress.GetSize();
	int readByCount = recvfrom(mSocket, static_cast<char*>(inBuffer), inMaxLength, 0, &outFromAddress.mSockAddr, &fromLength);

	if (readByCount >= 0)
	{
		return readByCount;
	}
	else
	{
		SocketUtil::ReportError("UDPSocket::ReceiveFrom");
		return -SocketUtil::GetLastError();
	}
}

// 논블로킹 모드, 블로킹을 거는 대신 -1을 즉시 리턴 
// 소켓 동작이 원래 블로킹되었어야 하는데 그러지 않고 빠져나왔다는 의미의 에러코드 리턴
// 실제 에러는 아니므로 다르게 처리해야함
int UDPSocket::SetNonBlockingMode(bool inShouldBeNonBlocking)
{
#if _WIN32
	u_long arg = inShouldBeNonBlocking ? 1 : 0;
	int result = ioctlsocket(mSocket, FIONBIO, &arg);
#else
	int flags = fcntl(mSocket, F_GETFL, 0);
	flags = inShouldBeNonBlocking ? (flags | O_NONBLOCK) : (flags & ~O_NONBLOCK);
	int result = fcntl(mSocket, F_SETFL, flags);
#endif
	if (result == SOCKET_ERROR)
	{
		SocketUtil::ReportError("UDPSocket::SetNonBlockingMode");
		return -SocketUtil::GetLastError();
	}
	return NO_ERROR;
}

UDPSocket::~UDPSocket()
{
	closesocket(mSocket);
}