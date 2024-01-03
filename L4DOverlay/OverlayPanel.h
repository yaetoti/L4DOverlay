#pragma once

#include <wrl/client.h>
#include <d2d1.h>
#include <d2d1_1.h>

#include "IGuiComponent.h"
#include <memory>

struct OverlayPanel final : IGuiComponent {
    explicit OverlayPanel(Microsoft::WRL::ComPtr<ID2D1DeviceContext> context)
        : m_context(std::move(context)) {
    }

    bool Initialize() override {
        return S_OK;
    }

    void Update() override {

    }

    void Render() override {
        
    }

private:
    Microsoft::WRL::ComPtr<ID2D1DeviceContext> m_context;
};
