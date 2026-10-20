#pragma once
#include "Service.h"
class ClientService : public Service
{
public:
	ClientService(NetAddress netAddress, std::shared_ptr<IocpCore> core, SessionFactory factory, int32 maxSeesioncont = 1);

	virtual ~ClientService();

	virtual bool Start() override;
};

