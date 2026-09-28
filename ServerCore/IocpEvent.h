#pragma once
class IocpObject;

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