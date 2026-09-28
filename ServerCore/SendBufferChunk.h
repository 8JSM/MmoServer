#pragma once
class SendBuffer;
using SendBufferRef = std::shared_ptr<SendBuffer>;
class SendBufferChunk : public std::enable_shared_from_this<SendBufferChunk>
{
	static constexpr int32 CHUNK_SIZE = 0x10000; // 64KB

public:
	SendBufferChunk();
	~SendBufferChunk();

	SendBufferRef Open(int32 allocSize);
	void Close(int32 writeSize);
	void Reset();

	BYTE* Buffer() { return &_buffer[_usedSize]; }
	bool IsOpen() const { return _isOpen; }
	int32 FreeSize() const { return static_cast<int32>(_buffer.size()) - _usedSize; }
	

private:
	std::vector<BYTE> _buffer;
	bool _isOpen = false;
	int _usedSize = 0;
};

