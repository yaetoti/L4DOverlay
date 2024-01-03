#pragma once

#include "IGuiComponent.h"
#include "OverlayGraphics.h"

struct OverlayPanel final : IGuiComponent {
    explicit OverlayPanel(OverlayGraphics& graphics)
    : m_graphics(graphics) {
    }

    bool Initialize() override {
        return false;
    }

    void Update() override {

    }

    void Render() override {
        
    }

private:
    OverlayGraphics& m_graphics;
};
