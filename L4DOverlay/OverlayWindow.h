#pragma once

#include <Windows.h>
#include <combaseapi.h>
#include <wrl/client.h>
// DX
#include <dxgi1_2.h>
#include <d3d11.h>
#include <d2d1.h>
#include <d2d1helper.h>
#include <d2d1_1.h>
#include <d2d1_1helper.h>
#include <d2d1effects.h>
#include <d2d1effects_2.h>
#include <dcomp.h>
#include <memory>
#include <wincodec.h>

#include "GlareComponent.h"
#include "TransformStack.h"

// Last

#include <WinSock2.h>
#include <atomic>
#include <future>
#include <vector>
#include <CommonLib/ScopedHandle.h>
#include <NetworkLib/Utils.h>
#include "A2SInfoPacket.h"
#include "A2SPlayerPacket.h"
#include "S2CChallengePacket.h"
#include "S2CInfoPacket.h"
#include "S2CPlayerPacket.h"

#include "EasingFunctions.h"

// Component model
#include "AsyncSsqLoader.h"
#include "IGuiComponent.h"

using Microsoft::WRL::ComPtr; // TODO remove

enum class OverlayState {
    HIDDEN,
	SHOWING,
	SHOWN,
	HIDING,
};

class OverlayWindow final : IGuiComponent {
public:
	OverlayWindow();
	~OverlayWindow() override;

    bool Initialize() override;
	void Update() override;
	void Render() override;

	bool RunMainLoop();

private:
	// Window
	HWND m_hWnd;
	MONITORINFO m_monitorInfo;
	const wchar_t* const kClassName = L"SourceOverlayClass";

	// Device-dependent
	ComPtr<ID3D11Device> m_d3dDevice;
	ComPtr<ID3D11DeviceContext> m_d3dDeviceContext;

	ComPtr<IDXGIDevice> m_dxgiDevice;
	ComPtr<IDXGISwapChain1> m_dxgiSwapChain;
	ComPtr<IDXGISurface2> m_dxgiTargetSurface;

	ComPtr<IDCompositionDevice> m_dcompDevice;
	ComPtr<IDCompositionTarget> m_dcompTarget;
	ComPtr<IDCompositionVisual> m_dcompVisual;

	ComPtr<ID2D1Device> m_d2dDevice;
	ComPtr<ID2D1DeviceContext> m_d2dDeviceContext;
	ComPtr<ID2D1Bitmap1> m_d2dTargetBitmap;

	// Device-independent
	ComPtr<IDXGIFactory2> m_dxgiFactory;
	ComPtr<ID2D1Factory1> m_d2dFactory;
	ComPtr<IWICImagingFactory> m_wicFactory;

	// Rendering
	ComPtr<ID2D1Bitmap> m_d2dMapBitmap;

	// Logic
	OverlayState m_overlayState = OverlayState::HIDDEN;

	double m_showingProgress = 0.0; // [0; 1]
	double m_showingTime = 0.0;
	double m_showingDuration = 150;

	std::chrono::time_point<std::chrono::high_resolution_clock> m_lastTime;
	std::chrono::time_point<std::chrono::high_resolution_clock> m_currentTime;
	double m_elapsedTime;

	// Networking
	AsyncSsqLoader m_dataLoader;

	// Resource methods
    bool CreateDeviceResources();
    bool CreateDeviceIndependentResources();
    void DiscardDeviceResources();
	void DiscardDeviceIndependentResources();
    // Internal
    bool CreateGraphicsFactories();
    bool CreateDeviceComponents();
    bool CreateWindowResources();
    bool CreateGraphicsResources();
    void DiscardGraphicsResources();
    void DiscardDeviceComponents();

    static LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
};
