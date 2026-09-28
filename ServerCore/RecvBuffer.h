#pragma once
class RecvBuffer
{
	
	static constexpr int32 BUFFER_COUNT = 10;

public:
	RecvBuffer(int32 bufferSize);
	~RecvBuffer();


	void Clean();
	bool OnRead(int32 numOfBytes);
	bool OnWrite(int32 numOfBytes);

	BYTE* ReadPos() { return &_buffer[_readPos]; } 
	BYTE* WritePos() { return &_buffer[_writePos]; }
	int32 DataSize() { return _writePos - _readPos; }
	int32 FreeSize() { return _capacity - _writePos; }


private:

	std::vector<BYTE> _buffer;
	int32 _capacity = 0;
	int32 _readPos = 0; // 데이터 읽고 나서 위치 인덱스
	int32 _writePos = 0; // 데이터 채울 위치 인덱스
	int32 _bufferSize = 0;

};

