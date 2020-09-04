#include "RoboCatPCH.h"

bool SocketUtil::StaticInit()
{
#if _WIN32
	WSADATA wsaData;
	int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != NO_ERROR)
	{
		ReportError("Starting Up");
		return false;
	}
#endif
	return true;

}

void SocketUtil::CleanUp()
{
#if _WIN32
	WSACleanup();
#endif
}

void SocketUtil::ReportError(const char* inOperationDesc)
{
#if _WIN32
	LPVOID lpMsgBuf;
	DWORD errorNum = GetLastError();
	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER |
		FORMAT_MESSAGE_FROM_SYSTEM |
		FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL,
		errorNum,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)&lpMsgBuf,
		0,
		NULL);
	
	LOG("Error %s: %d- %s", inOperationDesc, errorNum, lpMsgBuf);
#else

#endif
}

int SocketUtil::GetLastError()
{
#if _WIN32
	return WSAGetLastError();
#else
	return errno;
#endif

}

int SocketUtil::Select(
	const vector<TCPSocketPtr>* inReadSet, 
	vector<TCPSocketPtr>* outReadSet, 
	const vector<TCPSocketPtr>* inWriteSet, 
	vector<TCPSocketPtr>* outWriteSet, 
	const vector<TCPSocketPtr>* inExceptSet, 
	vector<TCPSocketPtr>* outExceptSet)
{
	// ���� vector�� fd_set�� ä��
	fd_set read, write, except;
	fd_set* readPtr = FillSetFromVector(read, inReadSet);
	fd_set* writePtr = FillSetFromVector(write, inWriteSet);
	fd_set* exceptPtr = FillSetFromVector(write, inExceptSet);

	// 0: POSIX���� �Լ��� �ѱ�� ������ �� �� �ִ��� ������ �����쿡�� ������ �������� �ƴ϶� ���������̹Ƿ� �� ���ڴ� ���õ�
	// readPtr: ���� �÷����� ����Ű�� �������̸� fd_set��, ���� �غ� �Ǿ����� Ȯ���ϰ� ���� ����
	// �� ���տ� ���Ե� �������� ��Ŷ�� �����ϸ� select()�Լ��� �ִ��� ���� ���ŷ ���¿��� �������� ȣ�� ������� ������
	// ���� ������ �Ǿ�� ��Ŷ�� �������� ���� ������ ���� ���տ��� �ɷ���. ���� select()�� ���������� ���� �� ���տ� ���� �ִ� ������ ���� �����Ͱ� �ִ� ����
	// writePtr: ��������� üũ�ϰ� ���� ������ ���� fd_set, select()�� ������ �� writePtr�� �����ִ� ������ ���� �۾��� �ص� ȣ�⽺���尡 ���ŷ �ɸ��� ����
	// exceptPtr: ������ �����ϰ��� �ϴ� ������ ���� fd_set, select()�� ������ �� exceptPtr�� �����ִ� ������ ������ �߻��� �� ��
	// nullptr: �ִ� ���� �ð�(timeout)�� ����. �ð������� �ΰ� ���� ������ nullptr
	int toRet = select(0, readPtr, writePtr, exceptPtr, nullptr);

	if (toRet > 0)
	{
		FillVectorFromSet(outReadSet, inReadSet, read);
		FillVectorFromSet(outWriteSet, inWriteSet, write);
		FillVectorFromSet(outExceptSet, inExceptSet, except);
	}
	// readPtr, writePtr, exceptPtr�� �����ִ� ������ ������ ����
	return toRet;
}

UDPSocketPtr SocketUtil::CreateUDPSocket(SocketAddressFamily inFamily)
{
	SOCKET s = socket(inFamily, SOCK_DGRAM, IPPROTO_UDP);
	if (s != INVALID_SOCKET)
	{
		// ���� ���� ������ �����ؾ߸� ��ü�� ����
		return UDPSocketPtr(new UDPSocket(s));
	}
	else
	{
		ReportError("SocketUtil::CreateUDPSocket");
		return nullptr;
	}
}

TCPSocketPtr SocketUtil::CreateTCPSocket(SocketAddressFamily inFamily)
{
	SOCKET s = socket(inFamily, SOCK_STREAM, IPPROTO_TCP);
	if (s != INVALID_SOCKET)
	{
		return TCPSocketPtr(new TCPSocket(s));
	}
	else
	{
		ReportError("SocketUtil::CreateTCPSocket");
		return nullptr;
	}
	return TCPSocketPtr();
}
// ������ Vector�� fd_set���� ��ȯ
inline fd_set* SocketUtil::FillSetFromVector(fd_set& outSet, const vector<TCPSocketPtr>* inSockets)
{
	if (inSockets)
	{
		// fd_set �ʱ�ȭ
		FD_ZERO(&outSet);
		for (const TCPSocketPtr& socket : *inSockets)
		{
			// fd_set�� ������ �߰�
			FD_SET(socket->mSocket, &outSet);
		}
		return &outSet;
	}
	else
	{
		return nullptr;
	}
}
// fd_set�� Vector�� ��ȯ
// fd_set�� ������ ���� ��ȸ�ϱ� ����
inline void SocketUtil::FillVectorFromSet(vector<TCPSocketPtr>* outSockets, const vector<TCPSocketPtr>* inSockets, const fd_set& inSet)
{
	if (inSockets && outSockets)
	{
		outSockets->clear();
		for (const TCPSocketPtr& socket : *inSockets)
		{
			// fd_set�� Ư�� ������ ���ԵǾ����� Ȯ��
			if (FD_ISSET(socket->mSocket, &inSet))
			{
				outSockets->push_back(socket);
			}
		}
	}
}
