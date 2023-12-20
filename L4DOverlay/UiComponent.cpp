#include "UiComponent.h"

#include <cassert>

UiComponent::UiComponent(ID2D1RenderTarget* target): m_renderTarget(target) {
    assert(target);
}

UiComponent::~UiComponent() = default;
