#include "LowLevelInputHandler.h"

#include <CommonLib/ScopedHandle.h>
#include <bit>

LowLevelInputHandler* LowLevelInputHandler::m_instance = nullptr;

LowLevelInputHandler::~LowLevelInputHandler() {
    Stop();
}

DWORD LowLevelInputHandler::Start() {
    if (nullptr != m_eventThread) {
        return 0;
    }

    m_eventThread = CreateThread(nullptr, 0, EventProc, nullptr, 0, nullptr);
    if (nullptr == m_eventThread) {
        return GetLastError();
    }

    return 0;
}

void LowLevelInputHandler::Stop() {
    if (nullptr != m_eventThread) {
        PostThreadMessageW(GetThreadId(m_eventThread), WM_QUIT, 0, 0);
        WaitForSingleObject(m_eventThread, INFINITE);
        CloseHandle(m_eventThread);
        m_eventThread = nullptr;
    }
}

void LowLevelInputHandler::Clear() {
    std::lock_guard lock(GetInstance()->m_mutex);
    decltype(m_events) clear;
    m_events.swap(clear);
}

bool LowLevelInputHandler::Dispatch() {
    if (m_events.empty()) {
        return !m_error;
    }

    std::queue<std::unique_ptr<LowLevelEvent>> events;
    {
        std::lock_guard lock(GetInstance()->m_mutex);
        m_events.swap(events);
    }

    while (!events.empty()) {
        for (const auto& listener : m_keyboardListeners) {
            switch (const auto& event = events.front(); event->GetHookType()) {
            case WH_KEYBOARD_LL:
                listener.get()->operator()(reinterpret_cast<LowLevelKeyboardEvent&>(*event));
                break;
            default: break;
            }
        }

        events.pop();
    }

    return !m_error;
}

std::shared_ptr<LowLevelInputHandler::KeyboardCallback> LowLevelInputHandler::AddKeyboardListener(const KeyboardCallback& callback) {
    auto fp = std::make_shared<KeyboardCallback>(callback);
    m_keyboardListeners.push_back(fp);
    return fp;
}

std::shared_ptr<LowLevelInputHandler::KeyboardCancelCallback> LowLevelInputHandler::AddKeyboardCancelListener(const KeyboardCancelCallback& callback) {
    std::lock_guard lock(GetInstance()->m_mutex);
    auto fp = std::make_shared<KeyboardCancelCallback>(callback);
    m_keyboardCancelListeners.push_back(fp);
    return fp;
}

void LowLevelInputHandler::RemoveKeyboardListener(const std::shared_ptr<KeyboardCallback>& callback) {
    if (const auto& iterator = std::find(m_keyboardListeners.rbegin(), m_keyboardListeners.rend(), callback);
        iterator != m_keyboardListeners.rend()) {
        m_keyboardListeners.erase(std::next(iterator).base());
    }
}

void LowLevelInputHandler::RemoveKeyboardCancelListener(const std::shared_ptr<KeyboardCancelCallback>& callback) {
    if (const auto& iterator = std::find(m_keyboardCancelListeners.rbegin(), m_keyboardCancelListeners.rend(), callback);
        iterator != m_keyboardCancelListeners.rend()) {
        m_keyboardCancelListeners.erase(std::next(iterator).base());
    }
}

LowLevelInputHandler* LowLevelInputHandler::GetInstance() {
    if (nullptr == m_instance) {
        m_instance = new LowLevelInputHandler();
    }

    return m_instance;
}

LowLevelInputHandler::LowLevelInputHandler() = default;

DWORD LowLevelInputHandler::EventProc(LPVOID param) {
    ScopedHandle<HHOOK> keyboardHook(
        SetWindowsHookExW(WH_KEYBOARD_LL, KeyboardHook, GetModuleHandleW(nullptr), 0),
        UnhookWindowsHookEx
    );
    if (!keyboardHook) {
        GetInstance()->m_error = true;
        return GetLastError();
    }

    MSG message;
    while (0 != GetMessageW(&message, nullptr, 0, 0)) {
    }

    return 0;
}

LRESULT LowLevelInputHandler::KeyboardHook(int nCode, WPARAM wParam, LPARAM lParam) {
    if (nCode < 0) {
        return CallNextHookEx(nullptr, nCode, wParam, lParam);
    }

    std::lock_guard lock(GetInstance()->m_mutex);
    auto event = std::make_unique<LowLevelKeyboardEvent>(wParam, *std::bit_cast<KBDLLHOOKSTRUCT*>(lParam));
    bool cancel = false;
    for (const auto& listener : GetInstance()->m_keyboardCancelListeners) {
        if (listener.get()->operator()(*event)) {
            cancel = true;
            break;
        }
    }

    GetInstance()->m_events.emplace(std::move(event));
    return cancel ? 1 : CallNextHookEx(nullptr, nCode, wParam, lParam);
}
