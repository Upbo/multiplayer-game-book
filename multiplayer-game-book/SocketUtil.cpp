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
	// 받은 vector로 fd_set을 채움
	fd_set read, write, except;
	fd_set* readPtr = FillSetFromVector(read, inReadSet);
	fd_set* writePtr = FillSetFromVector(write, inWriteSet);
	fd_set* exceptPtr = FillSetFromVector(write, inExceptSet);

	// 0: POSIX에선 함수에 넘기는 소켓의 값 중 최댓값을 넣으나 윈도우에선 소켓이 정수형이 아니라 포인터형이므로 이 인자는 무시됨
	// readPtr: 소켓 컬렉션을 가리키는 포인터이며 fd_set형, 읽을 준비가 되었는지 확인하고 싶은 소켓
	// 이 집합에 포함된 소켓으로 패킷이 도착하면 select()함수는 최대한 빨리 블로킹 상태에서 빠져나와 호출 스레드로 리턴함
	// 리턴 시점이 되어서도 패킷을 수신하지 못한 소켓은 전부 집합에서 걸러짐. 따라서 select()가 리턴했을때 아직 이 집합에 남아 있는 소켓은 읽을 데이터가 있는 상태
	// writePtr: 쓰기용으로 체크하고 싶은 소켓을 담은 fd_set, select()가 리턴한 뒤 writePtr에 남아있는 소켓은 쓰기 작업을 해도 호출스레드가 블로킹 걸리지 않음
	// exceptPtr: 에러를 점검하고자 하는 소켓을 담은 fd_set, select()가 리턴한 뒤 exceptPtr에 남아있는 소켓은 에러가 발생한 것 들
	// nullptr: 최대 제한 시간(timeout)을 지정. 시간제한을 두고 싶지 않으면 nullptr
	int toRet = select(0, readPtr, writePtr, exceptPtr, nullptr);

	if (toRet > 0)
	{
		FillVectorFromSet(outReadSet, inReadSet, read);
		FillVectorFromSet(outWriteSet, inWriteSet, write);
		FillVectorFromSet(outExceptSet, inExceptSet, except);
	}
	// readPtr, writePtr, exceptPtr에 남아있는 소켓의 개수를 리턴
	return toRet;
}

UDPSocketPtr SocketUtil::CreateUDPSocket(SocketAddressFamily inFamily)
{
	SOCKET s = socket(inFamily, SOCK_DGRAM, IPPROTO_UDP);
	if (s != INVALID_SOCKET)
	{
		// 소켓 생성 동작이 성공해야만 객체를 리턴
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
// 소켓의 Vector를 fd_set으로 변환
inline fd_set* SocketUtil::FillSetFromVector(fd_set& outSet, const vector<TCPSocketPtr>* inSockets)
{
	if (inSockets)
	{
		// fd_set 초기화
		FD_ZERO(&outSet);
		for (const TCPSocketPtr& socket : *inSockets)
		{
			// fd_set에 소켓을 추가
			FD_SET(socket->mSocket, &outSet);
		}
		return &outSet;
	}
	else
	{
		return nullptr;
	}
}
// fd_set을 Vector로 변환
// fd_set의 소켓을 쉽게 순회하기 위함
inline void SocketUtil::FillVectorFromSet(vector<TCPSocketPtr>* outSockets, const vector<TCPSocketPtr>* inSockets, const fd_set& inSet)
{
	if (inSockets && outSockets)
	{
		outSockets->clear();
		for (const TCPSocketPtr& socket : *inSockets)
		{
			// fd_set에 특정 소켓이 포함되었는지 확인
			if (FD_ISSET(socket->mSocket, &inSet))
			{
				outSockets->push_back(socket);
			}
		}
	}
}
