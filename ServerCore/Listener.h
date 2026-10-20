#pragma once
#include "IocpObject.h"
#include "IocpEvent.h"
class Service;

class Listener : public IocpObject
{
public:
	Listener();
	virtual ~Listener();

	virtual HANDLE GetHandle() override;
	virtual void Dispatch(struct IocpEvent* IocpEvent, int32 numOfBytes = 0) override;

public:
	// 외부 인터페이스

	bool StartAccept(std::shared_ptr<Service> service);
	void CloseSocket();

private:
	// 네트워크 계층
	void RegisterAccept(AcceptEvent* acceptEvent);
	void ProcessAccept(AcceptEvent* acceptEvent);

private:
	SOCKET _socket = INVALID_SOCKET;
	std::vector<AcceptEvent*> _acceptEvents;
	std::weak_ptr<Service> _service;
};

