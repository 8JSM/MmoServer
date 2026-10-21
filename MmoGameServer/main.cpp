#include "pch.h"
#include "GameSession.h"
int main()
{
	SocketUtils::Init();

	std::shared_ptr<IocpCore> iocpCore = std::make_shared<IocpCore>();

	NetAddress netAddress(L"127.0.0.1", 7777);
	
	std::shared_ptr<Service> service = std::make_shared<ServerService>(netAddress, iocpCore, std::make_shared<GameSession>, 100);

	ASSERT_CRASH(service->Start());

	std::cout << "Echo Server Running on Port 7777..." << std::endl;

	std::vector<std::thread> threads;

	//const int32 threadCount = std::thread::hardware_concurrency();
	const int32 threadCount = 1;

	for (int32 i = 0; i < threadCount; i++)
	{
		threads.emplace_back([iocpCore]()
			{
				while (true)
				{
					iocpCore->Dispatch();
				}
			});
	}

	for (auto& t : threads)
		t.join();

	SocketUtils::Clear();
	return 0;
}