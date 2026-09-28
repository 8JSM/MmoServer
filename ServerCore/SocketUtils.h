#pragma once
#include "NetAddress.h"
class SocketUtils
{
public:
	static LPFN_CONNECTEX ConnectEx;
	static LPFN_DISCONNECTEX DisconnectEx;
	static LPFN_ACCEPTEX AcceptEx;

public:
	static void Init();
	static void Clear();

	static SOCKET CreateSocket();
	static bool BindWindowsFunction(SOCKET socket, GUID guid, LPVOID* fn);

	// 家南 可记 汲沥 秋欺
	static bool SetLinger(SOCKET socket, uint16 onoff, uint16 linger);
	static bool SetTcpNoDelay(SOCKET socket, bool flag);
	static bool SetReuseAddress(SOCKET socket, bool flag);
	static bool SetRecvBufferSize(SOCKET socket, int32 size);
	static bool SetSendBufferSize(SOCKET socket, int32 size);
	static bool SetUpdateAcceptSocket(SOCKET socket, SOCKET listenSocket);

	// 家南 力绢 贰欺
	static bool Bind(SOCKET socket, const NetAddress& netAddr);
	static bool BindAnyAddress(SOCKET socket, uint16 port);
	static bool Listen(SOCKET socket, int32 backlog = SOMAXCONN);
	static void Close(SOCKET& socket);
};

