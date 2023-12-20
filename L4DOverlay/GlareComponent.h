#pragma once

#include <Windows.h>
#include <wrl/client.h>
#include <d2d1.h>
#include <d2d1helper.h>

#include "UiComponent.h"

class GlareComponent final : public UiComponent {
public:
	GlareComponent(ID2D1RenderTarget* target, float radiusX, float radiusY);
	~GlareComponent() override;

    GlareComponent(const GlareComponent& other) = delete;
    GlareComponent(GlareComponent&& other) = delete;
    GlareComponent& operator=(const GlareComponent& other) = delete;
    GlareComponent& operator=(GlareComponent&& other) = delete;

    HRESULT Initialize() override;
	void Render() const override;
    void Update() override;

private:
	float m_radiusX;
	float m_radiusY;
	D2D1_RECT_F m_rect;
	const D2D1_GRADIENT_STOP m_gradientStops[2] = {
		D2D1::GradientStop(0.9f, D2D1::ColorF(0xD9D9D9, 0.2f)),
		D2D1::GradientStop(1.0f, D2D1::ColorF(0xD9D9D9, 0.0f)),
	};

	Microsoft::WRL::ComPtr<ID2D1GradientStopCollection> m_gradient;
	Microsoft::WRL::ComPtr<ID2D1RadialGradientBrush> m_brush;
};

