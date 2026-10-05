#pragma once
#include "IocpObject.h"
#include "NetAddress.h"

class SendBuffer;

class Session : public IocpObject
{
	friend class IocpCore;

public:
	Session();
	virtual ~Session();

	virtual HANDLE GetHandle() override;
	virtual void Dispatch(struct IocpEvent* iocpEvent, int32 numOfBytes = 0) override;

public:
	void Send(std::shared_ptr<SendBuffer> sendBuffer);
	void Disconnect(const WCHAR* cause);

	bool IsConnected() const { return _connected; }
	SOCKET GetSocket() const { return _socket; }
	NetAddress GetNetAddress() const { return _netAddress; }
	void SetNetAddress(NetAddress netAddress) { _netAddress = netAddress; }


private:
	SOCKET _socket = INVALID_SOCKET;
	NetAddress _netAddress = {};
	std::atomic<bool> _connected = false;

};

