#pragma once

#include <NetworkLib/Packet.h>
#include <ConsoleLib/Console.h>
#include "Protocol.h"
#include <chrono>

template<typename Handler>
class A2SInfoPacket : public Packet<SsqPacketType, Handler> {
public:
	explicit A2SInfoPacket(SSQ_LONG challenge = SSQ_LONG(-1), const std::string& payload = "Source Engine Query")
		: m_challenge(challenge), m_payload(payload) { }

	void Serialize(ByteBuffer& buffer) const override {
		buffer.PutNullTerminatedString(m_payload);
		buffer.Put(m_challenge);
	}

	void Deserialize(ByteBuffer& buffer) override {
		buffer.GetNullTerminatedString(m_payload);
		buffer.Get(m_challenge);
	}

	void Apply(Handler& handler) override {
		throw std::runtime_error("Not implemented");
	}

	SsqPacketType GetType() const override {
		return SsqPacketType::A2S_INFO;
	}

	std::unique_ptr<Packet<SsqPacketType, Handler>> Clone() const override {
		return std::make_unique<A2SInfoPacket<Handler>>();
	}

private:
	std::string m_payload;
	SSQ_LONG m_challenge;
};

template<typename Handler>
class A2SPlayerPacket: public Packet<SsqPacketType, Handler> {
public:
	explicit A2SPlayerPacket(SSQ_LONG challenge = SSQ_LONG(-1))
		: m_challenge(challenge) { }

	void Serialize(ByteBuffer& buffer) const override {
		buffer.Put(m_challenge);
	}

	void Deserialize(ByteBuffer& buffer) override {
		buffer.Get(m_challenge);
	}

	void Apply(Handler& handler) override {
		throw std::runtime_error("Not implemented");
	}

	SsqPacketType GetType() const override {
		return SsqPacketType::A2S_PLAYER;
	}

	std::unique_ptr<Packet<SsqPacketType, Handler>> Clone() const override {
		return std::make_unique<A2SPlayerPacket<Handler>>();
	}

private:
	SSQ_LONG m_challenge;
};

// S2C

template<typename Handler>
class S2CChallengePacket : public Packet<SsqPacketType, Handler> {
public:
	explicit S2CChallengePacket(SSQ_LONG challenge = SSQ_LONG(-1))
		: m_challenge(challenge) { }

	void Serialize(ByteBuffer& buffer) const override {
		buffer.Put(m_challenge);
	}

	void Deserialize(ByteBuffer& buffer) override {
		buffer.Get(m_challenge);
	}

	void Apply(Handler& handler) override {
		throw std::runtime_error("Not implemented");
	}

	SsqPacketType GetType() const override {
		return SsqPacketType::S2C_CHALLENGE;
	}

	std::unique_ptr<Packet<SsqPacketType, Handler>> Clone() const override {
		return std::make_unique<S2CChallengePacket<Handler>>();
	}

public: // TODO: private, write getters
	SSQ_LONG m_challenge;
};

template<typename Handler>
class S2CInfoPacket : public Packet<SsqPacketType, Handler> {
public:
	S2CInfoPacket() = default;

	void Serialize(ByteBuffer& buffer) const override {
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

	void Deserialize(ByteBuffer& buffer) override {
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

	void Apply(Handler& handler) override {
		throw std::runtime_error("Not implemented");
	}

	SsqPacketType GetType() const override {
		return SsqPacketType::S2C_INFO;
	}

	std::unique_ptr<Packet<SsqPacketType, Handler>> Clone() const override {
		return std::make_unique<S2CInfoPacket<Handler>>();
	}


	void Print() const {
		Console::GetInstance()->PrintF("- Protocol: %lld\n", (int64_t)m_protocol);
		Console::GetInstance()->PrintF("- ServerName: %s\n", m_serverName.c_str());
		Console::GetInstance()->PrintF("- MapName: %s\n", m_mapName.c_str());
		Console::GetInstance()->PrintF("- GameFolder: %s\n", m_gameFolder.c_str());
		Console::GetInstance()->PrintF("- GameName: %s\n", m_gameName.c_str());
		Console::GetInstance()->PrintF("- GameId: %lld\n", (int64_t)m_gameId);
		Console::GetInstance()->PrintF("- Players: %lld\n", (int64_t)m_players);
		Console::GetInstance()->PrintF("- MaxPlayers: %lld\n", (int64_t)m_maxPlayers);
		Console::GetInstance()->PrintF("- Bots: %lld\n", (int64_t)m_bots);
		Console::GetInstance()->PrintF("- ServerType: %lld\n", (int64_t)m_serverType);
		Console::GetInstance()->PrintF("- Environment: %lld\n", (int64_t)m_environment);
		Console::GetInstance()->PrintF("- Visibility: %lld\n", (int64_t)m_visibility);
		Console::GetInstance()->PrintF("- VAC: %lld\n", (int64_t)m_vac);
		if (m_gameId == SsqGameId::THE_SHIP) {
			Console::GetInstance()->PrintF("- Mode: %lld\n", (int64_t)m_mode.value());
			Console::GetInstance()->PrintF("- Witnesses: %lld\n", (int64_t)m_witnesses.value());
			Console::GetInstance()->PrintF("- Duration: %lld\n", (int64_t)m_duration.value());
		}

		Console::GetInstance()->PrintF("- GameVersion: %s\n", m_gameVersion.c_str());
		Console::GetInstance()->PrintF("- ExtraDataFlag: %lld\n", (int64_t)m_extraDataFlag);
		if (m_extraDataFlag & 0x80) {
			Console::GetInstance()->PrintF("- Port: %lld\n", (int64_t)m_port.value());
		}

		if (m_extraDataFlag & 0x10) {
			Console::GetInstance()->PrintF("- SteamID: %llu\n", (uint64_t)m_serverSteamId.value());
		}

		if (m_extraDataFlag & 0x40) {
			Console::GetInstance()->PrintF("- SourceTVPort: %lld\n", (int64_t)m_sourceTvPort.value());
			Console::GetInstance()->PrintF("- SourceTVName: %s\n", m_sourceTvName.value().c_str());
		}

		if (m_extraDataFlag & 0x20) {
			Console::GetInstance()->PrintF("- Keywords: %s\n", m_keywords.value().c_str());
		}

		if (m_extraDataFlag & 0x01) {
			Console::GetInstance()->PrintF("- GameID64: %llu\n", (uint64_t)m_gameId64.value());
		}
	}

public: // TODO: private, write getters
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

template<typename Handler>
class S2CPlayerPacket : public Packet<SsqPacketType, Handler> {
public:
	explicit S2CPlayerPacket() = default;
	explicit S2CPlayerPacket(ByteBuffer& buffer) {
		Deserialize(buffer);
	};

	void Serialize(ByteBuffer& buffer) const override {
		buffer.Put(m_players);
		for (SSQ_BYTE i = 0; i < m_players; ++i) {
			m_playerList[i]->Serialize(buffer);
		}
	}

	void Deserialize(ByteBuffer& buffer) override {
		buffer.Get(m_players);
		for (SSQ_BYTE i = 0; i < m_players; ++i) {
			m_playerList.emplace_back(std::make_unique<Player>(buffer));
		}
	}

	void Apply(Handler& handler) override {
		throw std::runtime_error("Not implemented");
	}

	SsqPacketType GetType() const override {
		return SsqPacketType::S2C_PLAYER;
	}

	std::unique_ptr<Packet<SsqPacketType, Handler>> Clone() const override {
		return std::make_unique<S2CPlayerPacket<Handler>>();
	}

	void Print() const {
		Console::GetInstance()->PrintF("- Players: %llu\n", (int64_t)m_players);
		for (const auto& player : m_playerList) {
			Console::GetInstance()->PrintF("  - ChunkIndex: %llu\n", (int64_t)player->m_index);
			Console::GetInstance()->PrintF("  - Name: %s\n", player->m_name.c_str());
			Console::GetInstance()->PrintF("  - Score: %llu\n", (int64_t)player->m_score);

			auto duration = std::chrono::duration<float>(player->m_duration);
			auto hours = std::chrono::duration_cast<std::chrono::hours>(duration);
			auto minutes = std::chrono::duration_cast<std::chrono::minutes>(duration - hours);
			auto seconds = std::chrono::duration_cast<std::chrono::seconds>(duration - hours - minutes);

			Console::GetInstance()->PrintF("  - Time: %d:%d:%d\n", hours.count(), minutes.count(), seconds.count());
			Console::GetInstance()->PrintF("\n");
		}
	}

public: // TODO: private, write getters
	SSQ_BYTE m_players;
	std::vector<std::unique_ptr<Player>> m_playerList;
	std::vector<std::unique_ptr<PlayerExtra>> m_playerExtraList;
};
