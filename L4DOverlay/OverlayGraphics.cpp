#include "OverlayGraphics.h"

OverlayGraphics::OverlayGraphics(HWND hWnd)
: m_hWnd(hWnd) {
}

OverlayGraphics::~OverlayGraphics() {
    DiscardDeviceResources();
    DiscardDeviceIndependentResources();
}

bool OverlayGraphics::Initialize() {
    return CreateDeviceIndependentResources() && CreateDeviceResources();
}

void OverlayGraphics::Render() {
}

void OverlayGraphics::Update() {
}

IDXGIFactory2* OverlayGraphics::GetFactoryDxgi() const {
    return m_dxgiFactory.Get();
}

ID2D1Factory1* OverlayGraphics::GetFactory2D() const {
    return m_d2dFactory.Get();
}

ID3D11Device* OverlayGraphics::GetDevice3D() const {
    return m_d3dDevice.Get();
}

IDXGIDevice* OverlayGraphics::GetDeviceDxgi() const {
    return m_dxgiDevice.Get();
}

ID2D1Device* OverlayGraphics::GetDevice2D() const {
    return m_d2dDevice.Get();
}

IDCompositionDevice* OverlayGraphics::GetDeviceDcomp() const {
    return m_dcompDevice.Get();
}

ID3D11DeviceContext* OverlayGraphics::GetContext3D() const {
    return m_d3dDeviceContext.Get();
}

ID2D1DeviceContext* OverlayGraphics::GetContext2D() const {
    return m_d2dDeviceContext.Get();
}

IDXGISwapChain1* OverlayGraphics::GetDxgiSwapChain() const {
    return m_dxgiSwapChain.Get();
}

IDXGISurface2* OverlayGraphics::GetDxgiTargetSurface() const {
    return m_dxgiTargetSurface.Get();
}

IDCompositionTarget* OverlayGraphics::GetDcompTarget() const {
    return m_dcompTarget.Get();
}

IDCompositionVisual* OverlayGraphics::GetDcompVisual() const {
    return m_dcompVisual.Get();
}

ID2D1Bitmap1* OverlayGraphics::GetTargetBitmap2D() const {
    return m_d2dTargetBitmap.Get();
}

bool OverlayGraphics::CreateDeviceResources() {
    HRESULT status = S_OK;

    if (SUCCEEDED(status)) {
        constexpr D3D_FEATURE_LEVEL features[] = {
            D3D_FEATURE_LEVEL_11_1,
            D3D_FEATURE_LEVEL_11_0,
        };

        status = D3D11CreateDevice(
            nullptr,
            D3D_DRIVER_TYPE_HARDWARE,
            nullptr,
            D3D11_CREATE_DEVICE_SINGLETHREADED | D3D11_CREATE_DEVICE_BGRA_SUPPORT,
            features,
            sizeof features / sizeof features[0],
            D3D11_SDK_VERSION,
            m_d3dDevice.GetAddressOf(),
            nullptr,
            m_d3dDeviceContext.GetAddressOf()
        );
    }
    if (SUCCEEDED(status)) {
        status = m_d3dDevice.As(&m_dxgiDevice);
    }
    if (SUCCEEDED(status)) {
        DXGI_SWAP_CHAIN_DESC1 description = { };
        description.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
        description.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
        description.SwapEffect = DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL;
        description.BufferCount = 2;
        description.SampleDesc.Count = 1;
        description.AlphaMode = DXGI_ALPHA_MODE_PREMULTIPLIED;

        RECT clientRect = { };
        if (FALSE == GetClientRect(m_hWnd, &clientRect)) {
            return false;
        }

        description.Width = clientRect.right - clientRect.left;
        description.Height = clientRect.bottom - clientRect.top;
        status = m_dxgiFactory->CreateSwapChainForComposition(m_d3dDevice.Get(), &description, nullptr, m_dxgiSwapChain.GetAddressOf());
    }
    if (SUCCEEDED(status)) {
        status = m_dxgiSwapChain->GetBuffer(0, IID_PPV_ARGS(m_dxgiTargetSurface.GetAddressOf()));
    }
    if (SUCCEEDED(status)) {
        status = m_d2dFactory->CreateDevice(m_dxgiDevice.Get(), m_d2dDevice.GetAddressOf());
    }
    if (SUCCEEDED(status)) {
        status = m_d2dDevice->CreateDeviceContext(D2D1_DEVICE_CONTEXT_OPTIONS_NONE, m_d2dDeviceContext.GetAddressOf());
    }
    if (SUCCEEDED(status)) {
        D2D1_BITMAP_PROPERTIES1 bitmapProperties = { };
        bitmapProperties.pixelFormat.alphaMode = D2D1_ALPHA_MODE_PREMULTIPLIED;
        bitmapProperties.pixelFormat.format = DXGI_FORMAT_B8G8R8A8_UNORM;
        bitmapProperties.bitmapOptions = D2D1_BITMAP_OPTIONS_TARGET | D2D1_BITMAP_OPTIONS_CANNOT_DRAW;

        status = m_d2dDeviceContext->CreateBitmapFromDxgiSurface(m_dxgiTargetSurface.Get(), bitmapProperties, m_d2dTargetBitmap.GetAddressOf());
    }
    if (SUCCEEDED(status)) {
        m_d2dDeviceContext->SetTarget(m_d2dTargetBitmap.Get());
    }
    if (SUCCEEDED(status)) {
        status = DCompositionCreateDevice(m_dxgiDevice.Get(), IID_PPV_ARGS(m_dcompDevice.GetAddressOf()));
    }
    if (SUCCEEDED(status)) {
        status = m_dcompDevice->CreateTargetForHwnd(m_hWnd, TRUE, m_dcompTarget.GetAddressOf());
    }
    if (SUCCEEDED(status)) {
        status = m_dcompDevice->CreateVisual(m_dcompVisual.GetAddressOf());
    }
    if (SUCCEEDED(status)) {
        status = m_dcompVisual->SetContent(m_dxgiSwapChain.Get());
    }
    if (SUCCEEDED(status)) {
        status = m_dcompTarget->SetRoot(m_dcompVisual.Get());
    }
    if (SUCCEEDED(status)) {
        status = m_dcompDevice->Commit();
    }

    return SUCCEEDED(status);
}

bool OverlayGraphics::CreateDeviceIndependentResources() {
    HRESULT status = S_OK;

    if (SUCCEEDED(status)) {
        status = CreateDXGIFactory(IID_PPV_ARGS(m_dxgiFactory.GetAddressOf()));
    }
    if (SUCCEEDED(status)) {
        status = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, m_d2dFactory.GetAddressOf());
    }

    return SUCCEEDED(status);
}

void OverlayGraphics::DiscardDeviceResources() {
    m_dcompVisual.Reset();
    m_dcompTarget.Reset();
    m_dcompDevice.Reset();

    m_d2dTargetBitmap.Reset();
    m_d2dDeviceContext.Reset();
    m_d2dDevice.Reset();

    m_dxgiTargetSurface.Reset();
    m_dxgiSwapChain.Reset();
    m_dxgiDevice.Reset();

    m_d3dDeviceContext.Reset();
    m_d3dDevice.Reset();
}

void OverlayGraphics::DiscardDeviceIndependentResources() {
    m_d2dFactory.Reset();
    m_dxgiFactory.Reset();
}
