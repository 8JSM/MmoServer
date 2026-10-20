#pragma once
#include "NetAddress.h"
#include "IocpCore.h"

class Session;
class SendBuffer;

using SessionFactory = std::function<std::shared_ptr<Session>()>; // 의존성 역전 현상 방지

enum class ServiceType : uint8
{
	Server,
	Client
};
class Service : public std::enable_shared_from_this<Service>
{
public:

	Service(
		ServiceType type, 
		NetAddress netAddress, 
		std::shared_ptr<IocpCore> core, 
		SessionFactory factory, 
		int32 maxSessionCount = 1
	);

	virtual ~Service();

	virtual bool Start() abstract = 0;
	virtual void CloseService();
	

public:
	std::shared_ptr<Session> CreateSession();
	void AddSession(std::shared_ptr<Session> session);
	void ReleaseSession(std::shared_ptr<Session> session);
	int32 GetCurrentSessionCount();
	int32 GetMaxSessionCount() const { return _maxSessionCount; }

	void Broadcast(std::shared_ptr<SendBuffer> sendBuffer);

public:
	ServiceType GetServiceType() const { return _type; }
	NetAddress GetNetAddress() const { return _netAddress; }
	std::shared_ptr<IocpCore> GetIocpCore() const { return _iocpCore; }

	bool CanStart() const { return _sessionFactory != nullptr; }

protected:
	USE_LOCK;

	ServiceType _type;
	NetAddress _netAddress = {};
	std::shared_ptr<IocpCore> _iocpCore;

	std::set<std::shared_ptr<Session>> _sessions;
	
	int32 _sessionCount = 0;
	int32 _maxSessionCount = 0;

	SessionFactory _sessionFactory;

};

