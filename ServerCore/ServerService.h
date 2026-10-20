#pragma once
#include "Service.h"

class Listener;
class ServerService : public Service
{
public:

	ServerService(NetAddress netAddress, std::shared_ptr<IocpCore> core, SessionFactory factory, int32 maxSessionCount = 1);

	virtual ~ServerService();

	virtual bool Start() override;
	virtual void CloseService() override;

private:
	std::shared_ptr<Listener> _listener;
};

