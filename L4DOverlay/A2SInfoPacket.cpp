#include "A2SInfoPacket.h"

A2SInfoPacket::A2SInfoPacket(SSQ_LONG challenge, std::string payload): m_payload(std::move(payload)), m_challenge(challenge) {
}

A2SInfoPacket::A2SInfoPacket(const A2SInfoPacket& other): m_payload(other.m_payload),
                                                          m_challenge(other.m_challenge) {
}

A2SInfoPacket& A2SInfoPacket::operator=(const A2SInfoPacket& other) {
    if (this == &other) {
        return *this;
    }

    m_payload = other.m_payload;
    m_challenge = other.m_challenge;
    return *this;
}

void A2SInfoPacket::Serialize(ByteBuffer& buffer) const {
    buffer.PutNullTerminatedString(m_payload);
    buffer.Put(m_challenge);
}

void A2SInfoPacket::Deserialize(ByteBuffer& buffer) {
    buffer.GetNullTerminatedString(m_payload);
    buffer.Get(m_challenge);
}

SsqPacketType A2SInfoPacket::GetType() const {
    return SsqPacketType::A2S_INFO;
}

std::unique_ptr<Packet<SsqPacketType>> A2SInfoPacket::Clone() const {
    return std::make_unique<A2SInfoPacket>(*this);
}
