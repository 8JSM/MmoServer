#pragma once
#include "IocpObject.h"
#include "NetAddress.h"
#include "IocpEvent.h"
#include "RecvBuffer.h"
#include "SendBuffer.h"

class Session : public IocpObject
{
	friend class IocpCore;
	friend class Service;

	enum
	{
		BUFFER_SIZE = 0x10000, // 64KB
	};

public:
	Session();
	virtual ~Session();

	virtual HANDLE GetHandle() override;
	virtual void Dispatch(struct IocpEvent* iocpEvent, int32 numOfBytes = 0) override;

public:
	// 외부 인터페이스
	void Send(std::shared_ptr<SendBuffer> sendBuffer);
	void Disconnect(const WCHAR* cause);

	bool IsConnected() const { return _connected; }
	SOCKET GetSocket() const { return _socket; }
	NetAddress GetNetAddress() const { return _netAddress; }
	void SetNetAddress(NetAddress netAddress) { _netAddress = netAddress; }

protected:
	// 콘텐츠 계층 오버라이딩 매서드
	virtual void OnConnected() {}
	virtual void OnDisconnected() {}
	virtual int32 OnRecv(BYTE* buffer, int32 len) { return len; }
	virtual void OnSend(int32 len) {}

private:
	// 네트워크 계층 I/O 처리 매서드

	void ProcessConnect();
	void ProcessRecv(int32 numOfBytes);
	void RegisterRecv();

	void ProcessSend(int32 numOfBytes);
	void RegisterSend();

	void ProcessDisconnect();
	void RegisterDisconnect();

	void HandleError(int32 errorCode);

private:
	std::weak_ptr<Service> _service; // Service가 Session을 shared_ptr로 소유, 순환참조 방지를 위해 Session은 weak_ptr을 사용
	SOCKET _socket = INVALID_SOCKET;
	NetAddress _netAddress = {};
	std::atomic<bool> _connected = false;

private:
	USE_LOCK;

	RecvBuffer _recvBuffer;
	std::queue<std::shared_ptr<SendBuffer>> _sendQueue;
	std::atomic<bool> _sendRegistered = false;

private:
	RecvEvent _recvEvent;
	SendEvent _sendEvent;
	ConnectEvent _connectEvent;
	DisconnectEvent _disconnectEvent;
};

