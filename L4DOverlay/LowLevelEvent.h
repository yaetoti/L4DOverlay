#pragma once

class LowLevelEvent {
public:
    LowLevelEvent() = default;
    virtual ~LowLevelEvent() = default;

    LowLevelEvent(const LowLevelEvent& other) = delete;
    LowLevelEvent(LowLevelEvent&& other) = delete;
    LowLevelEvent& operator=(const LowLevelEvent& other) = delete;
    LowLevelEvent& operator=(LowLevelEvent&& other) = delete;

    virtual int GetHookType() const = 0;
};
