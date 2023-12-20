#pragma once

#include <d2d1.h>
#include <d2d1helper.h>
#include <stack>
#include <cassert>

class TransformStack final {
public:
    explicit TransformStack(ID2D1RenderTarget* renderTarget);

    void Reset();
    void Pop();
    void Push(const D2D1_MATRIX_3X2_F& transform);
    void Translate(float x, float y);
    void Rotate(float angle, const D2D1_POINT_2F& center = D2D1::Point2F());

private:
    ID2D1RenderTarget* m_renderTarget;
    std::stack<D2D1_MATRIX_3X2_F> m_matrixStack;
};
