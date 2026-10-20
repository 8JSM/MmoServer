#include "pch.h"
#include "SocketUtils.h"

// 함수 포인터 변수 정의
LPFN_CONNECTEX SocketUtils::ConnectEx = nullptr;
LPFN_DISCONNECTEX SocketUtils::DisconnectEx = nullptr;
LPFN_ACCEPTEX SocketUtils::AcceptEx = nullptr;

void SocketUtils::Init() {

	WSADATA wsaData; // 윈속 초기화
	ASSERT_CRASH(::WSAStartup(MAKEWORD(2, 2), &wsaData) == 0); // 2.2버전 로드

	SOCKET dummySocket = CreateSocket(); // 함수 주소 얻기 위해 더미 소켓 생성

	ASSERT_CRASH(BindWindowsFunction(dummySocket, WSAID_CONNECTEX, reinterpret_cast<LPVOID*>(&ConnectEx)));
	ASSERT_CRASH(BindWindowsFunction(dummySocket, WSAID_DISCONNECTEX, reinterpret_cast<LPVOID*>(&DisconnectEx)));
	ASSERT_CRASH(BindWindowsFunction(dummySocket, WSAID_ACCEPTEX, reinterpret_cast<LPVOID*>(&AcceptEx)));

	::closesocket(dummySocket);
}

void SocketUtils::Clear()
{
	::WSACleanup();
}

SOCKET SocketUtils::CreateSocket()
{
	return ::WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED);
}

bool SocketUtils::BindWindowsFunction(SOCKET socket, GUID guid, LPVOID* fn)
{
	DWORD bytes = 0;

	return SOCKET_ERROR != ::WSAIoctl(
		socket, // 대상 소켓
		SIO_GET_EXTENSION_FUNCTION_POINTER, // 명령 종류
		&guid, sizeof(guid), // 입력데이터
		fn, sizeof(*fn), // 출력 버퍼
		&bytes, // 출력된 데이트 크기
		NULL, NULL
	);
}

template<typename T>
static bool SetSocketOpt(SOCKET socket, int32 level, int32 optname, T opVal)
{
	return SOCKET_ERROR != ::setsockopt(socket, level, optname, reinterpret_cast<char*>(&opVal), sizeof(T));
}

/// <summary>
/// closesocket 시 버퍼에 남은 데이터 처리 방식 제어
/// </summary>
/// <param name="socket">설정할 소켓 핸들</param>
/// <param name="onoff">1이면 지정 시간 대기 후 닫기, 0이면 기본 방식으로 닫기</param>
/// <param name="linger">버퍼를 비울 때까지 기다릴 최대 지연 시간(초)</param>
/// <returns></returns>
bool SocketUtils::SetLinger(SOCKET socket, uint16 onoff, uint16 linger)
{
	LINGER l;
	l.l_linger = linger;
	l.l_onoff = onoff;
	return SetSocketOpt(socket, SOL_SOCKET, SO_LINGER, l);
}

/// <summary>
/// 네이글 알고리즘 on /off 설정
/// </summary>
/// <param name="socket">설정할 소켓 핸들</param>
/// <param name="flag">true: 네이글 알고리즘 끄기, false: 네이글 알고리즘 켜기</param>
/// <returns></returns>
bool SocketUtils::SetTcpNoDelay(SOCKET socket, bool flag)
{
	return SetSocketOpt(socket, IPPROTO_TCP, TCP_NODELAY, flag);
}
// AcceptEx로 접속받은 소켓에 Listen소켓의 속성을 복사
bool SocketUtils::SetUpdateAcceptSocket(SOCKET socket, SOCKET listenSocket)
{
	return SetSocketOpt(socket, SOL_SOCKET, SO_UPDATE_ACCEPT_CONTEXT, listenSocket);
}

bool SocketUtils::SetUpdateConnectSocket(SOCKET socket)
{
	return SOCKET_ERROR != ::setsockopt(socket, SOL_SOCKET, SO_UPDATE_CONNECT_CONTEXT, nullptr, 0);
}

bool SocketUtils::SetRecvBufferSize(SOCKET socket, int32 size)
{
	return SetSocketOpt(socket, SOL_SOCKET, SO_RCVBUF, size);
}

bool SocketUtils::SetSendBufferSize(SOCKET socket, int32 size)
{
	return SetSocketOpt(socket, SOL_SOCKET, SO_SNDBUF, size);
}

bool SocketUtils::SetReuseAddress(SOCKET socket, bool flag)
{
	return SetSocketOpt(socket, SOL_SOCKET, SO_REUSEADDR, flag);
}

bool SocketUtils::Bind(SOCKET socket, const NetAddress& netAddr)
{
	SOCKADDR_IN sockAddr = netAddr.GetSockAddr();
	return SOCKET_ERROR != ::bind(socket, reinterpret_cast<SOCKADDR*>(&sockAddr), sizeof(sockAddr));
}

bool SocketUtils::BindAnyAddress(SOCKET socket, uint16 port)
{
	SOCKADDR_IN myAddress;
	::memset(&myAddress, 0, sizeof(myAddress));
	myAddress.sin_family = AF_INET;
	myAddress.sin_addr.s_addr = ::htonl(INADDR_ANY);
	myAddress.sin_port = ::htons(port);

	return SOCKET_ERROR != ::bind(socket, reinterpret_cast<SOCKADDR*>(&myAddress), sizeof(myAddress));
}

bool SocketUtils::Listen(SOCKET socket, int32 backlog)
{
	return SOCKET_ERROR != ::listen(socket, backlog);
}

void SocketUtils::Close(SOCKET& socket)
{
	if (socket != INVALID_SOCKET)
	{
		::closesocket(socket);
		socket = INVALID_SOCKET;
	}
}