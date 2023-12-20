#pragma once

#include <Windows.h>
#include <combaseapi.h>
#include <wrl/client.h>

#include <d2d1.h>
#include <d2d1helper.h>
#include <d2d1_1.h>
#include <d2d1_1helper.h>

#include <wincodec.h>

struct D2D1_ROUNDED_RECT_EX {
    D2D1_RECT_F rect;
    D2D1_POINT_2F radiusTopLeft;
    D2D1_POINT_2F radiusTopRight;
    D2D1_POINT_2F radiusBottomRight;
    D2D1_POINT_2F radiusBottomLeft;
};

struct D2D1_FRAME {
    D2D1_RECT_F rect;
    D2D1_POINT_2F corner;
    D2D1_POINT_2F diagonal;
};

D2D1_ROUNDED_RECT_EX RoundedRectEx(
    const D2D1_RECT_F& rect,
    const D2D1_POINT_2F& radiusTopLeft,
    const D2D1_POINT_2F& radiusTopRight,
    const D2D1_POINT_2F& radiusBottomRight,
    const D2D1_POINT_2F& radiusBottomLeft);

D2D1_ROUNDED_RECT_EX RoundedRectEx(
    const D2D1_RECT_F& rect,
    FLOAT radiusTopLeft,
    FLOAT radiusTopRight,
    FLOAT radiusBottomRight,
    FLOAT radiusBottomLeft);

D2D1_FRAME Frame(
    const D2D1_RECT_F& rect,
    D2D1_POINT_2F corner,
    D2D1_POINT_2F diagonal);

D2D1_BEZIER_SEGMENT BezierSegmentArc(const D2D1_POINT_2F& begin, const D2D1_POINT_2F& end, const D2D1_POINT_2F& center);

HRESULT CreateFillRoundedRectangleGeometryEx(ID2D1Factory1* factory, const D2D1_ROUNDED_RECT_EX& rect, _COM_Outptr_ ID2D1PathGeometry** roundedRectangleGeometry);
HRESULT CreateFrameGeometry(ID2D1Factory1* factory, const D2D1_FRAME& frame, _COM_Outptr_ ID2D1PathGeometry** frameGeometry);

HRESULT LoadBitmapFromFile(ID2D1RenderTarget* pRenderTarget, IWICImagingFactory* pIWICFactory, const wchar_t* uri, UINT width, UINT height, _COM_Outptr_ ID2D1Bitmap** ppBitmap);
