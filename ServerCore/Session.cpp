#include "pch.h"
#include "Session.h"


Session::Session() : _recvBuffer(BUFFER_SIZE)
{
	_socket = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED);
}

Session::~Session()
{
	::closesocket(_socket);
}

HANDLE Session::GetHandle()
{
	return reinterpret_cast<HANDLE>(_socket);
}

void Session::Dispatch(IocpEvent* iocpEvent, int32 numOfBytes)
{
	switch (iocpEvent->eventType)
	{

	default:
		break;
	}
}

// 데이터 송신
void Session::Send(std::shared_ptr<SendBuffer> sendBuffer)
{
	if (IsConnected() == false) return;

	bool registerSend = false;

	{
		WRITE_LOCK;

		_sendQueue.push(sendBuffer);
		
		if (_sendRegistered.exchange(true) == false)
			registerSend = true;
	}

	if (registerSend)
		RegisterSend();
}

void Session::RegisterSend()
{
	if (IsConnected() == false) return;

	_sendEvent.Init();
	_sendEvent.owner = shared_from_this();
}

void Session::ProcessSend(int32 numOfBytes)
{

}

// 데이터 수신
void Session::RegisterRecv()
{

}

void Session::ProcessRecv(int32 numOfBytes)
{

}

// 연결 및 해제
void Session::ProcessConnect()
{

}

void Session::Disconnect(const WCHAR* cause)
{

}

void Session::ProcessDisconnect()
{

}

void Session::RegisterDisconnect()
{

}

// 에러
void Session::HandleError(int32 errorCode)
{

}