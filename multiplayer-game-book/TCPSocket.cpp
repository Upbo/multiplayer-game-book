#include "RoboCatPCH.h"

// 서버에 접속해 핸드셰이킹 절차를 시작
int TCPSocket::Connect(const SocketAddress& inAddress)
{
	// 최초 SYN 패킷을 대상 호스트에 전송하여 TCP 핸드셰이킹을 개시
	// 호스트에 해당 포트로 바인딩한 리스닝 모드 소켓이 있는 경우, 서버 호스트는 accept()를 호출해 이 핸드셰이킹을 처리
	// 옵션 지정하지 않을 시 connect() 호출 스레드는 연결이 수락되거나 시간이 초과될때 까지 블로킹됨
	int err = connect(mSocket, &inAddress.mSockAddr, inAddress.GetSize());
	if (err < 0)
	{
		SocketUtil::ReportError("TCPSocket::Connect");
		return -SocketUtil::GetLastError();
	}
	return NO_ERROR;
}

//핸드셰이킹 리스닝
int TCPSocket::Listen(int inBackLog)
{
	// mSocket: 리스닝 모드에 둘 소켓, inBackLog: 들어오는 연결을 대기열에 둘 최대 숫자
	// 소켓이 리스닝 모드에 있으면 외부에서 들어오는 TCP 핸드셰이킹 첫 단계 요청을 받아 이를 대기열에 저장해둠 
	// 이후 프로세스가 accept()를 호출하면 저장해둔 연결 요청의 그 다음 핸드셰이킹 단계 속행
	// 대기열이 가득차면 그 이후로 들어오려는 연결은 끊어짐
	int err = listen(mSocket, inBackLog);
	if (err < 0)
	{
		SocketUtil::ReportError("TCPSocket::Listen");
		return -SocketUtil::GetLastError();
	}
	return NO_ERROR;
}

// 들어오는 연결을 받아 TCP 핸드셰이킹을 계속 진행
TCPSocketPtr TCPSocket::Accept(SocketAddress& inFromAddress)
{
	int length = inFromAddress.GetSize();
	SOCKET newSocket = accept(mSocket, &inFromAddress.mSockAddr, &length);
	if (newSocket != INVALID_SOCKET)
	{
		return TCPSocketPtr(new TCPSocket(newSocket));
	}
	else
	{
		SocketUtil::ReportError("TCPSocket::Accept");
		return nullptr;
	}
}

int TCPSocket::Send(const void* inData, int inLen)
{
	int byteSentCount = send(mSocket, static_cast<const char*>(inData), inLen, 0);
	if (byteSentCount < 0)
	{
		SocketUtil::ReportError("TCPSocket::Send");
		return -SocketUtil::GetLastError();
	}
	return byteSentCount;
}

int TCPSocket::Receive(void* inData, int inLen)
{
	int byteReceivedCount = recv(mSocket, static_cast<char*>(inData), inLen, 0);
	if (byteReceivedCount < 0)
	{
		SocketUtil::ReportError("TCPSocket::Receive");
		return -SocketUtil::GetLastError();
	}
	return byteReceivedCount;
}

int TCPSocket::Bind(const SocketAddress& inBindAddress)
{
	int err = bind(mSocket, &inBindAddress.mSockAddr, inBindAddress.GetSize());
	if (err != 0)
	{
		SocketUtil::ReportError("TCPSocket::Bind");
		return -SocketUtil::GetLastError();
	}
	return NO_ERROR;
}

TCPSocket::~TCPSocket()
{
#if _WIN32
	closesocket(mSocket);
#else
	close(mSocket);
#endif
}