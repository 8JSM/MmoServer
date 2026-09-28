#include "pch.h"
#include "SendBufferChunk.h"
#include "SendBuffer.h"

SendBufferChunk::SendBufferChunk()
{
	_buffer.resize(CHUNK_SIZE);
}
SendBufferChunk::~SendBufferChunk()
{

}


SendBufferRef SendBufferChunk::Open(int32 allocSize)
{
	ASSERT(allocSize <= FreeSize());
	ASSERT(_isOpen == false);

	_isOpen = true;

	return std::make_shared<SendBuffer>(shared_from_this(), Buffer(), allocSize);
}

void SendBufferChunk::Close(int32 writeSize)
{
	ASSERT(_isOpen == true);

	_isOpen = false;

	_usedSize += writeSize;
}

void SendBufferChunk::Reset()
{
	_isOpen = false;
	_usedSize = 0;
}
