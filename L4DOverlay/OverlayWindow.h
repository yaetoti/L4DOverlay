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
#include "OverlayGraphics.h"

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

	std::unique_ptr<OverlayGraphics> m_graphics;

	// Device-independent
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
    void DiscardDeviceComponents();

    static LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
};
