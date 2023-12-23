#include "A2SPlayerPacket.h"

A2SPlayerPacket::A2SPlayerPacket(SSQ_LONG challenge): m_challenge(challenge) {
}

A2SPlayerPacket::A2SPlayerPacket(const A2SPlayerPacket& other): m_challenge(other.m_challenge) {
}

A2SPlayerPacket& A2SPlayerPacket::operator=(const A2SPlayerPacket& other) {
    if (this == &other) {
        return *this;
    }

    m_challenge = other.m_challenge;
    return *this;
}

void A2SPlayerPacket::Serialize(ByteBuffer& buffer) const {
    buffer.Put(m_challenge);
}

void A2SPlayerPacket::Deserialize(ByteBuffer& buffer) {
    buffer.Get(m_challenge);
}

SsqPacketType A2SPlayerPacket::GetType() const {
    return SsqPacketType::A2S_PLAYER;
}

std::unique_ptr<Packet<SsqPacketType>> A2SPlayerPacket::Clone() const {
    return std::make_unique<A2SPlayerPacket>(*this);
}
