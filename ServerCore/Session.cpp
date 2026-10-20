#include "pch.h"
#include "Session.h"
#include "SocketUtils.h"
#include "Service.h"

Session::Session() : _recvBuffer(BUFFER_SIZE)
{
	_socket = SocketUtils::CreateSocket();
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
	case EventType::Connect:
		ProcessConnect();
		break;
	case EventType::Disconnect:
		ProcessDisconnect();
		break;
	case EventType::Recv:
		ProcessRecv(numOfBytes);
		break;
	case EventType::Send:
		ProcessSend(numOfBytes);
		break;

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

	
	std::vector<WSABUF> wsaBufs; // OS에게 전달할 버퍼 주소
	wsaBufs.reserve(32);

	{
		WRITE_LOCK;

		while (_sendQueue.empty() == false)
		{
			std::shared_ptr<SendBuffer> sendBuffer = _sendQueue.front();

			_sendQueue.pop();

			_sendEvent.sendBuffers.push_back(sendBuffer);

			WSABUF wsaBuf;
			wsaBuf.buf = reinterpret_cast<CHAR*>(sendBuffer->Buffer());
			wsaBuf.len = sendBuffer->WriteSize();

			wsaBufs.push_back(wsaBuf);

		}

	}
						 //nullptr은 OS Error
	DWORD bytesSent = 0; // WSASend 함수 문법 상 필요, 비동기 작업에서는 사용하지 않음

	if (SOCKET_ERROR == ::WSASend(
		_socket,							// 소켓
		wsaBufs.data(),						// 버퍼 배열 시작 주소
		static_cast<DWORD>(wsaBufs.size()),	// 버퍼 총 개수
		&bytesSent,							// 즉시 보낸 크기 (비동기는 불필요하지만 문법상 필요)
		0,									// 세부 전용 옵션, 기본값 사용
		&_sendEvent.overlapped,				// Overlapped 구조체 주소
		nullptr								// 콜백 함수 포인터 IOCP를 사용하기 때문에 불필요
	))
	{
		int32 errorCode = ::WSAGetLastError(); // SOCKET_ERROR면 에러 번호 반환
		if (errorCode != WSA_IO_PENDING) // 비동기 작업이므로 '보류 중' 정상
		{
			HandleError(errorCode);
			_sendEvent.owner = nullptr;
			_sendRegistered.store(false);
		}
	}
}

void Session::ProcessSend(int32 numOfBytes)
{
	_sendEvent.owner = nullptr; // 작업 완료 후 소유권 해제

	if (numOfBytes == 0)
	{
		Disconnect(L"ProcessSend : 0 bytes transferred");
		return;
	}

	OnSend(numOfBytes);

	WRITE_LOCK;

	if (_sendQueue.empty())
	{
		_sendRegistered.store(false);
	}
	else
	{
		RegisterSend();
	}



}

// 데이터 수신
void Session::RegisterRecv()
{

	if (IsConnected() == false) return;

	_recvEvent.Init();
	_recvEvent.owner = shared_from_this();

	WSABUF wsaBuf;
	wsaBuf.buf = reinterpret_cast<CHAR*>(_recvBuffer.WritePos());
	wsaBuf.len = static_cast<ULONG>(_recvBuffer.FreeSize());

	DWORD numOfBytes = 0;
	DWORD flags = 0;

	if (SOCKET_ERROR == ::WSARecv(
		_socket,
		&wsaBuf,
		1,
		&numOfBytes,
		&flags,
		&_recvEvent.overlapped,
		nullptr
	))
	{
		int32 errorCode = ::WSAGetLastError();
		if (errorCode != WSA_IO_PENDING)
		{
			HandleError(errorCode);
			_recvEvent.owner = nullptr;
		}
	}

}

void Session::ProcessRecv(int32 numOfBytes)
{
	_recvEvent.owner = nullptr;

	if (numOfBytes == 0)
	{
		Disconnect(L"ProcessRecv : 0 bytes transferred");
		return;
	}

	int32 dataSize = _recvBuffer.DataSize();
	int32 processLen = OnRecv(_recvBuffer.ReadPos(), dataSize);

	if (processLen < 0 || processLen > dataSize || _recvBuffer.OnRead(processLen) == false)
	{
		Disconnect(L"OnRecv :  Invalid processed length");
		return;
	}

	_recvBuffer.Clean();
	
	RegisterRecv();
}

// 연결 및 해제
void Session::ProcessConnect()
{
	_connectEvent.owner = nullptr;
	_connected.store(true);

	if (auto service = _service.lock())
		service->AddSession(GetSessionRef());

	OnConnected();

	RegisterRecv();
}

void Session::Disconnect(const WCHAR* cause)
{
	if (_connected.exchange(false) == false) return; // 소켓 중복 해제 위험 방지

	wprintf(L"[Disconnect] Cause : %s\n", cause);

	if (auto service = _service.lock())
		service->ReleaseSession(GetSessionRef());

	OnDisconnected();
	SocketUtils::Close(_socket);
	RegisterDisconnect();
}

void Session::ProcessDisconnect()
{
	_disconnectEvent.owner = nullptr;
}

void Session::RegisterDisconnect()
{
	_disconnectEvent.Init();
	_disconnectEvent.owner = shared_from_this();

	if (SocketUtils::DisconnectEx)
	{
		if (false == SocketUtils::DisconnectEx(
			_socket,
			&_disconnectEvent.overlapped,
			TF_REUSE_SOCKET,
			0
		))
		{
			int32 errorCode = WSAGetLastError();

			if (errorCode != WSA_IO_PENDING)
			{
				HandleError(errorCode);
				_disconnectEvent.owner = nullptr;
			}
		}
	}
	else
	{
		ProcessDisconnect();
	}
}

// 에러
void Session::HandleError(int32 errorCode)
{
	switch (errorCode)
	{
	case WSAECONNRESET:		//클라이언트 연결 강제 끊기
	case WSAECONNABORTED:	//통신 포기
		Disconnect(L"HandleError : Connection Reset / Aborted");
		break;

	default:
		wprintf(L"HandleError : %d\n", errorCode);
		break;
	}
}