#pragma once

#include <NetworkLib/Packet.h>
#include <optional>
#include "Protocol.h"

struct S2CInfoPacket final : Packet<SsqPacketType> {
	S2CInfoPacket();
    S2CInfoPacket(const S2CInfoPacket& other);

    S2CInfoPacket& operator=(const S2CInfoPacket& other);

    void Serialize(ByteBuffer& buffer) const override;
    void Deserialize(ByteBuffer& buffer) override;
    SsqPacketType GetType() const override;
    std::unique_ptr<Packet> Clone() const override;

    SSQ_BYTE GetProtocol() const;
    const std::string& GetServerName() const;
    const std::string& GetMapName() const;
    const std::string& GetGameFolder() const;
    const std::string& GetGameName() const;
    SsqGameId GetGameId() const;
    SSQ_BYTE GetPlayers() const;
    SSQ_BYTE GetMaxPlayers() const;
    SSQ_BYTE GetBots() const;
    SsqServerType GetServerType() const;
    SsqServerEnvironment GetEnvironment() const;
    SsqServerVisibility GetVisibility() const;
    SsqServerVac GetVac() const;
    const std::optional<SsqServerMode>& GetMode() const;
    const std::optional<SSQ_BYTE>& GetWitnesses() const;
    const std::optional<SSQ_BYTE>& GetDuration() const;
    const std::string& GetGameVersion() const;
    SSQ_BYTE GetExtraDataFlag() const;
    const std::optional<SSQ_SHORT>& GetPort() const;
    const std::optional<SSQ_LLONG>& GetServerSteamId() const;
    const std::optional<SSQ_SHORT>& GetSourceTvPort() const;
    const std::optional<std::string>& GetSourceTvName() const;
    const std::optional<std::string>& GetKeywords() const;
    const std::optional<SSQ_LLONG>& GetGameId64() const;

private:
	SSQ_BYTE m_protocol;
	std::string m_serverName;
	std::string m_mapName;
	std::string m_gameFolder;
	std::string m_gameName;
	SsqGameId m_gameId;
	SSQ_BYTE m_players;
	SSQ_BYTE m_maxPlayers;
	SSQ_BYTE m_bots;
	SsqServerType m_serverType;
	SsqServerEnvironment m_environment;
	SsqServerVisibility m_visibility;
	SsqServerVac m_vac;
	std::optional<SsqServerMode> m_mode;		// Only for The Ship
	std::optional<SSQ_BYTE> m_witnesses;		// Only for The Ship
	std::optional<SSQ_BYTE> m_duration;			// Only for The Ship
	std::string m_gameVersion;
	SSQ_BYTE m_extraDataFlag;
	std::optional<SSQ_SHORT> m_port;			// Only if (EDF & 0x80)
	std::optional<SSQ_LLONG> m_serverSteamId;	// Only if (EDF & 0x10)
	std::optional<SSQ_SHORT> m_sourceTvPort;	// Only if (EDF & 0x40)
	std::optional<std::string> m_sourceTvName;	// Only if (EDF & 0x40)
	std::optional<std::string> m_keywords;		// Only if (EDF & 0x20)
	std::optional<SSQ_LLONG> m_gameId64;		// Only if (EDF & 0x01)
};
