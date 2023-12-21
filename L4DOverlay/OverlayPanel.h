#pragma once

#include <wrl/client.h>
#include <d2d1.h>
#include <d2d1_1.h>

#include "UiComponent.h"

class OverlayPanel final : public UiComponent {
public:
    explicit OverlayPanel(ID2D1RenderTarget* target)
        : UiComponent(target) {

    }

    ~OverlayPanel() override {
        
    }

    OverlayPanel(const OverlayPanel& other) = delete;
    OverlayPanel(OverlayPanel&& other) = delete;
    OverlayPanel& operator=(const OverlayPanel& other) = delete;
    OverlayPanel& operator=(OverlayPanel&& other) = delete;

    HRESULT Initialize() override {
        HRESULT status = S_OK;

        if (SUCCEEDED(status)) {
            status = m_renderTarget->QueryInterface(m_context.GetAddressOf());
        }

        return status;
    }

    void Render() const override {
        
    }

    void Update() override {
        
    }

private:
    Microsoft::WRL::ComPtr<ID2D1DeviceContext> m_context;
};
