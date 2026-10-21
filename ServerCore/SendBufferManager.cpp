#include "pch.h"
#include "SendBufferManager.h"
#include "SendBufferChunk.h"

thread_local SendBufferChunkRef LSendBufferChunk = nullptr;
std::mutex SendBufferManager::_lock;
std::vector<SendBufferChunkRef> SendBufferManager::_sendBufferChunks;

SendBufferManager::SendBufferManager()
{

}
SendBufferManager::~SendBufferManager()
{

}
SendBufferRef SendBufferManager::Open(int32 size)
{
	if (LSendBufferChunk == nullptr || LSendBufferChunk->FreeSize() < size)
	{
		LSendBufferChunk = Pop();
		LSendBufferChunk->Reset();
	}
	return LSendBufferChunk->Open(size);
}

SendBufferChunkRef SendBufferManager::Pop()
{
	WRITE_LOCK;

	if (_sendBufferChunks.empty() == false)
	{
		SendBufferChunkRef chunk = _sendBufferChunks.back();
		_sendBufferChunks.pop_back();
		return chunk;
	}
	return SendBufferChunkRef(new SendBufferChunk(), PushGlobal);
}

void SendBufferManager::Push(SendBufferChunkRef chunk)
{
	WRITE_LOCK;
	_sendBufferChunks.push_back(chunk);
}
void SendBufferManager::PushGlobal(SendBufferChunk* chunk)
{
	chunk->Reset();
	SendBufferChunkRef wrappedChunk = SendBufferChunkRef(chunk, PushGlobal);
	Push(wrappedChunk);
}
