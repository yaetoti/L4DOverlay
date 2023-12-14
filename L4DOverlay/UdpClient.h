#pragma once

#include <WinSock2.h>
#include <string>
#include <vector>

class UdpClient final {
public:
    UdpClient(const std::string& serverIp, unsigned short serverPort);
    ~UdpClient() noexcept;

    int Send(const char* data, int dataSize) const;
    std::vector<uint8_t> Receive(size_t bufferSize = 65507) const;

    UdpClient(const UdpClient&) = delete;
    const UdpClient& operator=(const UdpClient&) = delete;
private:
    SOCKET m_udpSocket;
    sockaddr_in m_serverAddr;
};