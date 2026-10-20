#include "pch.h"
#include "Service.h"
#include "Session.h"

Service::Service(ServiceType type, NetAddress netAddress, std::shared_ptr<IocpCore> core, SessionFactory factory, int32 maxSessionCount)
	: _type(type), _netAddress(netAddress), _iocpCore(core), _sessionFactory(factory), _maxSessionCount(maxSessionCount)
{

}

Service::~Service()
{

}

void Service::CloseService()
{

}

std::shared_ptr<Session> Service::CreateSession()
{
	if (_sessionFactory == nullptr)
		return nullptr;

	std::shared_ptr<Session> session = _sessionFactory();

	session->_service = shared_from_this();

	if (_iocpCore->Register(session) == false)
		return nullptr;

	return session;
}

void Service::AddSession(std::shared_ptr<Session> session)
{
	WRITE_LOCK;

	_sessionCount++;
	_sessions.insert(session);

}

void Service::ReleaseSession(std::shared_ptr<Session> session)
{
	WRITE_LOCK;

	ASSERT_CRASH(_sessions.erase(session) != 0);
	_sessionCount--;
}

int32 Service::GetCurrentSessionCount()
{
	WRITE_LOCK;
	return _sessionCount;
}

void Service::Broadcast(std::shared_ptr<SendBuffer> sendBuffer)
{
	WRITE_LOCK;

	for (const auto& session : _sessions)
	{
		session->Send(sendBuffer);
	}

}