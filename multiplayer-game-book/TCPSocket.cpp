#include "RoboCatPCH.h"

// ������ ������ �ڵ����ŷ ������ ����
int TCPSocket::Connect(const SocketAddress& inAddress)
{
	// ���� SYN ��Ŷ�� ��� ȣ��Ʈ�� �����Ͽ� TCP �ڵ����ŷ�� ����
	// ȣ��Ʈ�� �ش� ��Ʈ�� ���ε��� ������ ��� ������ �ִ� ���, ���� ȣ��Ʈ�� accept()�� ȣ���� �� �ڵ����ŷ�� ó��
	// �ɼ� �������� ���� �� connect() ȣ�� ������� ������ �����ǰų� �ð��� �ʰ��ɶ� ���� ���ŷ��
	int err = connect(mSocket, &inAddress.mSockAddr, inAddress.GetSize());
	if (err < 0)
	{
		SocketUtil::ReportError("TCPSocket::Connect");
		return -SocketUtil::GetLastError();
	}
	return NO_ERROR;
}

//�ڵ����ŷ ������
int TCPSocket::Listen(int inBackLog)
{
	// mSocket: ������ ��忡 �� ����, inBackLog: ������ ������ ��⿭�� �� �ִ� ����
	// ������ ������ ��忡 ������ �ܺο��� ������ TCP �ڵ����ŷ ù �ܰ� ��û�� �޾� �̸� ��⿭�� �����ص� 
	// ���� ���μ����� accept()�� ȣ���ϸ� �����ص� ���� ��û�� �� ���� �ڵ����ŷ �ܰ� ����
	// ��⿭�� �������� �� ���ķ� �������� ������ ������
	int err = listen(mSocket, inBackLog);
	if (err < 0)
	{
		SocketUtil::ReportError("TCPSocket::Listen");
		return -SocketUtil::GetLastError();
	}
	return NO_ERROR;
}

// ������ ������ �޾� TCP �ڵ����ŷ�� ��� ����
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