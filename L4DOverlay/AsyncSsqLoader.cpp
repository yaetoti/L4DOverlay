#include "AsyncSsqLoader.h"

#include <WS2tcpip.h>
#include "CommonLib/ScopedHandle.h"
#include "A2SInfoPacket.h"
#include "A2SPlayerPacket.h"
#include "S2CChallengePacket.h"

AsyncSsqLoader::AsyncSsqLoader(std::wstring serverIp, std::wstring serverPort)
: m_serverIp(std::move(serverIp)), m_serverPort(std::move(serverPort)) {
}

AsyncSsqLoader::~AsyncSsqLoader() {
    Stop();
}

void AsyncSsqLoader::Start() {
    if (m_isRunning) {
        return;
    }

    m_isConnectionError = false;
    m_isDataValid = false;
    m_connectionTimer = 0.0;
    m_updateTimer = 0.0;
    m_isRunning = true;
}

void AsyncSsqLoader::Stop() {
    if (!m_isRunning) {
        return;
    }

    m_isRunning = false;
    m_connectionFuture.wait();
}

void AsyncSsqLoader::Update(double elapsedTime) {
    if (m_isRunning) {
        // Update data validity
        if (m_isDataValid) {
            m_updateTimer += elapsedTime;
            if (m_updateTimer >= m_updateTimeout) {
                m_isDataValid = false;
            }
        }

        // Update data
        if (!m_isDataValid && !m_isConnecting) {
            m_isConnecting = true;
            FetchData();
        }

        if (m_isConnecting) {
            m_connectionTimer += elapsedTime;
            if (m_connectionTimer >= m_connectionTimeout) {
                Stop();
            }
        }
    }
}

bool AsyncSsqLoader::GetIsDataValid() const {
    return m_isDataValid;
}

bool AsyncSsqLoader::GetIsConnecting() const {
    return m_isConnecting;
}

bool AsyncSsqLoader::GetIsConnectionError() const {
    return m_isConnectionError;
}

bool AsyncSsqLoader::ResolveServerAddress(const wchar_t* ip, const wchar_t* port, ADDRINFOEXW* serverAddress) const {
    ADDRINFOEXW* info;
    OVERLAPPED overlapped = { };
    ScopedHandle<HANDLE> event(CreateEventW(nullptr, TRUE, FALSE, nullptr), CloseHandle);
    overlapped.hEvent = event.Get();
    if (nullptr == event.Get()) {
        return false;
    }

    HANDLE cancelHandle;
    if (0 != GetAddrInfoExW(ip, port, NS_DNS, nullptr, nullptr, &info, nullptr, &overlapped, nullptr, &cancelHandle)) {
        if (WSA_IO_PENDING != WSAGetLastError()) {
            // Function failed to get ip
            return false;
        }

        while (true) {
            const DWORD status = WaitForSingleObject(overlapped.hEvent, m_cancelTimeout);
            if (WAIT_FAILED == status) {
                // Function failed to get ip
                return false;
            }
            if (WAIT_TIMEOUT == status && !m_isRunning) {
                // If function isn't completed but the request is cancelled
                GetAddrInfoExCancel(&cancelHandle);
                return false;
            }
            if (WAIT_OBJECT_0 == status) {
                // Function completed
                break;
            }
        }
    }

    // Take the first available ip
    *serverAddress = *info;
    FreeAddrInfoExW(info);
    return true;
}

int AsyncSsqLoader::ReceiveSomeFrom(SOCKET socket, char* buffer, int length, const sockaddr& receiveAddress, int addressLength) const {
    WSABUF descriptor { static_cast<ULONG>(length), buffer };
    OVERLAPPED overlapped = { };
    ScopedHandle<HANDLE> event(CreateEventW(nullptr, TRUE, FALSE, nullptr), CloseHandle);
    overlapped.hEvent = event.Get();
    if (nullptr == event.Get()) {
        return SOCKET_ERROR;
    }

    sockaddr_in address = { };
    int receiveAddressLength = sizeof(address);

    DWORD received = 0;
    DWORD flags = 0;
    int status = WSARecvFrom(socket, &descriptor, 1, &received, &flags, reinterpret_cast<sockaddr*>(&address), &receiveAddressLength, &overlapped, nullptr);
    if (0 != status) {
        if (WSA_IO_PENDING != WSAGetLastError()) {
            // Function failed to receive data
            return SOCKET_ERROR;
        }

        while (true) {
            const DWORD waitStatus = WaitForSingleObject(overlapped.hEvent, m_cancelTimeout);
            if (WAIT_FAILED == waitStatus) {
                // Function failed to receive data
                return SOCKET_ERROR;
            }
            if (WAIT_TIMEOUT == waitStatus && !m_isRunning) {
                // If function isn't completed but the request is cancelled
                return SOCKET_ERROR;
            }
            if (WAIT_OBJECT_0 == waitStatus) {
                // Function completed
                if (!WSAGetOverlappedResult(socket, &overlapped, &received, FALSE, &flags)) {
                    return SOCKET_ERROR;
                }

                status = static_cast<int>(received);
                break;
            }
        }
    }

    if (receiveAddressLength != addressLength || 0 != std::memcmp(&address, &receiveAddress, receiveAddressLength)) {
        // Wrong address
        status = -2;
    }

    return status;
}

bool AsyncSsqLoader::SendAllTo(SOCKET socket, char* buffer, int length, const sockaddr& sendAddress, int addressLength) const {
    WSABUF descriptor { static_cast<ULONG>(length), buffer };
    OVERLAPPED overlapped = { };
    ScopedHandle<HANDLE> event(CreateEventW(nullptr, TRUE, FALSE, nullptr), CloseHandle);
    overlapped.hEvent = event.Get();
    if (nullptr == event.Get()) {
        return false;
    }

    DWORD sent = 0;
    int status = WSASendTo(socket, &descriptor, 1, &sent, 0, &sendAddress, addressLength, &overlapped, nullptr);
    if (0 != status) {
        if (WSA_IO_PENDING != WSAGetLastError()) {
            // Function failed to send data
            return false;
        }

        while (true) {
            const DWORD waitStatus = WaitForSingleObject(overlapped.hEvent, m_cancelTimeout);
            if (WAIT_FAILED == waitStatus) {
                // Function failed to send data
                return false;
            }
            if (WAIT_TIMEOUT == waitStatus && !m_isRunning) {
                // If function isn't completed but the request is cancelled
                return false;
            }
            if (WAIT_OBJECT_0 == waitStatus) {
                // Function completed
                DWORD flags;
                if (!WSAGetOverlappedResult(socket, &overlapped, &sent, FALSE, &flags)) {
                    return false;
                }

                break;
            }
        }
    }

    return true;
}

bool AsyncSsqLoader::SendPacket(SOCKET socket, const Packet<SsqPacketType>& packet, const sockaddr& sendAddress, int addressLength) const {
    std::vector<uint8_t> buffer;
    ByteBuffer wrapper(buffer);

    wrapper.Put(HeaderType::HEADER_SIMPLE);
    wrapper.Put(packet.GetType());
    packet.Serialize(wrapper);

    return SendAllTo(socket, std::bit_cast<char*>(buffer.data()), static_cast<int>(buffer.size()), sendAddress, addressLength);
}

std::unique_ptr<Packet<SsqPacketType>> AsyncSsqLoader::ReceivePacket(SOCKET socket, const sockaddr& receiveAddress, int addressLength) const {
    std::vector<uint8_t> data;
    HeaderType headerType;
    SsqPacketType packetType;

    while (true) {
        try {
            // Receive some data
            char buffer[kReceiveBufferSize];
            const int status = ReceiveSomeFrom(socket, buffer, kReceiveBufferSize, receiveAddress, addressLength);
            if (SOCKET_ERROR == status) {
                return nullptr;
            }
            if (-2 == status) {
                // Skip data from unexpected address
                continue;
            }

            data.insert(data.end(), buffer, buffer + status);

            // Parse header
            ByteBuffer wrapper(data);
            wrapper.Get(headerType);
            if (HeaderType::HEADER_SIMPLE != headerType) {
                // Unsupported header
                return nullptr;
            }

            wrapper.Get(packetType);

            // Parse packet
            switch (packetType) {
            case SsqPacketType::S2C_CHALLENGE:
            {
                S2CChallengePacket packet;
                packet.Deserialize(wrapper);
                return std::make_unique<S2CChallengePacket>(packet);
            }
            case SsqPacketType::S2C_PLAYER:
            {
                S2CPlayerPacket packet;
                packet.Deserialize(wrapper);
                return std::make_unique<S2CPlayerPacket>(packet);
            }
            case SsqPacketType::S2C_INFO:
            {
                S2CInfoPacket packet;
                packet.Deserialize(wrapper);
                return std::make_unique<S2CInfoPacket>(packet);
            }
            default:
                // Unexpected answer
                return nullptr;
            }
        }
        catch (const std::out_of_range&) {
            // If not enough to parse - repeat
            continue;
        }
    }
}

void AsyncSsqLoader::FetchData() {
    m_connectionFuture = std::async(std::launch::async, [this] {
        ScopedHandle<SOCKET> serverSocket(
            INVALID_SOCKET,
            WSASocketW(AF_INET, SOCK_DGRAM, IPPROTO_UDP, nullptr, 0, WSA_FLAG_OVERLAPPED),
            closesocket
        );
        if (INVALID_SOCKET == serverSocket.Get()) {
            m_isConnectionError = true;
            m_isConnecting = false;
            return;
        }

        // Resolve server address
        ADDRINFOEXW serverAddress = { };
        if (!ResolveServerAddress(m_serverIp.c_str(), m_serverPort.c_str(), &serverAddress)) {
            m_isConnectionError = true;
            m_isConnecting = false;
            return;
        }

        // Variables
        std::unique_ptr<Packet<SsqPacketType>> infoPacket;
        std::unique_ptr<Packet<SsqPacketType>> playersPacket;
        int challenge = -1;

        // Retrieving server info
        while (true) {
            if (!SendPacket(serverSocket.Get(), A2SInfoPacket(challenge), *serverAddress.ai_addr, static_cast<int>(serverAddress.ai_addrlen))) {
                m_isConnectionError = true;
                m_isConnecting = false;
                return;
            }

            infoPacket = ReceivePacket(serverSocket.Get(), *serverAddress.ai_addr, static_cast<int>(serverAddress.ai_addrlen));
            if (!infoPacket) {
                m_isConnectionError = true;
                m_isConnecting = false;
                return;
            }

            if (SsqPacketType::S2C_CHALLENGE == infoPacket->GetType()) {
                // Repeat with received challenge number
                challenge = reinterpret_cast<S2CChallengePacket*>(infoPacket.get())->GetChallenge();
                continue;
            }
            if (SsqPacketType::S2C_INFO != infoPacket->GetType()) {
                // Unexpected answer
                m_isConnectionError = true;
                m_isConnecting = false;
                return;
            }

            break;
        }
        // Retrieving players info
        while (true) {
            if (!SendPacket(serverSocket.Get(), A2SPlayerPacket(challenge), *serverAddress.ai_addr, static_cast<int>(serverAddress.ai_addrlen))) {
                m_isConnectionError = true;
                m_isConnecting = false;
                return;
            }

            playersPacket = ReceivePacket(serverSocket.Get(), *serverAddress.ai_addr, static_cast<int>(serverAddress.ai_addrlen));
            if (!playersPacket) {
                m_isConnectionError = true;
                m_isConnecting = false;
                return;
            }

            if (SsqPacketType::S2C_CHALLENGE == playersPacket->GetType()) {
                // Repeat with received challenge number
                challenge = reinterpret_cast<S2CChallengePacket*>(playersPacket.get())->GetChallenge();
                continue;
            }
            if (SsqPacketType::S2C_PLAYER != playersPacket->GetType()) {
                // Unexpected answer
                m_isConnectionError = true;
                m_isConnecting = false;
                return;
            }

            break;
        }

        // Packets received
        // TODO Ugly. Implement move semantic
        m_serverInfo = std::make_unique<S2CInfoPacket>(reinterpret_cast<S2CInfoPacket&>(*infoPacket));
        m_playerInfo = std::make_unique<S2CPlayerPacket>(reinterpret_cast<S2CPlayerPacket&>(*playersPacket));
        m_connectionTimer = 0.0;
        m_updateTimer = 0.0;
        m_isDataValid = true;
        m_isConnecting = false;
    });
}
