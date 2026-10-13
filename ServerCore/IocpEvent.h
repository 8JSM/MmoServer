#pragma once

class IocpObject;
class SendBuffer;

enum class EventType : uint8
{
	Connect,
	Disconnect,
	Accept,
	Recv,
	Send
};
struct IocpEvent
{	// 가상 함수 추가 금지
	WSAOVERLAPPED overlapped = {};
	EventType eventType; // 어떤 종류의 작업인지 구분하기 위해 필요
	std::shared_ptr<IocpObject> owner = nullptr;

	IocpEvent(EventType type) : eventType(type)
	{
		Init();
	}

	virtual void Init()
	{
		::memset(&overlapped, 0, sizeof(overlapped));
		owner = nullptr;
	}
};

struct AcceptEvent : public IocpEvent
{
	AcceptEvent() : IocpEvent(EventType::Accept) {}
};

struct SendEvent : public IocpEvent
{
	SendEvent() : IocpEvent(EventType::Send) {}

	std::vector<std::shared_ptr<SendBuffer>> sendBuffers;

	virtual void Init() override
	{
		IocpEvent::Init();
		sendBuffers.clear();
	}
};

struct RecvEvent : public IocpEvent
{
	RecvEvent() : IocpEvent(EventType::Recv) {}
};

struct ConnectEvent : public IocpEvent
{
	ConnectEvent() : IocpEvent(EventType::Connect) {}
};

struct DisconnectEvent : public IocpEvent
{
	DisconnectEvent() : IocpEvent(EventType::Disconnect) {}
};