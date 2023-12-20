#include "TransformStack.h"

TransformStack::TransformStack(ID2D1RenderTarget* renderTarget): m_renderTarget(renderTarget) {
    assert(renderTarget);
    D2D1_MATRIX_3X2_F current;
    m_renderTarget->GetTransform(&current);
    m_matrixStack.emplace(current);
}

void TransformStack::Reset() {
    while (m_matrixStack.size() > 1) {
        m_matrixStack.pop();
    }

    m_renderTarget->SetTransform(m_matrixStack.top());
}

void TransformStack::Pop() {
    if (m_matrixStack.size() > 1) {
        m_matrixStack.pop();
        m_renderTarget->SetTransform(m_matrixStack.top());
    }
}

void TransformStack::Push(const D2D1_MATRIX_3X2_F& transform) {
    D2D1_MATRIX_3X2_F newTransform = m_matrixStack.top() * transform;
    m_renderTarget->SetTransform(newTransform);
    m_matrixStack.emplace(newTransform);
}

void TransformStack::Translate(float x, float y) {
    Push(D2D1::Matrix3x2F::Translation(x, y));
}

void TransformStack::Rotate(float angle, const D2D1_POINT_2F& center) {
    Push(D2D1::Matrix3x2F::Rotation(angle, center));
}
