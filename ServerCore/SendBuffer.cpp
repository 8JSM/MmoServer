#include "pch.h"
#include "SendBuffer.h"
#include "SendBufferChunk.h"

SendBuffer::SendBuffer(SendBufferChunkRef owner, BYTE* buffer, int32 allocSize)
	: _owner(owner), _buffer(buffer), _allocSize(allocSize)
{

}
SendBuffer::~SendBuffer()
{

}

void SendBuffer::Close(int32 writeSize)
{
	ASSERT(_allocSize >= writeSize);
	
	_writeSize = writeSize;

	_owner->Close(writeSize);
}
