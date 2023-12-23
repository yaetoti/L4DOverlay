#define WIN32_LEAN_AND_MEAN

#include <Windows.h>
#include <ShellScalingApi.h>
#include <ConsoleLib/Console.h>

#include "OverlayWindow.h"
#include "NetworkLib/WsaDataWrapper.h"

int WINAPI wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ PWSTR pCmdLine, _In_ int nCmdShow) {
    HeapSetInformation(nullptr, HeapEnableTerminationOnCorruption, nullptr, 0);
    SetProcessDpiAwareness(PROCESS_PER_MONITOR_DPI_AWARE);

    if (FAILED(CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED))) {
        Console::GetInstance()->WPrintF(L"[ERROR]: CoInitializeEx() failed\n");
        return 0;
    }

    WsaDataWrapper wsaSession(2, 2);
    int result = 0;
    {
        auto overlay = std::make_unique<OverlayWindow>();
        result = overlay->Run();
    }

    CoUninitialize();
    return result;
}

// Algorithm:
// - Start COM, DirectComposition, Direct3D, Direct2D
// - Run program in background
// - If user press [TAB] -> Make async request to the server
// - Calculate position and sizes
// - Start sliding animation. While request isn't done - display progress bar

//using Microsoft::WRL::ComPtr;
//
//const wchar_t* const kClassName = L"SourceOverlayClass";
//
//LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
//    switch (message) {
//    case WM_NCHITTEST:
//        return HTCAPTION;
//    case WM_CLOSE:
//        // Perform any necessary cleanup or confirmation here
//        // If you want to proceed with window destruction, call DestroyWindow
//        DestroyWindow(hWnd);
//        break;
//    case WM_DESTROY:
//        // Release resources and do any cleanup
//        PostQuitMessage(0); // This will post WM_QUIT to the message queue
//        break;
//    case WM_PAINT:
//        // No rendering directly in WndProc; instead, schedule a redraw.
//        InvalidateRect(hWnd, nullptr, FALSE);
//        break;
//    default:
//        return DefWindowProc(hWnd, message, wParam, lParam);
//    }
//
//    return 0;
//}
//
//int WINAPI wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ PWSTR pCmdLine, _In_ int nCmdShow) {
//    CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);
//    SetProcessDpiAwareness(PROCESS_DPI_UNAWARE);
//
//    WNDCLASSEX wc = { 0 };
//    wc.cbSize = sizeof(WNDCLASSEX);
//    wc.style = CS_HREDRAW | CS_VREDRAW;
//    wc.lpfnWndProc = WndProc;
//    wc.cbClsExtra = 0;
//    wc.cbWndExtra = 0;
//    wc.hInstance = hInstance;
//    wc.hIcon = LoadIconW(hInstance, IDI_APPLICATION);
//    wc.hCursor = LoadCursorW(nullptr, IDC_ARROW);
//    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
//    wc.lpszMenuName = nullptr;
//    wc.lpszClassName = kClassName;
//    wc.hIconSm = LoadIconW(wc.hInstance, IDI_APPLICATION);
//    if (0 == RegisterClassExW(&wc)) {
//        MessageBoxW(nullptr, L"Window Registration Failed!", L"Error", MB_ICONEXCLAMATION | MB_OK);
//        return 0;
//    }
//
//    // Get monitor from cursor's position
//    // TODO Don't close a window but move it and recalculate sizes
//    // TODO Support different DPI
//    POINT cursorPos;
//    GetCursorPos(&cursorPos);
//    HMONITOR hMonitor = MonitorFromPoint(cursorPos, MONITOR_DEFAULTTONEAREST);
//    MONITORINFO monitorInfo = { sizeof(monitorInfo) };
//    GetMonitorInfoW(hMonitor, &monitorInfo);
//
//    HWND hWnd = CreateWindowExW(
//        WS_EX_LAYERED | WS_EX_TOPMOST | WS_EX_NOACTIVATE | WS_EX_APPWINDOW | WS_EX_NOREDIRECTIONBITMAP, // WS_EX_LAYERED WS_EX_NOREDIRECTIONBITMAP
//        kClassName,
//        L"Source Overlay",
//        WS_POPUP,
//        monitorInfo.rcWork.left, monitorInfo.rcWork.top, 160, 160,
//        nullptr, nullptr, hInstance, nullptr
//    );
//    if (nullptr == hWnd) {
//        MessageBoxW(nullptr, L"Window Creation Failed!", L"Error", MB_ICONEXCLAMATION | MB_OK);
//        return 0;
//    }
//
//    // TODO: DPI scaling
//    // DPI scaling
//    UINT dpi = GetDpiForWindow(hWnd);
//    Console::GetInstance()->WPrintF(L"%d\n", dpi);
//
//    // DX initialization
//    HRESULT status;
//
//    ComPtr<IDXGIFactory2> dxgiFactory;
//    status = CreateDXGIFactory(IID_PPV_ARGS(dxgiFactory.GetAddressOf()));
//
//    D3D_FEATURE_LEVEL d3dFeatures[] = {
//        D3D_FEATURE_LEVEL_11_1,
//        D3D_FEATURE_LEVEL_11_0,
//    };
//
//    ComPtr<IDXGIAdapter> dxgiAdapter;
//    for (int i = 0; ; ++i) {
//        status = dxgiFactory->EnumAdapters(i, dxgiAdapter.GetAddressOf());
//        if (DXGI_ERROR_NOT_FOUND == status) {
//            // TODO: ERROR No device that support featute level
//            break;
//        }
//
//        if (SUCCEEDED(D3D11CreateDevice(
//            dxgiAdapter.Get(),
//            D3D_DRIVER_TYPE_HARDWARE,
//            nullptr,
//            D3D11_CREATE_DEVICE_SINGLETHREADED | D3D11_CREATE_DEVICE_BGRA_SUPPORT,
//            d3dFeatures,
//            static_cast<UINT>(std::size(d3dFeatures)),
//            D3D11_SDK_VERSION,
//            nullptr,
//            nullptr,
//            nullptr
//        ))) {
//            break;
//        }
//
//        dxgiAdapter.Reset();
//    }
//
//    ComPtr<ID3D11Device> d3dDevice;
//    status = D3D11CreateDevice(
//        dxgiAdapter.Get(),
//        D3D_DRIVER_TYPE_HARDWARE,
//        nullptr,
//        D3D11_CREATE_DEVICE_SINGLETHREADED | D3D11_CREATE_DEVICE_BGRA_SUPPORT,
//        d3dFeatures,
//        static_cast<UINT>(std::size(d3dFeatures)),
//        D3D11_SDK_VERSION,
//        d3dDevice.GetAddressOf(),
//        nullptr,
//        nullptr
//    );
//
//    ComPtr<IDXGIDevice> dxgiDevice;
//    status = d3dDevice.As(&dxgiDevice);
//
//    DXGI_SWAP_CHAIN_DESC1 description = { 0 };
//    description.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
//    description.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
//    description.SwapEffect = DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL;
//    description.BufferCount = 2;
//    description.SampleDesc.Count = 1;
//    description.AlphaMode = DXGI_ALPHA_MODE_PREMULTIPLIED;
//
//    RECT clientRect;
//    GetClientRect(hWnd, &clientRect);
//    description.Width = clientRect.right - clientRect.left;
//    description.Height = clientRect.bottom - clientRect.top;
//
//    ComPtr<IDXGISwapChain1> dxgiSwapChain;
//    dxgiFactory->CreateSwapChainForComposition(d3dDevice.Get(), &description, nullptr, dxgiSwapChain.GetAddressOf());
//
//    ComPtr<IDXGISurface2> dxgiSurface;
//    dxgiSwapChain->GetBuffer(0, IID_PPV_ARGS(dxgiSurface.GetAddressOf()));
//
//    ComPtr<ID2D1Factory1> d2dFactory;
//    status = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, d2dFactory.GetAddressOf());
//
//    ComPtr<ID2D1Device> d2dDevice;
//    status = d2dFactory->CreateDevice(dxgiDevice.Get(), d2dDevice.GetAddressOf());
//
//    ComPtr<ID2D1DeviceContext> d2dDeviceContext;
//    d2dDevice->CreateDeviceContext(D2D1_DEVICE_CONTEXT_OPTIONS_NONE, d2dDeviceContext.GetAddressOf());
//
//    D2D1_BITMAP_PROPERTIES1 bitmapProperties = { };
//    bitmapProperties.pixelFormat.alphaMode = D2D1_ALPHA_MODE_PREMULTIPLIED;
//    bitmapProperties.pixelFormat.format = DXGI_FORMAT_B8G8R8A8_UNORM;
//    bitmapProperties.bitmapOptions = D2D1_BITMAP_OPTIONS_TARGET | D2D1_BITMAP_OPTIONS_CANNOT_DRAW;
//
//    ComPtr<ID2D1Bitmap1> d2dBitmap;
//    d2dDeviceContext->CreateBitmapFromDxgiSurface(dxgiSurface.Get(), bitmapProperties, d2dBitmap.GetAddressOf());
//    d2dDeviceContext->SetTarget(d2dBitmap.Get());
//
//    ComPtr<IDCompositionDevice> dcompDevice;
//    DCompositionCreateDevice(dxgiDevice.Get(), IID_PPV_ARGS(dcompDevice.GetAddressOf()));
//
//    ComPtr<IDCompositionTarget> dcompTarget;
//    dcompDevice->CreateTargetForHwnd(hWnd, TRUE, dcompTarget.GetAddressOf());
//
//    ComPtr<IDCompositionVisual> dcompVisual;
//    dcompDevice->CreateVisual(dcompVisual.GetAddressOf());
//
//    dcompVisual->SetContent(dxgiSwapChain.Get());
//    dcompTarget->SetRoot(dcompVisual.Get());
//    dcompDevice->Commit();
//
//    // DX End
//
//    ShowWindow(hWnd, nCmdShow);
//    UpdateWindow(hWnd);
//
//    // Message loop
//    MSG msg;
//    bool isRunning = true;
//    while (isRunning) {
//        while (PeekMessageW(&msg, nullptr, 0, 0, PM_REMOVE)) {
//            if (msg.message == WM_QUIT) {
//                isRunning = false;
//            }
//
//            TranslateMessage(&msg);
//            DispatchMessageW(&msg);
//        }
//
//        d2dDeviceContext->BeginDraw();
//        d2dDeviceContext->Clear(D2D1::ColorF(0.0f, 0.0f, 0.0f, 0.0f));
//        ComPtr<ID2D1SolidColorBrush> brush;
//        d2dDeviceContext->CreateSolidColorBrush(D2D1::ColorF(0.1f, 0.5f, 0.3f, 0.5f), brush.GetAddressOf());
//        d2dDeviceContext->FillEllipse(D2D1::Ellipse(D2D1::Point2F(80, 80), 80, 80), brush.Get());
//        d2dDeviceContext->EndDraw();
//        dxgiSwapChain->Present(1, 0);
//    }
//
//    CoUninitialize();
//    return static_cast<int>(msg.wParam);
//}




/*
#define WIN32_LEAN_AND_MEAN

#include <ConsoleLib/Console.h>
#include <NetworkLib/WsaDataWrapper.h>
#include <CommonLib/ByteBuffer.h>
#include <CommonLib/VecUtils.h>
#include <Windows.h>

#include "UdpClient.h"
#include "Protocol.h"
#include "Packets.h"

// https://www.gs4u.net/en/s/236165.html BCM COOP Ukrainian 31.43.157.18:1302 185.135.80.185:2356
int WINAPI wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ PWSTR pCmdLine, _In_ int nCmdShow) {
    WsaDataWrapper wsa(2, 2);
    // UdpClient client("46.174.48.20", 27209u);
    UdpClient client("46.174.48.86", 27015u);
    std::vector<uint8_t> request;
    ByteBuffer buffer(request);

    // Send info request
    buffer.Put(HeaderType::HEADER_SIMPLE);
    buffer.Put(SsqPacketType::A2S_INFO);
    A2SInfoPacket<uint8_t>().Serialize(buffer);
    client.Send((const char*)request.data(), (int)request.size());
    buffer.Clear();

    // Parse info response
    std::vector<uint8_t> response = client.Receive();
    ByteBuffer responseBuffer(response);
    HeaderType responseHeader;
    SsqPacketType responsePacket;
    responseBuffer.Get(responseHeader);
    responseBuffer.Get(responsePacket);
    if (responseHeader != HeaderType::HEADER_SIMPLE || responsePacket != SsqPacketType::S2C_INFO) {
        Console::GetInstance()->WPrintF(L"Response not expected\n");
        Console::GetInstance()->Pause();
        return 0;
    }
    S2CInfoPacket<uint8_t> infoResponse;
    infoResponse.Deserialize(responseBuffer);
    Console::GetInstance()->PrintF("Server Info:\n");
    infoResponse.Print();
    Console::GetInstance()->PrintF("\n");

    // Send player request
    buffer.Put(HeaderType::HEADER_SIMPLE);
    buffer.Put(SsqPacketType::A2S_PLAYER);
    A2SPlayerPacket<uint8_t>().Serialize(buffer);
    client.Send((const char*)request.data(), (int)request.size());
    buffer.Clear();

    // Parse player (challenge) response
    response = client.Receive();
    responseBuffer.Reset();
    responseBuffer.Get(responseHeader);
    responseBuffer.Get(responsePacket);
    if (responseHeader != HeaderType::HEADER_SIMPLE || responsePacket != SsqPacketType::S2C_CHALLENGE) {
        Console::GetInstance()->WPrintF(L"Response not expected\n");
        Console::GetInstance()->Pause();
        return 0;
    }
    S2CChallengePacket<uint8_t> chellengeResponse;
    chellengeResponse.Deserialize(responseBuffer);
    Console::GetInstance()->WPrintF(L"Challenge = %u\n", chellengeResponse.m_challenge);

    // Send player request
    buffer.Put(HeaderType::HEADER_SIMPLE);
    buffer.Put(SsqPacketType::A2S_PLAYER);
    A2SPlayerPacket<uint8_t>(chellengeResponse.m_challenge).Serialize(buffer);
    client.Send((const char*)request.data(), (int)request.size());
    buffer.Clear();

    // Parse player (challenge) response
    response = client.Receive();
    responseBuffer.Reset();
    responseBuffer.Get(responseHeader);
    responseBuffer.Get(responsePacket);
    S2CPlayerPacket<uint8_t> playerResponse(responseBuffer);
    Console::GetInstance()->PrintF("Players Info:\n");
    playerResponse.Print();

	Console::GetInstance()->WPrintF(L"Press any key to exit...\n");
	Console::GetInstance()->Pause();
	return 0;
}
*/