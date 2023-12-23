#include "S2CInfoPacket.h"

S2CInfoPacket::S2CInfoPacket() = default;

S2CInfoPacket::S2CInfoPacket(const S2CInfoPacket& other): m_protocol(other.m_protocol),
                                                          m_serverName(other.m_serverName),
                                                          m_mapName(other.m_mapName),
                                                          m_gameFolder(other.m_gameFolder),
                                                          m_gameName(other.m_gameName),
                                                          m_gameId(other.m_gameId),
                                                          m_players(other.m_players),
                                                          m_maxPlayers(other.m_maxPlayers),
                                                          m_bots(other.m_bots),
                                                          m_serverType(other.m_serverType),
                                                          m_environment(other.m_environment),
                                                          m_visibility(other.m_visibility),
                                                          m_vac(other.m_vac),
                                                          m_mode(other.m_mode),
                                                          m_witnesses(other.m_witnesses),
                                                          m_duration(other.m_duration),
                                                          m_gameVersion(other.m_gameVersion),
                                                          m_extraDataFlag(other.m_extraDataFlag),
                                                          m_port(other.m_port),
                                                          m_serverSteamId(other.m_serverSteamId),
                                                          m_sourceTvPort(other.m_sourceTvPort),
                                                          m_sourceTvName(other.m_sourceTvName),
                                                          m_keywords(other.m_keywords),
                                                          m_gameId64(other.m_gameId64) {
}

S2CInfoPacket& S2CInfoPacket::operator=(const S2CInfoPacket& other) {
    if (this == &other) {
        return *this;
    }

    m_protocol = other.m_protocol;
    m_serverName = other.m_serverName;
    m_mapName = other.m_mapName;
    m_gameFolder = other.m_gameFolder;
    m_gameName = other.m_gameName;
    m_gameId = other.m_gameId;
    m_players = other.m_players;
    m_maxPlayers = other.m_maxPlayers;
    m_bots = other.m_bots;
    m_serverType = other.m_serverType;
    m_environment = other.m_environment;
    m_visibility = other.m_visibility;
    m_vac = other.m_vac;
    m_mode = other.m_mode;
    m_witnesses = other.m_witnesses;
    m_duration = other.m_duration;
    m_gameVersion = other.m_gameVersion;
    m_extraDataFlag = other.m_extraDataFlag;
    m_port = other.m_port;
    m_serverSteamId = other.m_serverSteamId;
    m_sourceTvPort = other.m_sourceTvPort;
    m_sourceTvName = other.m_sourceTvName;
    m_keywords = other.m_keywords;
    m_gameId64 = other.m_gameId64;
    return *this;
}

void S2CInfoPacket::Serialize(ByteBuffer& buffer) const {
    buffer.Put(m_protocol);
    buffer.PutNullTerminatedString(m_serverName);
    buffer.PutNullTerminatedString(m_mapName);
    buffer.PutNullTerminatedString(m_gameFolder);
    buffer.PutNullTerminatedString(m_gameName);
    buffer.Put(m_gameId);
    buffer.Put(m_players);
    buffer.Put(m_maxPlayers);
    buffer.Put(m_bots);
    buffer.Put(m_serverType);
    buffer.Put(m_environment);
    buffer.Put(m_visibility);
    buffer.Put(m_vac);
    if (m_gameId == SsqGameId::THE_SHIP) {
        buffer.Put(m_mode.value());
        buffer.Put(m_witnesses.value());
        buffer.Put(m_duration.value());
    }

    buffer.PutNullTerminatedString(m_gameVersion);
    buffer.Put(m_extraDataFlag);
    if (m_extraDataFlag & 0x80) {
        buffer.Put(m_port.value());
    }

    if (m_extraDataFlag & 0x10) {
        buffer.Put(m_serverSteamId.value());
    }

    if (m_extraDataFlag & 0x40) {
        buffer.Put(m_sourceTvPort.value());
        buffer.PutNullTerminatedString(m_sourceTvName.value());
    }

    if (m_extraDataFlag & 0x20) {
        buffer.PutNullTerminatedString(m_keywords.value());
    }

    if (m_extraDataFlag & 0x01) {
        buffer.Put(m_gameId64.value());
    }
}

void S2CInfoPacket::Deserialize(ByteBuffer& buffer) {
    buffer.Get(m_protocol);
    buffer.GetNullTerminatedString(m_serverName);
    buffer.GetNullTerminatedString(m_mapName);
    buffer.GetNullTerminatedString(m_gameFolder);
    buffer.GetNullTerminatedString(m_gameName);
    buffer.Get(m_gameId);
    buffer.Get(m_players);
    buffer.Get(m_maxPlayers);
    buffer.Get(m_bots);
    buffer.Get(m_serverType);
    buffer.Get(m_environment);
    buffer.Get(m_visibility);
    buffer.Get(m_vac);
    if (m_gameId == SsqGameId::THE_SHIP) {
        SsqServerMode mode;
        buffer.Get(mode);
        m_mode.emplace(mode);

        SSQ_BYTE witnesses;
        buffer.Get(witnesses);
        m_witnesses.emplace(witnesses);

        SSQ_BYTE duration;
        buffer.Get(duration);
        m_duration.emplace(duration);
    }

    buffer.GetNullTerminatedString(m_gameVersion);
    buffer.Get(m_extraDataFlag);
    if (m_extraDataFlag & 0x80) {
        SSQ_SHORT port;
        buffer.Get(port);
        m_port.emplace(port);
    }

    if (m_extraDataFlag & 0x10) {
        SSQ_LONG serverSteamId;
        buffer.Get(serverSteamId);
        m_serverSteamId.emplace(serverSteamId);
    }

    if (m_extraDataFlag & 0x40) {
        SSQ_SHORT sourceTvPort;
        buffer.Get(sourceTvPort);
        m_sourceTvPort.emplace(sourceTvPort);

        std::string sourceTvName;
        buffer.GetNullTerminatedString(sourceTvName);
        m_sourceTvName.emplace(std::move(sourceTvName));
    }

    if (m_extraDataFlag & 0x20) {
        std::string keywords;
        buffer.GetNullTerminatedString(keywords);
        m_keywords.emplace(std::move(keywords));
    }

    if (m_extraDataFlag & 0x01) {
        SSQ_LLONG gameId64;
        buffer.Get(gameId64);
        m_gameId64.emplace(gameId64);
    }
}

SsqPacketType S2CInfoPacket::GetType() const {
    return SsqPacketType::S2C_INFO;
}

std::unique_ptr<Packet<SsqPacketType>> S2CInfoPacket::Clone() const {
    return std::make_unique<S2CInfoPacket>(*this);
}

SSQ_BYTE S2CInfoPacket::GetProtocol() const {
    return m_protocol;
}

const std::string& S2CInfoPacket::GetServerName() const {
    return m_serverName;
}

const std::string& S2CInfoPacket::GetMapName() const {
    return m_mapName;
}

const std::string& S2CInfoPacket::GetGameFolder() const {
    return m_gameFolder;
}

const std::string& S2CInfoPacket::GetGameName() const {
    return m_gameName;
}

SsqGameId S2CInfoPacket::GetGameId() const {
    return m_gameId;
}

SSQ_BYTE S2CInfoPacket::GetPlayers() const {
    return m_players;
}

SSQ_BYTE S2CInfoPacket::GetMaxPlayers() const {
    return m_maxPlayers;
}

SSQ_BYTE S2CInfoPacket::GetBots() const {
    return m_bots;
}

SsqServerType S2CInfoPacket::GetServerType() const {
    return m_serverType;
}

SsqServerEnvironment S2CInfoPacket::GetEnvironment() const {
    return m_environment;
}

SsqServerVisibility S2CInfoPacket::GetVisibility() const {
    return m_visibility;
}

SsqServerVac S2CInfoPacket::GetVac() const {
    return m_vac;
}

const std::optional<SsqServerMode>& S2CInfoPacket::GetMode() const {
    return m_mode;
}

const std::optional<SSQ_BYTE>& S2CInfoPacket::GetWitnesses() const {
    return m_witnesses;
}

const std::optional<SSQ_BYTE>& S2CInfoPacket::GetDuration() const {
    return m_duration;
}

const std::string& S2CInfoPacket::GetGameVersion() const {
    return m_gameVersion;
}

SSQ_BYTE S2CInfoPacket::GetExtraDataFlag() const {
    return m_extraDataFlag;
}

const std::optional<SSQ_SHORT>& S2CInfoPacket::GetPort() const {
    return m_port;
}

const std::optional<SSQ_LLONG>& S2CInfoPacket::GetServerSteamId() const {
    return m_serverSteamId;
}

const std::optional<SSQ_SHORT>& S2CInfoPacket::GetSourceTvPort() const {
    return m_sourceTvPort;
}

const std::optional<std::string>& S2CInfoPacket::GetSourceTvName() const {
    return m_sourceTvName;
}

const std::optional<std::string>& S2CInfoPacket::GetKeywords() const {
    return m_keywords;
}

const std::optional<SSQ_LLONG>& S2CInfoPacket::GetGameId64() const {
    return m_gameId64;
}
