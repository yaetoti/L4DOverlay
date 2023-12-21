#pragma once

#include <Windows.h>
#include "LowLevelEvent.h"

class LowLevelKeyboardEvent final : public LowLevelEvent {
public:
    LowLevelKeyboardEvent(WPARAM id, const KBDLLHOOKSTRUCT& info);

    WPARAM GetId() const;
    const KBDLLHOOKSTRUCT& GetInfo() const;
    int GetHookType() const override;

private:
    WPARAM m_id;
    KBDLLHOOKSTRUCT m_info;
};
