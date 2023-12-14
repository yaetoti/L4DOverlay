#define WIN32_LEAN_AND_MEAN

#include <ConsoleLib/Console.h>
#include <NetworkLib/WsaDataWrapper.h>
#include <CommonLib/ByteBuffer.h>
#include <CommonLib/VecUtils.h>
#include <Windows.h>

#include "UdpClient.h"
#include "Protocol.h"
#include "Packets.h"

// https://www.gs4u.net/en/s/236165.html BCM COOP Ukrainian 31.43.157.18:1302 185.135.80.185:2356
int WINAPI wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ PWSTR pCmdLine, _In_ int nCmdShow) {
    WsaDataWrapper wsa(2, 2);
    // UdpClient client("46.174.48.20", 27209u);
    UdpClient client("46.174.48.86", 27015u);
    std::vector<uint8_t> request;
    ByteBuffer buffer(request);

    // Send info request
    buffer.Put(HeaderType::HEADER_SIMPLE);
    buffer.Put(SsqPacketType::A2S_INFO);
    A2SInfoPacket<uint8_t>().Serialize(buffer);
    client.Send((const char*)request.data(), (int)request.size());
    buffer.Clear();

    // Parse info response
    std::vector<uint8_t> response = client.Receive();
    ByteBuffer responseBuffer(response);
    HeaderType responseHeader;
    SsqPacketType responsePacket;
    responseBuffer.Get(responseHeader);
    responseBuffer.Get(responsePacket);
    if (responseHeader != HeaderType::HEADER_SIMPLE || responsePacket != SsqPacketType::S2C_INFO) {
        Console::GetInstance()->WPrintF(L"Response not expected\n");
        Console::GetInstance()->Pause();
        return 0;
    }
    S2CInfoPacket<uint8_t> infoResponse;
    infoResponse.Deserialize(responseBuffer);
    Console::GetInstance()->PrintF("Server Info:\n");
    infoResponse.Print();
    Console::GetInstance()->PrintF("\n");

    // Send player request
    buffer.Put(HeaderType::HEADER_SIMPLE);
    buffer.Put(SsqPacketType::A2S_PLAYER);
    A2SPlayerPacket<uint8_t>().Serialize(buffer);
    client.Send((const char*)request.data(), (int)request.size());
    buffer.Clear();

    // Parse player (challenge) response
    response = client.Receive();
    responseBuffer.Reset();
    responseBuffer.Get(responseHeader);
    responseBuffer.Get(responsePacket);
    if (responseHeader != HeaderType::HEADER_SIMPLE || responsePacket != SsqPacketType::S2C_CHALLENGE) {
        Console::GetInstance()->WPrintF(L"Response not expected\n");
        Console::GetInstance()->Pause();
        return 0;
    }
    S2CChallengePacket<uint8_t> chellengeResponse;
    chellengeResponse.Deserialize(responseBuffer);
    Console::GetInstance()->WPrintF(L"Challenge = %u\n", chellengeResponse.m_challenge);

    // Send player request
    buffer.Put(HeaderType::HEADER_SIMPLE);
    buffer.Put(SsqPacketType::A2S_PLAYER);
    A2SPlayerPacket<uint8_t>(chellengeResponse.m_challenge).Serialize(buffer);
    client.Send((const char*)request.data(), (int)request.size());
    buffer.Clear();

    // Parse player (challenge) response
    response = client.Receive();
    responseBuffer.Reset();
    responseBuffer.Get(responseHeader);
    responseBuffer.Get(responsePacket);
    S2CPlayerPacket<uint8_t> playerResponse(responseBuffer);
    Console::GetInstance()->PrintF("Players Info:\n");
    playerResponse.Print();

	Console::GetInstance()->WPrintF(L"Press any key to exit...\n");
	Console::GetInstance()->Pause();
	return 0;
}