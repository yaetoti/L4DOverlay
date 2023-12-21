#include "LowLevelKeyboardEvent.h"

LowLevelKeyboardEvent::LowLevelKeyboardEvent(WPARAM id, const KBDLLHOOKSTRUCT& info)
    : m_id(id), m_info(info) {
}

WPARAM LowLevelKeyboardEvent::GetId() const {
    return m_id;
}

const KBDLLHOOKSTRUCT& LowLevelKeyboardEvent::GetInfo() const {
    return m_info;
}

int LowLevelKeyboardEvent::GetHookType() const {
    return WH_KEYBOARD_LL;
}
