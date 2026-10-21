#include "pch.h"
#include "IocpCore.h"
#include "IocpObject.h"
#include "IocpEvent.h"

IocpCore::IocpCore()
{
	_iocpHandle = ::CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);
	ASSERT_CRASH(_iocpHandle != NULL);
}
IocpCore::~IocpCore()
{
	if (_iocpHandle != INVALID_HANDLE_VALUE)
	{
		::CloseHandle(_iocpHandle);
		_iocpHandle = INVALID_HANDLE_VALUE;
	}
}

bool IocpCore::Register(const std::shared_ptr<IocpObject>& iocpObject)
{
	HANDLE result = ::CreateIoCompletionPort(iocpObject->GetHandle(), _iocpHandle, 0, 0);

	return (result != NULL);
}

bool IocpCore::Dispatch(uint32 timeoutMs)
{
	DWORD numOfBytes = 0;
	ULONG_PTR key = 0;
	IocpEvent* iocpEvent = nullptr;

	BOOL ret = GetQueuedCompletionStatus(
		_iocpHandle,
		&numOfBytes,
		&key,
		reinterpret_cast<LPOVERLAPPED*>(&iocpEvent),
		timeoutMs
	);

	if (ret == FALSE)
	{
		int32 errorCode = ::WSAGetLastError();
		if (iocpEvent == nullptr)
		{
			return false;
		}
	}

	std::shared_ptr<IocpObject> owner = iocpEvent->owner;
	iocpEvent->owner = nullptr;

	if (owner != nullptr)
	{
		owner->Dispatch(iocpEvent, numOfBytes);
		
	}
	return true;
}