#pragma once

#include <Windows.h>
#include <queue>
#include <mutex>
#include <atomic>
#include <functional>

#include "LowLevelKeyboardEvent.h"

class LowLevelInputHandler final {
    using KeyboardCallback = std::function<void(const LowLevelKeyboardEvent&)>;
    using KeyboardCancelCallback = std::function<bool(const LowLevelKeyboardEvent&)>;

public:
    ~LowLevelInputHandler();

    LowLevelInputHandler(const LowLevelInputHandler& other) = delete;
    LowLevelInputHandler(LowLevelInputHandler&& other) = delete;
    LowLevelInputHandler& operator=(const LowLevelInputHandler& other) = delete;
    LowLevelInputHandler& operator=(LowLevelInputHandler&& other) = delete;

    DWORD Start();
    void Stop();
    void Clear();
    bool Dispatch();

    std::shared_ptr<KeyboardCallback> AddKeyboardListener(const KeyboardCallback& callback);
    std::shared_ptr<KeyboardCancelCallback> AddKeyboardCancelListener(const KeyboardCancelCallback& callback);
    void RemoveKeyboardListener(const std::shared_ptr<KeyboardCallback>& callback);
    void RemoveKeyboardCancelListener(const std::shared_ptr<KeyboardCancelCallback>& callback);

    static LowLevelInputHandler* GetInstance();

private:
    static LowLevelInputHandler* m_instance;

    std::atomic<bool> m_error = false;
    HANDLE m_eventThread = nullptr;
    std::mutex m_mutex;

    std::queue<std::unique_ptr<LowLevelEvent>> m_events;
    std::vector<std::shared_ptr<KeyboardCallback>> m_keyboardListeners;
    std::vector<std::shared_ptr<KeyboardCancelCallback>> m_keyboardCancelListeners;

    LowLevelInputHandler();

    static DWORD EventProc(LPVOID param);
    static LRESULT CALLBACK KeyboardHook(int nCode, WPARAM wParam, LPARAM lParam);
};
