#include "RoboCatPCH.h"

// 주어진 문자열에 대한 SocketAddress를 생성하는 static 함수
SocketAddressPtr SocketAddressFactory::CreateIPv4FromString(const string& inString)
{
	// 이름에서 콜론을 찾아 포트 분리
	auto pos = inString.find_last_of(':');
	string host, service;
	if (pos != string::npos)
	{
		host = inString.substr(0, pos);
		service = inString.substr(pos + 1);
	}
	else
	{
		// 포트가 지정되지 않았으므로 디폴트를 사용
		host = inString;
		service = "0";
	}
	addrinfo hint;
	memset(&hint, 0, sizeof(hint));
	hint.ai_family = AF_INET;

	addrinfo* result = nullptr;
	int error = getaddrinfo(host.c_str(), service.c_str(), &hint, &result);
	addrinfo* initResult = result;

	if (error != 0 && result != nullptr)
	{
		freeaddrinfo(initResult);
		return nullptr;
	}
	// 리스트를 순회하며 null이 아닌 주소를 찾음
	while (!result->ai_addr && result->ai_next)
	{
		result = result->ai_next;
	}

	if (!result->ai_addr)
	{
		freeaddrinfo(initResult);
		return nullptr;
	}
	// 찾은 주소를 뽑아 SocketAddress의 적당한 생성자로 객체를 생성
	auto toRet = std::make_shared<SocketAddress>(*result->ai_addr);

	freeaddrinfo(initResult);
	return toRet;
	
}