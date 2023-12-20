#include "GraphicsHelper.h"

#include <math.h>
#include <utility>

using Microsoft::WRL::ComPtr;

D2D1_ROUNDED_RECT_EX RoundedRectEx(
    const D2D1_RECT_F& rect,
    const D2D1_POINT_2F& radiusTopLeft,
    const D2D1_POINT_2F& radiusTopRight,
    const D2D1_POINT_2F& radiusBottomRight,
    const D2D1_POINT_2F& radiusBottomLeft) {

    D2D1_ROUNDED_RECT_EX result;
    result.rect = rect;
    result.radiusTopLeft = radiusTopLeft;
    result.radiusTopRight = radiusTopRight;
    result.radiusBottomRight = radiusBottomRight;
    result.radiusBottomLeft = radiusBottomLeft;
    return result;
}

D2D1_ROUNDED_RECT_EX RoundedRectEx(
    const D2D1_RECT_F& rect,
    FLOAT radiusTopLeft,
    FLOAT radiusTopRight,
    FLOAT radiusBottomRight,
    FLOAT radiusBottomLeft) {

    D2D1_ROUNDED_RECT_EX result;
    result.rect = rect;
    result.radiusTopLeft = D2D1::Point2F(radiusTopLeft, radiusTopLeft);
    result.radiusTopRight = D2D1::Point2F(radiusTopRight, radiusTopRight);
    result.radiusBottomRight = D2D1::Point2F(radiusBottomRight, radiusBottomRight);
    result.radiusBottomLeft = D2D1::Point2F(radiusBottomLeft, radiusBottomLeft);
    return result;
}

D2D1_FRAME Frame(const D2D1_RECT_F& rect, D2D1_POINT_2F corner, D2D1_POINT_2F diagonal) {

    D2D1_FRAME result;
    result.rect = rect;
    result.corner = corner;
    result.diagonal = diagonal;
    return result;
}

D2D1_BEZIER_SEGMENT BezierSegmentArc(const D2D1_POINT_2F& begin, const D2D1_POINT_2F& end, const D2D1_POINT_2F& center) {
    float ax = begin.x - center.x;
    float ay = begin.y - center.y;
    float bx = end.x - center.x;
    float by = end.y - center.y;
    float q1 = ax * ax + ay * ay;
    float q2 = q1 + ax * bx + ay * by;
    float k2 = (4.0f / 3.0f) * (sqrtf(2.0f * q1 * q2) - q2) / (ax * by - ay * bx);

    D2D1_BEZIER_SEGMENT result;
    result.point1.x = center.x + ax - k2 * ay;
    result.point1.y = center.y + ay + k2 * ax;
    result.point2.x = center.x + bx + k2 * by;
    result.point2.y = center.y + by - k2 * bx;
    result.point3 = end;
    return result;

    // TODO Support per-axis rounding (ellipsing?)
}

HRESULT CreateFillRoundedRectangleGeometryEx(ID2D1Factory1* factory, const D2D1_ROUNDED_RECT_EX& rect, _COM_Outptr_ ID2D1PathGeometry** roundedRectangleGeometry) {
    HRESULT status = S_OK;
    ComPtr<ID2D1PathGeometry> geometry;
    ComPtr<ID2D1GeometrySink> sink;
    if (SUCCEEDED(status)) {
        status = factory->CreatePathGeometry(geometry.GetAddressOf());
    }
    if (SUCCEEDED(status)) {
        status = geometry->Open(sink.GetAddressOf());
    }
    if (SUCCEEDED(status)) {
        // Clockwise, starting from the top line
        D2D1_POINT_2F points[] = {
            D2D1::Point2F(rect.rect.left + rect.radiusTopLeft.x, rect.rect.top),
            D2D1::Point2F(rect.rect.right - rect.radiusTopRight.x, rect.rect.top),

            D2D1::Point2F(rect.rect.right, rect.rect.top + rect.radiusTopRight.y),
            D2D1::Point2F(rect.rect.right, rect.rect.bottom - rect.radiusBottomRight.y),

            D2D1::Point2F(rect.rect.right - rect.radiusBottomRight.x, rect.rect.bottom),
            D2D1::Point2F(rect.rect.left + rect.radiusBottomLeft.x, rect.rect.bottom),

            D2D1::Point2F(rect.rect.left, rect.rect.bottom - rect.radiusBottomLeft.y),
            D2D1::Point2F(rect.rect.left, rect.rect.top + rect.radiusTopLeft.y),
        };
        // Clockwise, starting from the top right
        D2D1_POINT_2F centers[] = {
            D2D1::Point2F(rect.rect.right - rect.radiusTopRight.x, rect.rect.top + rect.radiusTopRight.y),
            D2D1::Point2F(rect.rect.right - rect.radiusBottomRight.x, rect.rect.bottom - rect.radiusBottomRight.y),
            D2D1::Point2F(rect.rect.left + rect.radiusBottomLeft.x, rect.rect.bottom - rect.radiusBottomLeft.y),
            D2D1::Point2F(rect.rect.left + rect.radiusTopLeft.x, rect.rect.top + rect.radiusTopLeft.y),
        };

        sink->SetFillMode(D2D1_FILL_MODE_WINDING);
        sink->BeginFigure(points[0], D2D1_FIGURE_BEGIN_FILLED);
        sink->AddLine(points[1]);
        if (rect.radiusTopRight.x != 0 || rect.radiusTopRight.y != 0) {
            sink->AddBezier(BezierSegmentArc(points[1], points[2], centers[0]));
        }
        sink->AddLine(points[2]);
        sink->AddLine(points[3]);
        if (rect.radiusBottomRight.x != 0 || rect.radiusBottomRight.y != 0) {
            sink->AddBezier(BezierSegmentArc(points[3], points[4], centers[1]));
        }
        sink->AddLine(points[4]);
        sink->AddLine(points[5]);
        if (rect.radiusBottomLeft.x != 0 || rect.radiusBottomLeft.y != 0) {
            sink->AddBezier(BezierSegmentArc(points[5], points[6], centers[2]));
        }
        sink->AddLine(points[6]);
        sink->AddLine(points[7]);
        if (rect.radiusTopLeft.x != 0 || rect.radiusTopLeft.y != 0) {
            sink->AddBezier(BezierSegmentArc(points[7], points[0], centers[3]));
        }
        sink->EndFigure(D2D1_FIGURE_END_CLOSED);
        sink->Close();
    }

    *roundedRectangleGeometry = geometry.Detach();
    return status;
}

HRESULT CreateFrameGeometry(ID2D1Factory1* factory, const D2D1_FRAME& frame, ID2D1PathGeometry** frameGeometry) {
    HRESULT status = S_OK;
    ComPtr<ID2D1PathGeometry> geometry;
    ComPtr<ID2D1GeometrySink> sink;
    if (SUCCEEDED(status)) {
        status = factory->CreatePathGeometry(geometry.GetAddressOf());
    }
    if (SUCCEEDED(status)) {
        status = geometry->Open(sink.GetAddressOf());
    }
    if (SUCCEEDED(status)) {
        // Clockwise, starting from the top left
        D2D1_POINT_2F points[] = {
            D2D1::Point2F(frame.rect.left, frame.rect.top),
            D2D1::Point2F(frame.rect.left + frame.corner.x, frame.rect.top),
            D2D1::Point2F(frame.rect.left + frame.corner.x + frame.diagonal.x, frame.rect.top + frame.diagonal.y),
            D2D1::Point2F(frame.rect.right - frame.corner.x - frame.diagonal.x, frame.rect.top + frame.diagonal.y),
            D2D1::Point2F(frame.rect.right - frame.corner.x, frame.rect.top),

            D2D1::Point2F(frame.rect.right, frame.rect.top),
            D2D1::Point2F(frame.rect.right, frame.rect.top + frame.corner.y),
            D2D1::Point2F(frame.rect.right - frame.diagonal.y, frame.rect.top + frame.corner.y + frame.diagonal.x),
            D2D1::Point2F(frame.rect.right - frame.diagonal.y, frame.rect.bottom - frame.corner.y - frame.diagonal.x),
            D2D1::Point2F(frame.rect.right, frame.rect.bottom - frame.corner.y),

            D2D1::Point2F(frame.rect.right, frame.rect.bottom),
            D2D1::Point2F(frame.rect.right - frame.corner.x, frame.rect.bottom),
            D2D1::Point2F(frame.rect.right - frame.corner.x - frame.diagonal.x, frame.rect.bottom - frame.diagonal.y),
            D2D1::Point2F(frame.rect.left + frame.corner.x + frame.diagonal.x, frame.rect.bottom - frame.diagonal.y),
            D2D1::Point2F(frame.rect.left + frame.corner.x, frame.rect.bottom),

            D2D1::Point2F(frame.rect.left, frame.rect.bottom),
            D2D1::Point2F(frame.rect.left, frame.rect.bottom - frame.corner.y),
            D2D1::Point2F(frame.rect.left + frame.diagonal.y, frame.rect.bottom - frame.corner.y - frame.diagonal.x),
            D2D1::Point2F(frame.rect.left + frame.diagonal.y, frame.rect.top + frame.corner.y + frame.diagonal.x),
            D2D1::Point2F(frame.rect.left, frame.rect.top + frame.corner.y),
        };

        sink->SetFillMode(D2D1_FILL_MODE_WINDING);
        sink->BeginFigure(points[0], D2D1_FIGURE_BEGIN_FILLED);
        sink->AddLines(points + 1, 19);
        sink->EndFigure(D2D1_FIGURE_END_CLOSED);
        sink->Close();
    }

    *frameGeometry = geometry.Detach();
    return status;
}

HRESULT LoadBitmapFromFile(ID2D1RenderTarget* target, IWICImagingFactory* factory, const wchar_t* uri, UINT width, UINT height, ID2D1Bitmap** ppBitmap) {
    ComPtr<IWICBitmapDecoder> decoder;
    ComPtr<IWICBitmapFrameDecode> source;
    ComPtr<IWICFormatConverter> converter;
    ComPtr <IWICBitmapScaler> scaler;
    HRESULT status = S_OK;

    if (SUCCEEDED(status)) {
        status = factory->CreateDecoderFromFilename(
            uri,
            nullptr,
            GENERIC_READ,
            WICDecodeMetadataCacheOnLoad,
            decoder.GetAddressOf()
        );
    }
    if (SUCCEEDED(status)) {
        status = decoder->GetFrame(0, source.GetAddressOf());
    }
    if (SUCCEEDED(status)) {
        status = factory->CreateFormatConverter(converter.GetAddressOf());
    }
    if (SUCCEEDED(status)) {
        status = converter->Initialize(
            source.Get(),
            GUID_WICPixelFormat32bppPBGRA,
            WICBitmapDitherTypeNone,
            nullptr,
            0.0f,
            WICBitmapPaletteTypeMedianCut
        );
    }
    if (SUCCEEDED(status)) {
        status = factory->CreateBitmapScaler(scaler.GetAddressOf());
    }
    if (SUCCEEDED(status)) {
        status = scaler->Initialize(converter.Get(), width, height, WICBitmapInterpolationModeFant);
    }
    if (SUCCEEDED(status)) {
        status = target->CreateBitmapFromWicBitmap(scaler.Get(), nullptr, ppBitmap);
    }

    return status;
}
