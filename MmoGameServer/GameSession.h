#pragma once
class GameSession :
    public Session
{
public:
    GameSession() = default;
    virtual ~GameSession() = default;

protected:

    virtual void OnConnected() override;
    virtual void OnDisconnected() override;
    virtual int32 OnRecv(BYTE* buffer, int32 len) override;
    virtual void OnSend(int32 len) override;
};

