#include "S2CPlayerPacket.h"

S2CPlayerPacket::S2CPlayerPacket() = default;

S2CPlayerPacket::S2CPlayerPacket(const S2CPlayerPacket& other)
: m_players(other.m_players) {
    for (const auto& ptr : other.m_playerList) {
        m_playerList.push_back(std::make_unique<Player>(*ptr));
    }
    for (const auto& ptr : other.m_playerExtraList) {
        m_playerExtraList.push_back(std::make_unique<PlayerExtra>(*ptr));
    }
}

S2CPlayerPacket& S2CPlayerPacket::operator=(const S2CPlayerPacket& other) {
    if (this == &other) {
        return *this;
    }
    m_players = other.m_players;
    for (const auto& ptr : other.m_playerList) {
        m_playerList.push_back(std::make_unique<Player>(*ptr));
    }
    for (const auto& ptr : other.m_playerExtraList) {
        m_playerExtraList.push_back(std::make_unique<PlayerExtra>(*ptr));
    }
    return *this;
}

S2CPlayerPacket::S2CPlayerPacket(ByteBuffer& buffer) {
    Deserialize(buffer);
}

void S2CPlayerPacket::Serialize(ByteBuffer& buffer) const {
    buffer.Put(m_players);
    for (SSQ_BYTE i = 0; i < m_players; ++i) {
        m_playerList[i]->Serialize(buffer);
    }
}

void S2CPlayerPacket::Deserialize(ByteBuffer& buffer) {
    buffer.Get(m_players);
    for (SSQ_BYTE i = 0; i < m_players; ++i) {
        m_playerList.emplace_back(std::make_unique<Player>(buffer));
    }
}

SsqPacketType S2CPlayerPacket::GetType() const {
    return SsqPacketType::S2C_PLAYER;
}

std::unique_ptr<Packet<SsqPacketType>> S2CPlayerPacket::Clone() const {
    return std::make_unique<S2CPlayerPacket>(*this);
}

SSQ_BYTE S2CPlayerPacket::GetPlayers() const {
    return m_players;
}

const std::vector<std::unique_ptr<Player>>& S2CPlayerPacket::GetPlayerList() const {
    return m_playerList;
}

const std::vector<std::unique_ptr<PlayerExtra>>& S2CPlayerPacket::GetPlayerExtraList() const {
    return m_playerExtraList;
}
