#pragma once

class SendBufferChunk;
using SendBufferChunkRef = std::shared_ptr<SendBufferChunk>;

class SendBuffer 
{
public:
	SendBuffer(SendBufferChunkRef owner, BYTE* buffer, int32 allocSize);
	~SendBuffer();

	BYTE* Buffer() { return _buffer; }
	int32 AllocSize() { return _allocSize; }
	int32 WriteSize() { return _writeSize; }

	void Close(int32 WriteSize);

private:
	BYTE* _buffer = nullptr;
	int32 _writeSize = 0; // 실제 메모리 크기
	int32 _allocSize = 0; // 버퍼 크기
	SendBufferChunkRef _owner;
};

