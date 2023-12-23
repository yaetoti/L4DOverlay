#define WIN32_LEAN_AND_MEAN

#include "OverlayWindow.h"

#include <ConsoleLib/Console.h>
#include "GraphicsHelper.h"

#define _USE_MATH_DEFINES
#include <math.h>

#include "LowLevelInputHandler.h"

OverlayWindow::OverlayWindow() {

}

OverlayWindow::~OverlayWindow() {
    DiscardDeviceResources();
    DiscardDeviceIndependentResources();

    UnregisterClassW(kClassName, GetModuleHandleW(nullptr));
}

HRESULT OverlayWindow::CreateDeviceResources() {
    HRESULT status = S_OK;

    // Window resources
    if (SUCCEEDED(status)) {
        D3D_FEATURE_LEVEL d3dFeatures[] = {
            D3D_FEATURE_LEVEL_11_1,
            D3D_FEATURE_LEVEL_11_0,
        };

        if (SUCCEEDED(status)) {
            status = D3D11CreateDevice(
                nullptr,
                D3D_DRIVER_TYPE_HARDWARE,
                nullptr,
                D3D11_CREATE_DEVICE_SINGLETHREADED | D3D11_CREATE_DEVICE_BGRA_SUPPORT,
                d3dFeatures,
                std::size(d3dFeatures),
                D3D11_SDK_VERSION,
                m_d3dDevice.GetAddressOf(),
                nullptr,
                m_d3dDeviceContext.GetAddressOf()
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

        RECT clientRect = { };
        if (FALSE == GetClientRect(m_hWnd, &clientRect)) {
            status = GetLastError();
        }
        else {
            description.Width = clientRect.right - clientRect.left;
            description.Height = clientRect.bottom - clientRect.top;
            status = m_dxgiFactory->CreateSwapChainForComposition(m_d3dDevice.Get(), &description, nullptr, m_dxgiSwapChain.GetAddressOf());
        }
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
    // End Window resources
    // Resources
    if (SUCCEEDED(status)) {
        status = LoadBitmapFromFile(m_d2dDeviceContext.Get(), m_wicFactory.Get(), L"1.webp", 342, 165, m_d2dMapBitmap.GetAddressOf());
    }
    
    // End resources
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
    // WIC resources
    if (SUCCEEDED(status)) {
        status = CoCreateInstance(CLSID_WICImagingFactory, nullptr, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(m_wicFactory.GetAddressOf()));
    }

    // End WIC resources

    return status;
}

void OverlayWindow::DiscardDeviceResources() {
    m_d2dMapBitmap.Reset();

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

void OverlayWindow::DiscardDeviceIndependentResources() {
    m_wicFactory.Reset();
    m_d2dFactory.Reset();
    m_dxgiFactory.Reset();
}

int OverlayWindow::Run() {
	// Initialize resources
    WNDCLASSEX wc = { };
    wc.cbSize = sizeof(WNDCLASSEX);
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = WndProc;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hInstance = GetModuleHandleW(nullptr);
    wc.hIcon = LoadIconW(GetModuleHandleW(nullptr), IDI_APPLICATION);
    wc.hCursor = LoadCursorW(nullptr, IDC_ARROW);
    wc.hbrBackground = reinterpret_cast<HBRUSH>(COLOR_WINDOW + 1);
    wc.lpszMenuName = nullptr;
    wc.lpszClassName = kClassName;
    wc.hIconSm = LoadIconW(wc.hInstance, IDI_APPLICATION);
    if (0 == RegisterClassExW(&wc)) {
        MessageBoxW(nullptr, L"Window Registration Failed!", L"Error", MB_ICONEXCLAMATION | MB_OK);
        return 0;
    }

    // TODO Don't close a window but move it and recalculate sizes
    POINT cursorPos;
    GetCursorPos(&cursorPos);
    HMONITOR hMonitor = MonitorFromPoint(cursorPos, MONITOR_DEFAULTTONEAREST);
    m_monitorInfo = { sizeof(m_monitorInfo) };
    GetMonitorInfoW(hMonitor, &m_monitorInfo);

    m_hWnd = CreateWindowExW(
        WS_EX_TRANSPARENT | WS_EX_LAYERED | WS_EX_TOPMOST | WS_EX_NOACTIVATE | WS_EX_APPWINDOW | WS_EX_NOREDIRECTIONBITMAP, // WS_EX_LAYERED WS_EX_NOREDIRECTIONBITMAP
        kClassName,
        L"Source Overlay",
        WS_POPUP,
        m_monitorInfo.rcWork.left, m_monitorInfo.rcWork.top, m_monitorInfo.rcMonitor.right, m_monitorInfo.rcMonitor.bottom,
        nullptr, nullptr, GetModuleHandleW(nullptr), nullptr
    );
    if (nullptr == m_hWnd) {
        MessageBoxW(nullptr, L"Window Creation Failed!", L"Error", MB_ICONEXCLAMATION | MB_OK);
        return 0;
    }

    // DirectX initialization
    HRESULT status = CreateDeviceIndependentResources();
    if (FAILED(status)) {
        MessageBoxW(nullptr, L"CreateDeviceIndependentResources() failed", L"Error", MB_ICONEXCLAMATION | MB_OK);
        return 0;
    }
    status = CreateDeviceResources();
    if (FAILED(status)) {
        MessageBoxW(nullptr, L"CreateDeviceResources() failed", L"Error", MB_ICONEXCLAMATION | MB_OK);
        return 0;
    }

    ShowWindow(m_hWnd, SW_SHOW);
    UpdateWindow(m_hWnd);

    // Add input listeners
    LowLevelInputHandler::GetInstance()->AddKeyboardListener([this](const LowLevelKeyboardEvent& event) {
        if (event.GetInfo().vkCode != VK_TAB) {
            return;
        }

        if (OverlayState::HIDDEN == m_overlayState && WM_KEYDOWN == event.GetId()) {
            m_overlayState = OverlayState::SHOWING;
            return;
        }
        if (OverlayState::HIDDEN != m_overlayState && WM_KEYUP == event.GetId()) {
            m_overlayState = OverlayState::HIDING;
            return;
        }
    });
    LowLevelInputHandler::GetInstance()->AddKeyboardCancelListener([this](const LowLevelKeyboardEvent& event) {
        return VK_TAB == event.GetInfo().vkCode && (WM_KEYDOWN == event.GetId() || WM_KEYUP == event.GetId());
    });
    LowLevelInputHandler::GetInstance()->Start();

    // Main loop
    MSG message;
    while (true) {
        while (PeekMessageW(&message, nullptr, 0, 0, PM_REMOVE)) {
            if (message.message == WM_QUIT) {
                LowLevelInputHandler::GetInstance()->Stop();
                return static_cast<int>(message.wParam);
            }

            TranslateMessage(&message);
            DispatchMessageW(&message);
        }

        LowLevelInputHandler::GetInstance()->Dispatch();
        OnUpdate();
        OnRender();
    }
}

static bool SendPacket(SOCKET socket, const Packet<SsqPacketType>& packet, const sockaddr* sendAddress, int length) {
    std::vector<uint8_t> buffer;
    ByteBuffer wrapper(buffer);
    int sent = 0;

    wrapper.Put(HeaderType::HEADER_SIMPLE);
    wrapper.Put(packet.GetType());
    packet.Serialize(wrapper);
    do {
        int status = sendto(socket, std::bit_cast<const char*>(buffer.data() + sent), static_cast<int>(buffer.size()) - sent, 0, sendAddress, length);
        if (SOCKET_ERROR == sent) {
            return false;
        }
        sent += status;
    } while (sent < static_cast<int>(buffer.size()));
    wrapper.Clear();
    return true;
}

static int ReceiveSomeFrom(SOCKET socket, char* buffer, int length, const sockaddr& receiveAddress) {
    sockaddr address;
    int addressLength = sizeof(address);
    int status = recvfrom(socket, buffer, length, 0, &address, &addressLength);
    if (0 != std::memcmp(&address, &receiveAddress, addressLength)) {
        status = -2;
    }

    return status;
}

static std::unique_ptr<Packet<SsqPacketType>> ReceivePacket(SOCKET socket, const sockaddr& receiveAddress) {
    std::vector<uint8_t> data;
    HeaderType headerType;
    SsqPacketType packetType;

    while (true) {
        try {
            // Receive some data
            char buffer[4096];
            int status = ReceiveSomeFrom(socket, buffer, 4096, receiveAddress);
            if (SOCKET_ERROR == status) {
                return nullptr;
            }
            if (-2 == status) {
                // Skip data from unexpected address
                continue;
            }

            data.insert(data.end(), buffer, buffer + status);
            // Parse header
            ByteBuffer wrapper(data);
            wrapper.Get(headerType);
            if (HeaderType::HEADER_SIMPLE != headerType) {
                // Unsupported header
                return nullptr;
            }
            wrapper.Get(packetType);
            // Parse packet
            switch (packetType) {
            case SsqPacketType::S2C_CHALLENGE:
            {
                S2CChallengePacket packet;
                packet.Deserialize(wrapper);
                return std::make_unique<S2CChallengePacket>(packet);
            }
            case SsqPacketType::S2C_PLAYER:
            {
                S2CPlayerPacket packet;
                packet.Deserialize(wrapper);
                return std::make_unique<S2CPlayerPacket>(packet);
            }
            case SsqPacketType::S2C_INFO:
            {
                S2CInfoPacket packet;
                packet.Deserialize(wrapper);
                return std::make_unique<S2CInfoPacket>(packet);
            }
            default:
                // Unexpected answer
                return nullptr;
            }
        }
        catch(const std::out_of_range&) {
            // If not enough to parse - repeat
            continue;
        }
    }
}

void OverlayWindow::FetchData() {
    m_isConnecting = true;
    m_connectionFuture = std::async(std::launch::async, [this] {
        ScopedHandle<SOCKET> serverSocket(
            INVALID_SOCKET,
            socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP),
            closesocket
        );
        if (INVALID_SOCKET == serverSocket.Get()) {
            m_isConnectionError = true;
            m_isConnecting = false;
            return;
        }

        // Set timeout
        DWORD timeout = 5000;
        setsockopt(serverSocket.Get(), SOL_SOCKET, SO_SNDTIMEO, std::bit_cast<const char*>(&timeout), sizeof(timeout));
        setsockopt(serverSocket.Get(), SOL_SOCKET, SO_RCVTIMEO, std::bit_cast<const char*>(&timeout), sizeof(timeout));

        // TODO: Server IP
        ADDRINFOEXW serverAddress;
        if (!ResolveAddressEx(L"46.174.48.81", L"27015", { 5, 0 }, [&serverAddress](const ADDRINFOEXW* address) {
            serverAddress = *address;
            return true;
        })) {
            m_isConnectionError = true;
            m_isConnecting = false;
            return;
        }

        // Variables
        std::unique_ptr<Packet<SsqPacketType>> infoPacket;
        std::unique_ptr<Packet<SsqPacketType>> playersPacket;
        int challenge = -1;

        // Retrieving server info
        while (true) {
            if (!SendPacket(serverSocket.Get(), A2SInfoPacket(challenge), serverAddress.ai_addr, serverAddress.ai_addrlen)) {
                m_isConnectionError = true;
                m_isConnecting = false;
                return;
            }

            infoPacket = ReceivePacket(serverSocket.Get(), *serverAddress.ai_addr);
            if (!infoPacket) {
                m_isConnectionError = true;
                m_isConnecting = false;
                return;
            }

            if (SsqPacketType::S2C_CHALLENGE == infoPacket->GetType()) {
                challenge = reinterpret_cast<S2CChallengePacket*>(infoPacket.get())->GetChallenge();
                continue;
            }
            if (SsqPacketType::S2C_INFO != infoPacket->GetType()) {
                // Unexpected answer
                m_isConnectionError = true;
                m_isConnecting = false;
                return;
            }

            break;
        }
        // Retrieving players info
        while (true) {
            if (!SendPacket(serverSocket.Get(), A2SPlayerPacket(challenge), serverAddress.ai_addr, serverAddress.ai_addrlen)) {
                m_isConnectionError = true;
                m_isConnecting = false;
                return;
            }

            playersPacket = ReceivePacket(serverSocket.Get(), *serverAddress.ai_addr);
            if (!playersPacket) {
                m_isConnectionError = true;
                m_isConnecting = false;
                return;
            }

            if (SsqPacketType::S2C_CHALLENGE == playersPacket->GetType()) {
                challenge = reinterpret_cast<S2CChallengePacket*>(playersPacket.get())->GetChallenge();
                continue;
            }
            if (SsqPacketType::S2C_PLAYER != playersPacket->GetType()) {
                // Unexpected answer
                m_isConnectionError = true;
                m_isConnecting = false;
                return;
            }

            break;
        }
        // Packets received
        // TODO Ugly. Implement move semantic
        m_serverInfo = std::make_unique<S2CInfoPacket>(reinterpret_cast<S2CInfoPacket&>(*infoPacket));
        m_playerInfo = std::make_unique<S2CPlayerPacket>(reinterpret_cast<S2CPlayerPacket&>(*playersPacket));
        m_isDataValid = true;
        m_isConnecting = false;
    });
}

void OverlayWindow::OnUpdate() {
    if (OverlayState::HIDDEN != m_overlayState && !m_isDataValid && !m_isConnecting && !m_isConnectionError) {
        Console::GetInstance()->WPrintF(L"Loading data\n");
        FetchData();
    }

    if (m_isConnecting) {
        Console::GetInstance()->WPrintF(L"Connecting\n");
    }

    if (!m_isConnecting) {
        if (m_isConnectionError) {
            Console::GetInstance()->WPrintF(L"Error\n");
        }

        if (m_isDataValid) {
            Console::GetInstance()->WPrintF(L"Data valid\n");
        }
    }

    switch (m_overlayState) {
    case OverlayState::HIDDEN:
        break;
    case OverlayState::SHOWING:

        break;
    case OverlayState::SHOWN:
        break;
    case OverlayState::HIDING:
        break;
    }
}

void OverlayWindow::OnRender() {
    static float colorR = 0.0;
    static bool up = true;
    //if (up) {
    //    colorR += 0.001f;
    //    if (colorR >= 1.0f) {
    //        up = false;
    //        colorR = 1.0f;
    //    }
    //}
    //else {
    //    colorR -= 0.001f;
    //    if (colorR <= 0.0f) {
    //        up = true;
    //        colorR = 0.0f;
    //    }
    //}

    HRESULT status = S_OK;

    // Variables
    float monitorY = static_cast<float>(m_monitorInfo.rcWork.bottom);
    float overlayPaddingY = monitorY * 0.1f;

    float OverlayX = 0.0f;
    float OverlayY = overlayPaddingY;
    float OverlayWidth = 390.0f;
    float OverlayHeight = monitorY - 2 * overlayPaddingY;
    float OverlayDiagonal = sqrtf(OverlayWidth * OverlayWidth + OverlayHeight * OverlayHeight);
    
    float BaseGradientAngle = atan2f(OverlayWidth, OverlayHeight) * 180.0f / M_PI;
    float BaseGradientWidth = OverlayWidth / 1.8f;
    float BaseGradientHeight = OverlayHeight / 1.8f;
    
    float MapWidth = 342.0f;
    float MapHeight = 165.0f;
    float MapX = 24.0f;
    float MapY = 56.0;

    float MapFrameStroke = 4.0f;

    D2D1_RECT_F MapRect = D2D1::RectF(OverlayX + MapX, OverlayY + MapY, OverlayX + MapX + MapWidth, OverlayY + MapY + MapHeight);
    D2D1_RECT_F MapFrameRect = D2D1::RectF(
        OverlayX + MapX - MapFrameStroke * 2.7f,
        OverlayY + MapY - MapFrameStroke * 2.7f,
        OverlayX + MapX + MapWidth + MapFrameStroke * 2.7f,
        OverlayY + MapY + MapHeight + MapFrameStroke * 2.7f
    );

    // Gradients
    ComPtr<ID2D1GradientStopCollection> overlayBaseGradientStop;
    D2D1_GRADIENT_STOP overlayBaseGradientStops[] = {
        D2D1::GradientStop(0.0f, D2D1::ColorF(0x414141)),
        D2D1::GradientStop(0.2f, D2D1::ColorF(0x3D3D3D)),
        D2D1::GradientStop(1.0f, D2D1::ColorF(0x161616)),
    };
    m_d2dDeviceContext->CreateGradientStopCollection(overlayBaseGradientStops, 3, D2D1_GAMMA_2_2, D2D1_EXTEND_MODE_CLAMP, overlayBaseGradientStop.GetAddressOf());

    ComPtr<ID2D1GradientStopCollection> overlayStrokeGradientStop;
    D2D1_GRADIENT_STOP overlayStrokeGradientStops[] = {
        D2D1::GradientStop(0.0f, D2D1::ColorF(0x858585)),
        D2D1::GradientStop(1.0f, D2D1::ColorF(0x121212)),
    };
    m_d2dDeviceContext->CreateGradientStopCollection(overlayStrokeGradientStops, 2, D2D1_GAMMA_2_2, D2D1_EXTEND_MODE_CLAMP, overlayStrokeGradientStop.GetAddressOf());

    ComPtr<ID2D1GradientStopCollection> mapStrokeGradientStop;
    D2D1_GRADIENT_STOP mapStrokeGradientStops[] = {
        D2D1::GradientStop(0.0f, D2D1::ColorF(0x858585)),
        D2D1::GradientStop(0.5f, D2D1::ColorF(0x121212)),
        D2D1::GradientStop(1.0f, D2D1::ColorF(0x121212)),
    };
    m_d2dDeviceContext->CreateGradientStopCollection(mapStrokeGradientStops, 2, D2D1_GAMMA_2_2, D2D1_EXTEND_MODE_CLAMP, mapStrokeGradientStop.GetAddressOf());

    ComPtr<ID2D1GradientStopCollection> mapframeGradientStop;
    D2D1_GRADIENT_STOP mapframeGradientStops[] = {
        D2D1::GradientStop(0.1f, D2D1::ColorF(0x3E3E3E)),
        D2D1::GradientStop(0.3f, D2D1::ColorF(0x9C9C9C)),
        D2D1::GradientStop(0.5f, D2D1::ColorF(0xFFFFFF)),
        D2D1::GradientStop(0.7f, D2D1::ColorF(0x9C9C9C)),
        D2D1::GradientStop(0.9f, D2D1::ColorF(0x3E3E3E)),
    };
    m_d2dDeviceContext->CreateGradientStopCollection(mapframeGradientStops, 5, D2D1_GAMMA_2_2, D2D1_EXTEND_MODE_CLAMP, mapframeGradientStop.GetAddressOf());

    // Brushes
    ComPtr<ID2D1RadialGradientBrush> overlayBaseBrush;
    m_d2dDeviceContext->CreateRadialGradientBrush(
        D2D1::RadialGradientBrushProperties(D2D1::Point2F(), D2D1::Point2F(), BaseGradientWidth, BaseGradientHeight),
        overlayBaseGradientStop.Get(), overlayBaseBrush.GetAddressOf());
    overlayBaseBrush->SetTransform(
        D2D1::Matrix3x2F::Rotation(-BaseGradientAngle)
        * D2D1::Matrix3x2F::Scale(D2D1::SizeF(1.0f, OverlayDiagonal / OverlayHeight))
        * D2D1::Matrix3x2F::Translation(OverlayX + OverlayWidth / 2, OverlayY + OverlayHeight / 2));

    ComPtr<ID2D1RadialGradientBrush> overlayStrokeBrush;
    m_d2dDeviceContext->CreateRadialGradientBrush(
        D2D1::RadialGradientBrushProperties(D2D1::Point2F(), D2D1::Point2F(), BaseGradientWidth, BaseGradientHeight),
        overlayStrokeGradientStop.Get(), overlayStrokeBrush.GetAddressOf());
    overlayStrokeBrush->SetTransform(
        D2D1::Matrix3x2F::Rotation(-BaseGradientAngle)
        * D2D1::Matrix3x2F::Scale(D2D1::SizeF(1.0f, OverlayDiagonal / OverlayHeight))
        * D2D1::Matrix3x2F::Translation(OverlayX + OverlayWidth / 2, OverlayY + OverlayHeight / 2));

    ComPtr<ID2D1SolidColorBrush> mapStrokeBrush;
    m_d2dDeviceContext->CreateSolidColorBrush(D2D1::ColorF(0x5F5F5F), mapStrokeBrush.GetAddressOf());

    ComPtr<ID2D1LinearGradientBrush> mapFrameBrush;
    m_d2dDeviceContext->CreateLinearGradientBrush(
        D2D1::LinearGradientBrushProperties(D2D1::Point2F(MapFrameRect.left), D2D1::Point2F(MapFrameRect.right)),
        mapframeGradientStop.Get(),
        mapFrameBrush.GetAddressOf());

    // Geometries
    ComPtr<ID2D1PathGeometry> overlayBaseGeometry;
    CreateFillRoundedRectangleGeometryEx(m_d2dFactory.Get(),
        RoundedRectEx(D2D1::RectF(0.0f, overlayPaddingY, 390.0f - colorR * 394.0f, monitorY - overlayPaddingY),
            0.0f, 10.0f, 10.0f, 0.0f),
        overlayBaseGeometry.GetAddressOf());
    ComPtr<ID2D1PathGeometry> overlayStrokeGeometry;
    CreateFillRoundedRectangleGeometryEx(m_d2dFactory.Get(),
        RoundedRectEx(D2D1::RectF(0.0f, overlayPaddingY, 394.0f - colorR * 394.0f, monitorY - overlayPaddingY + 1.0f),
            0.0f, 10.0f, 10.0f, 0.0f),
        overlayStrokeGeometry.GetAddressOf());
    ComPtr<ID2D1PathGeometry> mapGeometry;
    CreateFillRoundedRectangleGeometryEx(m_d2dFactory.Get(),
        RoundedRectEx(MapRect, 0.0f, 0.0f, 0.0f, 0.0f),
        mapGeometry.GetAddressOf());

    ComPtr<ID2D1PathGeometry> frameGeometry;
    CreateFrameGeometry(m_d2dFactory.Get(), Frame(MapFrameRect, D2D1::Point2F(90.0f, 30.0f), D2D1::Point2F(MapFrameStroke * 4, MapFrameStroke * 4)), frameGeometry.GetAddressOf());

    // Overlay rendering preparation
    ComPtr<ID2D1BitmapRenderTarget> overlayRenderTarget;
    m_d2dDeviceContext->CreateCompatibleRenderTarget(overlayRenderTarget.GetAddressOf());
    ComPtr<ID2D1DeviceContext> overlayDeviceContext;
    overlayRenderTarget->QueryInterface(IID_PPV_ARGS(overlayDeviceContext.GetAddressOf()));
    ComPtr<ID2D1Bitmap> overlayBitmap;

    // Overlay rendering
    overlayRenderTarget->BeginDraw();
    overlayRenderTarget->Clear(D2D1::ColorF(0x000000, 0.0f));
    
    overlayRenderTarget->FillGeometry(overlayStrokeGeometry.Get(), overlayStrokeBrush.Get());
    overlayRenderTarget->FillGeometry(overlayBaseGeometry.Get(), overlayBaseBrush.Get());

    // Map stroke
    overlayDeviceContext->DrawRectangle(MapRect, mapStrokeBrush.Get(), 12.0f);

    // Map vignette effect
    ComPtr<ID2D1Effect> vignetteEffect;
    ComPtr<ID2D1Image> vignetteOutput;
    overlayDeviceContext->CreateEffect(CLSID_D2D1Vignette, &vignetteEffect);
    vignetteEffect->SetInput(0, m_d2dMapBitmap.Get());
    vignetteEffect->SetValue(D2D1_VIGNETTE_PROP_COLOR, D2D1::ColorF(0x000000, 1.0f));
    vignetteEffect->SetValue(D2D1_VIGNETTE_PROP_TRANSITION_SIZE, 40.0f);
    vignetteEffect->SetValue(D2D1_VIGNETTE_PROP_STRENGTH, 0.5f);
    vignetteEffect->GetOutput(vignetteOutput.GetAddressOf());

    // Map rendering
    ComPtr<ID2D1ImageBrush> mapImageBrush;
    overlayDeviceContext->CreateImageBrush(vignetteOutput.Get(), D2D1::ImageBrushProperties(D2D1::RectF(0, 0, MapWidth, MapHeight)), mapImageBrush.GetAddressOf());
    mapImageBrush->SetTransform(D2D1::Matrix3x2F::Translation(OverlayX + MapX, OverlayY + MapY));
    overlayDeviceContext->FillGeometry(mapGeometry.Get(), mapImageBrush.Get());

    // Map frame
    overlayDeviceContext->DrawGeometry(frameGeometry.Get(), mapFrameBrush.Get(), 4.0f);

    // Glare rendering
    GlareComponent glare1(overlayDeviceContext.Get(), 6.0f, 600.0f);
    GlareComponent glare2(overlayDeviceContext.Get(), 60.0f, 600.0f);
    GlareComponent glare3(overlayDeviceContext.Get(), 3.0f, 600.0f);
    glare1.Initialize();
    glare2.Initialize();
    glare3.Initialize();

    ComPtr<ID2D1Layer> layer;
    overlayDeviceContext->CreateLayer(nullptr, layer.GetAddressOf());
    overlayDeviceContext->PushLayer(D2D1::LayerParameters(D2D1::InfiniteRect(), overlayBaseGeometry.Get()), layer.Get());

    TransformStack stack(overlayDeviceContext.Get());
    stack.Rotate(-30.0f);
    stack.Translate(150.0f, 450.0f);
    glare1.Render();
    stack.Translate(160.0f, 0.0f);
    glare2.Render();
    stack.Translate(80.0f, 0.0f);
    glare3.Render();
    stack.Reset();

    overlayDeviceContext->PopLayer();

    // Get overlay bitmap
    overlayRenderTarget->EndDraw();
    overlayRenderTarget->GetBitmap(overlayBitmap.GetAddressOf());

    // Rendering to composition
    m_d2dDeviceContext->BeginDraw();
    m_d2dDeviceContext->Clear(D2D1::ColorF(0x000000, 0.0f));

    // Overlay shadow
    ComPtr<ID2D1Effect> shadowEffect;
    m_d2dDeviceContext->CreateEffect(CLSID_D2D1Shadow, shadowEffect.GetAddressOf());
    shadowEffect->SetValue(D2D1_SHADOW_PROP_BLUR_STANDARD_DEVIATION, 4.0f);
    shadowEffect->SetValue(D2D1_SHADOW_PROP_COLOR, D2D1::Vector4F(0.0f, 0.0f, 0.0f, 0.25f));
    shadowEffect->SetInput(0, overlayBitmap.Get());

    // Transform shadow
    ComPtr<ID2D1Effect> affineTransformEffect;
    m_d2dDeviceContext->CreateEffect(CLSID_D2D12DAffineTransform, &affineTransformEffect);
    affineTransformEffect->SetValue(D2D1_2DAFFINETRANSFORM_PROP_TRANSFORM_MATRIX, D2D1::Matrix3x2F::Translation(6.0f, 6.0f));
    affineTransformEffect->SetInputEffect(0, shadowEffect.Get());

    // Draw shadow and overlay
    m_d2dDeviceContext->DrawImage(affineTransformEffect.Get());
    m_d2dDeviceContext->DrawImage(overlayBitmap.Get());

    // Glare rendering
    // m_d2dDeviceContext->SetTransform(D2D1::Matrix3x2F::Rotation(0.0f) * D2D1::Matrix3x2F::Translation(0.0f, 0.0f));
    // m_d2dDeviceContext->SetTransform(D2D1::Matrix3x2F::Identity());

    // End
    status = m_d2dDeviceContext->EndDraw();
    if (FAILED(status)) {
        // TODO handle device hotswitch
        Console::GetInstance()->WPrintF(L"EndDraw() failed with status: 0x%lXL\n", status);
    }
    status = m_dxgiSwapChain->Present(1, 0);
    if (FAILED(status)) {
        // TODO handle device hotswitch
        Console::GetInstance()->WPrintF(L"Present() failed with status: 0x%lXL\n", status);
    }
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
