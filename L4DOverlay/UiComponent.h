#pragma once

#include <d2d1.h>

class UiComponent {
public:
    explicit UiComponent(ID2D1RenderTarget* target);
    virtual ~UiComponent();

    UiComponent(const UiComponent& other) = delete;
    UiComponent(UiComponent&& other) = delete;
    UiComponent& operator=(const UiComponent& other) = delete;
    UiComponent& operator=(UiComponent&& other) = delete;

    virtual HRESULT Initialize() = 0;
    virtual void Render() const = 0;
    virtual void Update() = 0;

protected:
    ID2D1RenderTarget* m_renderTarget;
};
