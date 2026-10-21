#pragma once

class IocpObject;
class SendBuffer;
class Session;

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

	void Init()
	{
		::memset(&overlapped, 0, sizeof(overlapped));
		owner = nullptr;
	}
};

struct AcceptEvent : public IocpEvent
{
	AcceptEvent() : IocpEvent(EventType::Accept) {}

	std::shared_ptr<Session> session = nullptr;

	void Init()
	{
		IocpEvent::Init();
		session = nullptr;
	}
};

struct SendEvent : public IocpEvent
{
	SendEvent() : IocpEvent(EventType::Send) {}

	std::vector<std::shared_ptr<SendBuffer>> sendBuffers;

	void Init()
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