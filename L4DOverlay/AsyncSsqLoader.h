#pragma once

#include <WinSock2.h>
#include <string>
#include <memory>
#include <future>

#include "S2CInfoPacket.h"
#include "S2CPlayerPacket.h"

struct AsyncSsqLoader final {
    AsyncSsqLoader(std::wstring serverIp, std::wstring serverPort);
    ~AsyncSsqLoader();

    void Start();
    void Stop();
    void Update(double elapsedTime);

    bool GetIsDataValid() const;
    bool GetIsConnecting() const;
    bool GetIsConnectionError() const;

private:
    std::wstring m_serverIp;
    std::wstring m_serverPort;
    std::unique_ptr<S2CInfoPacket> m_serverInfo;
    std::unique_ptr<S2CPlayerPacket> m_playerInfo;
    std::future<void> m_connectionFuture;

    std::atomic<bool> m_isRunning;
    std::atomic<bool> m_isDataValid;
    std::atomic<bool> m_isConnecting;
    std::atomic<bool> m_isConnectionError;
    // TODO data present

    double m_connectionTimer = 0.0;
    double m_updateTimer = 0.0;
    DWORD m_cancelTimeout = 10;
    DWORD m_updateTimeout = 5000;
    DWORD m_connectionTimeout = 5000;

    static constexpr int kReceiveBufferSize = 4096;

    // Network
    void FetchData();
    // Helpers
    bool ResolveServerAddress(const wchar_t* ip, const wchar_t* port, ADDRINFOEXW* serverAddress) const;
    int ReceiveSomeFrom(SOCKET socket, char* buffer, int length, const sockaddr& receiveAddress, int addressLength) const;
    bool SendAllTo(SOCKET socket, char* buffer, int length, const sockaddr& sendAddress, int addressLength) const;
    bool SendPacket(SOCKET socket, const Packet<SsqPacketType>& packet, const sockaddr& sendAddress, int addressLength) const;
    std::unique_ptr<Packet<SsqPacketType>> ReceivePacket(SOCKET socket, const sockaddr& receiveAddress, int addressLength) const;
};
