#pragma once

#include <NetworkLib/Packet.h>
#include "Protocol.h"

struct A2SInfoPacket final : Packet<SsqPacketType> {
	explicit A2SInfoPacket(SSQ_LONG challenge = -1, std::string payload = "Source Engine Query");
	A2SInfoPacket(const A2SInfoPacket& other);

	A2SInfoPacket& operator=(const A2SInfoPacket& other);

	void Serialize(ByteBuffer& buffer) const override;
	void Deserialize(ByteBuffer& buffer) override;
	SsqPacketType GetType() const override;
	std::unique_ptr<Packet> Clone() const override;

private:
	std::string m_payload;
	SSQ_LONG m_challenge;
};
