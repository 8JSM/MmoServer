#include "pch.h"
#include "ServerService.h"
#include "Listener.h"
ServerService::ServerService(NetAddress netAddress, std::shared_ptr<IocpCore> core, SessionFactory factory, int32 maxSessionCount)
	: Service(ServiceType::Server, netAddress, core, factory, maxSessionCount)
{

}

ServerService::~ServerService()
{
	CloseService();
}

void ServerService::CloseService()
{
	if (_listener)
	{
		_listener->CloseSocket();
		_listener = nullptr;
	}
	Service::CloseService();
}

bool ServerService::Start()
{
	if (CanStart() == false)
		return false;
	_listener = std::make_shared<Listener>();

	if (_listener == nullptr)
		return false;

	if (_listener->StartAccept(shared_from_this()) == false)
		return false;

	return true;

}