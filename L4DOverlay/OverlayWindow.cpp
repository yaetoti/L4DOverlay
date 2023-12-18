#include "OverlayWindow.h"

#include <ConsoleLib/Console.h>

OverlayWindow::OverlayWindow() {

}

OverlayWindow::~OverlayWindow() {

}

HRESULT OverlayWindow::CreateDeviceResources() {
    HRESULT status = S_OK;

    // Window resources
    if (SUCCEEDED(status)) {
        ComPtr<IDXGIAdapter1> dxgiAdapter;
        D3D_FEATURE_LEVEL d3dFeatures[] = {
            D3D_FEATURE_LEVEL_11_1,
            D3D_FEATURE_LEVEL_11_0,
        };

        status = FindAdapterWithFeatureLevel(m_dxgiFactory, d3dFeatures, static_cast<int>(std::size(d3dFeatures)), dxgiAdapter);
        Console::GetInstance()->WPrintF(L"0x%lXL\n", status);
        if (SUCCEEDED(status)) {
            status = D3D11CreateDevice(
                dxgiAdapter.Get(),
                D3D_DRIVER_TYPE_HARDWARE,
                nullptr,
                D3D11_CREATE_DEVICE_SINGLETHREADED | D3D11_CREATE_DEVICE_BGRA_SUPPORT,
                d3dFeatures,
                static_cast<int>(std::size(d3dFeatures)),
                D3D11_SDK_VERSION,
                m_d3dDevice.GetAddressOf(),
                nullptr,
                nullptr
            );
        }
    }
    if (SUCCEEDED(status)) {
        status = m_d3dDevice.As(&m_dxgiDevice);
    }
    if (SUCCEEDED(status)) {
        DXGI_SWAP_CHAIN_DESC1 description = { 0 };
        description.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
        description.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
        description.SwapEffect = DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL;
        description.BufferCount = 2;
        description.SampleDesc.Count = 1;
        description.AlphaMode = DXGI_ALPHA_MODE_PREMULTIPLIED;

        RECT clientRect;
        if (FALSE == GetClientRect(m_hWnd, &clientRect)) {
            status = E_FAIL;
        }
        else {
            description.Width = clientRect.right - clientRect.left;
            description.Height = clientRect.bottom - clientRect.top;
            status = m_dxgiFactory->CreateSwapChainForComposition(m_d3dDevice.Get(), &description, nullptr, m_dxgiSwapChain.GetAddressOf());
        }
    }
    Console::GetInstance()->WPrintF(L"0x%lXL\n", status);
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
    // End Window resources
    return status;
}

HRESULT OverlayWindow::CreateDeviceIndependentResources() {
    HRESULT status = S_OK;

    // Window resources
    if (SUCCEEDED(status)) {
        status = CreateDXGIFactory(IID_PPV_ARGS(m_dxgiFactory.GetAddressOf()));
    }
    if (SUCCEEDED(status)) {
        status = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, m_d2dFactory.GetAddressOf());
    }
    // End Window resources

    return status;
}

void OverlayWindow::DiscardDeviceResources() {

}

void OverlayWindow::DiscardDeviceIndependentResources() {
    m_dxgiFactory.Reset();
    m_d2dFactory.Reset();
}

int OverlayWindow::Run() {
	// Initialize resources

    WNDCLASSEX wc = { 0 };
    wc.cbSize = sizeof(WNDCLASSEX);
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = WndProc;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hInstance = GetModuleHandleW(nullptr);
    wc.hIcon = LoadIconW(GetModuleHandleW(nullptr), IDI_APPLICATION);
    wc.hCursor = LoadCursorW(nullptr, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wc.lpszMenuName = nullptr;
    wc.lpszClassName = kClassName;
    wc.hIconSm = LoadIconW(wc.hInstance, IDI_APPLICATION);
    if (0 == RegisterClassExW(&wc)) {
        // TODO error
        MessageBoxW(nullptr, L"Window Registration Failed!", L"Error", MB_ICONEXCLAMATION | MB_OK);
        return 0;
    }

    // TODO Don't close a window but move it and recalculate sizes
    POINT cursorPos;
    GetCursorPos(&cursorPos);
    HMONITOR hMonitor = MonitorFromPoint(cursorPos, MONITOR_DEFAULTTONEAREST);
    MONITORINFO monitorInfo = { sizeof(monitorInfo) };
    GetMonitorInfoW(hMonitor, &monitorInfo);

    HWND hWnd = CreateWindowExW(
        WS_EX_TRANSPARENT | WS_EX_LAYERED | WS_EX_TOPMOST | WS_EX_NOACTIVATE | WS_EX_APPWINDOW | WS_EX_NOREDIRECTIONBITMAP, // WS_EX_LAYERED WS_EX_NOREDIRECTIONBITMAP
        kClassName,
        L"Source Overlay",
        WS_POPUP,
        monitorInfo.rcWork.left, monitorInfo.rcWork.top, 160, 160,
        nullptr, nullptr, GetModuleHandleW(nullptr), nullptr
    );
    if (nullptr == hWnd) {
        // TODO error
        MessageBoxW(nullptr, L"Window Creation Failed!", L"Error", MB_ICONEXCLAMATION | MB_OK);
        return 0;
    }

    // TODO: DPI scaling
    // DPI scaling
    UINT dpi = GetDpiForWindow(hWnd);

    // DX initialization
    HRESULT status;

    status = CreateDeviceIndependentResources();
    if (FAILED(status)) {
        // TODO error
        MessageBoxW(nullptr, L"CreateDeviceIndependentResources() failed", L"Error", MB_ICONEXCLAMATION | MB_OK);
        return 0;
    }
    status = CreateDeviceResources();
    if (FAILED(status)) {
        // TODO error
        MessageBoxW(nullptr, L"CreateDeviceResources() failed", L"Error", MB_ICONEXCLAMATION | MB_OK);
        return 0;
    }

	// Main loop
    ShowWindow(hWnd, SW_SHOW);
    UpdateWindow(hWnd);

    MSG msg;
    while (true) {
        while (PeekMessageW(&msg, nullptr, 0, 0, PM_REMOVE)) {
            if (msg.message == WM_QUIT) {
                return static_cast<int>(msg.wParam);
            }

            TranslateMessage(&msg);
            DispatchMessageW(&msg);
        }

        m_d2dDeviceContext->BeginDraw();
        m_d2dDeviceContext->Clear(D2D1::ColorF(0.0f, 0.0f, 0.0f, 0.0f));
        ComPtr<ID2D1SolidColorBrush> brush;
        m_d2dDeviceContext->CreateSolidColorBrush(D2D1::ColorF(0.1f, 0.5f, 0.3f, 0.5f), brush.GetAddressOf());
        m_d2dDeviceContext->FillEllipse(D2D1::Ellipse(D2D1::Point2F(80, 80), 80, 80), brush.Get());
        m_d2dDeviceContext->EndDraw();
        m_dxgiSwapChain->Present(1, 0);
    }

	return 0;
}

void OverlayWindow::OnRender() {
}

void OverlayWindow::OnResize() {
}

LRESULT OverlayWindow::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
    switch (message) {
    case WM_NCHITTEST:
        return HTCAPTION;
    case WM_CLOSE:
        // Perform any necessary cleanup or confirmation here
        // If you want to proceed with window destruction, call DestroyWindow
        DestroyWindow(hWnd);
        break;
    case WM_DESTROY:
        // Release resources and do any cleanup
        PostQuitMessage(0); // This will post WM_QUIT to the message queue
        break;
    case WM_PAINT:
        // No rendering directly in WndProc; instead, schedule a redraw.
        // TODO: ValidateRect here
        InvalidateRect(hWnd, nullptr, FALSE);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }

    return 0;
}

HRESULT OverlayWindow::FindAdapterWithFeatureLevel(ComPtr<IDXGIFactory2> dxgiFactory, const D3D_FEATURE_LEVEL* d3dFeatures, int featuresCount, ComPtr<IDXGIAdapter1>& dxgiAdapter) {
    for (int i = 0; ; ++i) {
        HRESULT status = dxgiFactory->EnumAdapters1(i, dxgiAdapter.GetAddressOf());
        if (FAILED(status)) {
            return status;
        }

        status = D3D11CreateDevice(
            dxgiAdapter.Get(),
            D3D_DRIVER_TYPE_HARDWARE,
            nullptr,
            D3D11_CREATE_DEVICE_SINGLETHREADED | D3D11_CREATE_DEVICE_BGRA_SUPPORT,
            d3dFeatures,
            featuresCount,
            D3D11_SDK_VERSION,
            nullptr,
            nullptr,
            nullptr
        );
        if (SUCCEEDED(status)) {
            return status;
        }

        dxgiAdapter.Reset();
    }
}
