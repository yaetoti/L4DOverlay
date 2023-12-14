#include "UdpClient.h"

#include <stdexcept>
#include <WS2tcpip.h>

UdpClient::UdpClient(const std::string& serverIp, unsigned short serverPort) {
    m_udpSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (m_udpSocket == INVALID_SOCKET) {
        throw std::runtime_error("Failed to create socket");
    }

    m_serverAddr.sin_family = AF_INET;
    m_serverAddr.sin_port = htons(serverPort);
    if (inet_pton(AF_INET, serverIp.c_str(), &m_serverAddr.sin_addr) != 1) {
        closesocket(m_udpSocket);
        throw std::runtime_error("Invalid server IP address");
    }
}

UdpClient::~UdpClient() {
    closesocket(m_udpSocket);
}

int UdpClient::Send(const char* data, int dataSize) const {
    int sent = sendto(m_udpSocket, data, dataSize, 0, reinterpret_cast<const sockaddr*>(&m_serverAddr), sizeof(m_serverAddr));
    if (sent == SOCKET_ERROR) {
        throw std::runtime_error("Failed to send data to the server");
    }

    return sent;
}

std::vector<uint8_t> UdpClient::Receive(size_t bufferSize) const {
    std::vector<uint8_t> receiveBuffer(bufferSize);
    int recvResult = recvfrom(m_udpSocket, (char*)receiveBuffer.data(), (int)bufferSize, 0, nullptr, nullptr);
    if (recvResult == SOCKET_ERROR) {
        throw std::runtime_error("Failed to receive data from the server");
    }

    receiveBuffer.resize(recvResult);
    return receiveBuffer;
}
