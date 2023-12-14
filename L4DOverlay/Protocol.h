#pragma once

#include <CommonLib/ByteBuffer.h>

// Types used by Source Server Query protocol
// Byte         8 bit character or unsigned integer
// Short        16 bit signed integer
// Long         32 bit signed integer
// Float	    32 bit floating point
// Long long    64 bit unsigned integer
// String	    Variable-length byte field, encoded in UTF8, terminated by \0

using SSQ_BYTE  = uint8_t;
using SSQ_SHORT = int16_t;
using SSQ_LONG  = int32_t;
using SSQ_FLOAT = float;
using SSQ_LLONG = uint64_t;

enum class HeaderType : SSQ_LONG {
    HEADER_SIMPLE   = SSQ_LONG(0xFFFFFFFF),
    HEADER_SPLIT    = SSQ_LONG(0xFFFFFFFE),
};

enum class SsqPacketType : SSQ_BYTE {
    S2C_CHALLENGE   = 0x41,
    S2C_PLAYER      = 0x44,
    S2C_INFO        = 0x49,

    A2S_INFO        = 0x54,
    A2S_PLAYER      = 0x55,
};

enum class SsqGameId : SSQ_SHORT {
    LEFT_4_DEAD     = 500,
    LEFT_4_DEAD_2   = 550,
    THE_SHIP        = 2400,
};

enum class SsqServerType : SSQ_BYTE {
    DEDICATED       = 'd',
    LOCAL           = 'l',
    PROXY           = 'p',
};

enum class SsqServerEnvironment : SSQ_BYTE {
    LINUX   = 'l',
    WINDOWS = 'w',
    MAC     = 'm',
    OTHER   = 'o',
};

enum class SsqServerVisibility : SSQ_BYTE {
    PUBLIC = 0,
    PRIVATE = 1,
};

enum class SsqServerVac : SSQ_BYTE {
    UNSECURED = 0,
    SECURED = 1,
};

enum class SsqServerMode : SSQ_BYTE {
    HUNT                = 0,
    ELIMINATION         = 1,
    DUEL                = 2,
    DEATHMATCH          = 3,
    VIP_TEAM            = 4,
    TEAM_ELIMINATION    = 5,
};

struct Player final {
    SSQ_BYTE m_index;
    std::string m_name;
    SSQ_LONG m_score;
    SSQ_FLOAT m_duration;

    Player() = default;
    explicit Player(ByteBuffer& buffer) {
        Deserialize(buffer);
    };

    void Serialize(ByteBuffer& buffer) const {
        buffer.Put(m_index);
        buffer.PutNullTerminatedString(m_name);
        buffer.Put(m_score);
        buffer.Put(m_duration);
    }

    void Deserialize(ByteBuffer& buffer) {
        buffer.Get(m_index);
        buffer.GetNullTerminatedString(m_name);
        buffer.Get(m_score);
        buffer.Get(m_duration);
    }
};

// Only for The Ship
struct PlayerExtra final {
    SSQ_LONG m_deaths;
    SSQ_LONG m_money;

    void Serialize(ByteBuffer& buffer) const {
        buffer.Put(m_deaths);
        buffer.Put(m_money);
    }

    void Deserialize(ByteBuffer& buffer) {
        buffer.Get(m_deaths);
        buffer.Get(m_money);
    }
};
