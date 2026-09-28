#pragma once
class IocpObject : public std::enable_shared_from_this<IocpObject>
{
public:
	virtual ~IocpObject() = default;
	virtual HANDLE GetHandle() abstract = 0;
	virtual void Dispatch(struct IocpEvent* IocpEvent, int32 numOfBytes = 0) abstract = 0;
};

