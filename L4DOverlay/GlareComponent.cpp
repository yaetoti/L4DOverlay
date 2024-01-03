#include "GlareComponent.h"

GlareComponent::GlareComponent(ID2D1RenderTarget* target, float radiusX, float radiusY)
	: m_renderTarget(target), m_radiusX(radiusX), m_radiusY(radiusY), m_rect(D2D1::RectF(-radiusX, -radiusY, radiusX, radiusY)) {

}

GlareComponent::~GlareComponent() {
	m_brush.Reset();
	m_gradient.Reset();
}

bool GlareComponent::Initialize() {
	HRESULT status = S_OK;

	if (SUCCEEDED(status)) {
		status = m_renderTarget->CreateGradientStopCollection(m_gradientStops, 2, D2D1_GAMMA_2_2, D2D1_EXTEND_MODE_CLAMP, m_gradient.GetAddressOf());
	}
	if (SUCCEEDED(status)) {
		status = m_renderTarget->CreateRadialGradientBrush(
			D2D1::RadialGradientBrushProperties(D2D1::Point2F(), D2D1::Point2F(), m_radiusX, m_radiusY),
			m_gradient.Get(), m_brush.GetAddressOf()
		);
	}

	return status;
}

void GlareComponent::Render() {
	m_renderTarget->FillRectangle(m_rect, m_brush.Get());
}

void GlareComponent::Update() {
}
