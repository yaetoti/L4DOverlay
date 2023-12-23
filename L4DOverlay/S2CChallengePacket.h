#pragma once

#include <NetworkLib/Packet.h>
#include "Protocol.h"

struct S2CChallengePacket final : Packet<SsqPacketType> {
	explicit S2CChallengePacket(SSQ_LONG challenge = -1);
    S2CChallengePacket(const S2CChallengePacket& other);

    S2CChallengePacket& operator=(const S2CChallengePacket& other);

    void Serialize(ByteBuffer& buffer) const override;
    void Deserialize(ByteBuffer& buffer) override;
    SsqPacketType GetType() const override;
    std::unique_ptr<Packet> Clone() const override;

    SSQ_LONG GetChallenge() const;

private:
	SSQ_LONG m_challenge;
};

