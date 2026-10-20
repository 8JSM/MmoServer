#include "pch.h"
#include "ClientService.h"
#include "Session.h"

ClientService::ClientService(NetAddress netAddress, std::shared_ptr<IocpCore> core, SessionFactory factory, int32 maxSessionCount)
	: Service(ServiceType::Client, netAddress, core, factory, maxSessionCount)
{
}

ClientService::~ClientService()
{

}

bool ClientService::Start()
{
	if (CanStart() == false)
		return false;

	const int32 sessionCount = GetMaxSessionCount();

	for (int32 i = 0; i < sessionCount; i++)
	{
		std::shared_ptr<Session> session = CreateSession();
		if (session == nullptr)
			return false;

		if (session->Connect() == false)
			return false;
	}
}