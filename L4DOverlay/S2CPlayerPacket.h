#pragma once

#include <NetworkLib/Packet.h>
#include "Protocol.h"

struct S2CPlayerPacket final : Packet<SsqPacketType> {
	explicit S2CPlayerPacket();
    S2CPlayerPacket(const S2CPlayerPacket& other);

    S2CPlayerPacket& operator=(const S2CPlayerPacket& other);

    explicit S2CPlayerPacket(ByteBuffer& buffer);;
	void Serialize(ByteBuffer& buffer) const override;
    void Deserialize(ByteBuffer& buffer) override;
    SsqPacketType GetType() const override;
    std::unique_ptr<Packet> Clone() const override;

    SSQ_BYTE GetPlayers() const;
    const std::vector<std::unique_ptr<Player>>& GetPlayerList() const;
    const std::vector<std::unique_ptr<PlayerExtra>>& GetPlayerExtraList() const;

private:
	SSQ_BYTE m_players;
	std::vector<std::unique_ptr<Player>> m_playerList;
	std::vector<std::unique_ptr<PlayerExtra>> m_playerExtraList;
};
