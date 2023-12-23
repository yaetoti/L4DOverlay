#include "S2CChallengePacket.h"

S2CChallengePacket::S2CChallengePacket(SSQ_LONG challenge): m_challenge(challenge) {
}

S2CChallengePacket::S2CChallengePacket(const S2CChallengePacket& other): m_challenge(other.m_challenge) {
}

S2CChallengePacket& S2CChallengePacket::operator=(const S2CChallengePacket& other) {
    if (this == &other) {
        return *this;
    }
    m_challenge = other.m_challenge;
    return *this;
}

void S2CChallengePacket::Serialize(ByteBuffer& buffer) const {
    buffer.Put(m_challenge);
}

void S2CChallengePacket::Deserialize(ByteBuffer& buffer) {
    buffer.Get(m_challenge);
}

SsqPacketType S2CChallengePacket::GetType() const {
    return SsqPacketType::S2C_CHALLENGE;
}

std::unique_ptr<Packet<SsqPacketType>> S2CChallengePacket::Clone() const {
    return std::make_unique<S2CChallengePacket>(*this);
}

SSQ_LONG S2CChallengePacket::GetChallenge() const {
    return m_challenge;
}
