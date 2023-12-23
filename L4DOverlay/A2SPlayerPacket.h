#pragma once

#include <NetworkLib/Packet.h>
#include "Protocol.h"

struct A2SPlayerPacket final : Packet<SsqPacketType> {
	explicit A2SPlayerPacket(SSQ_LONG challenge = -1);
    A2SPlayerPacket(const A2SPlayerPacket& other);

    A2SPlayerPacket& operator=(const A2SPlayerPacket& other);

    void Serialize(ByteBuffer& buffer) const override;
    void Deserialize(ByteBuffer& buffer) override;
    SsqPacketType GetType() const override;
    std::unique_ptr<Packet> Clone() const override;

private:
	SSQ_LONG m_challenge;
};
