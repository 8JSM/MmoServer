#include "pch.h"
#include "GameSession.h"

void GameSession::OnConnected()
{
	std::cout << "[Server] Client Connected: " <<  GetSessionRef().get() << std::endl;
}

void GameSession::OnDisconnected()
{
	std::cout << "[Server] Client Disconnected: " << GetSessionRef().get() << std::endl;
}

int32 GameSession::OnRecv(BYTE* buffer, int32 len)
{
	SendBufferRef sendBuffer = SendBufferManager::Open(len);
	::memcpy(sendBuffer->Buffer(), buffer, len);
	sendBuffer->Close(len);

	Send(sendBuffer);

	return len;
}

void GameSession::OnSend(int32 len)
{

}