#pragma once
class SendBuffer;
class SendBufferChunk;
using SendBufferRef = std::shared_ptr<SendBuffer>;
using SendBufferChunkRef = std::shared_ptr<SendBufferChunk>;

class SendBufferManager
{
public:
	SendBufferManager();
	~SendBufferManager();
	static SendBufferRef Open(int32 size);

private:
	static SendBufferChunkRef Pop();
	static void Push(SendBufferChunkRef chunk);
	static void PushGlobal(SendBufferChunk* chunk);

private:
	USE_STATIC_LOCK;
	static std::vector<SendBufferChunkRef> _sendBufferChunks;
};

