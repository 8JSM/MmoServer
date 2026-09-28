#pragma once
class IocpObject;
class IocpCore
{
public:
	IocpCore();
	~IocpCore();

	HANDLE GetHandle() const {return _iocpHandle;}

	bool Register(const std::shared_ptr<IocpObject>& iocpObject);
	bool Dispatch(uint32 timeoutMs = INFINITE);

private:
	HANDLE _iocpHandle = INVALID_HANDLE_VALUE;
};


