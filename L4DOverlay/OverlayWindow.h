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

using Microsoft::WRL::ComPtr; // TODO remove

//class Timer final {
//public:
//    Timer() = default;
//
//private:
//	
//};

enum class OverlayState {
    HIDDEN,
	SHOWING,
	SHOWN,
	HIDING,
};

class OverlayWindow final {
public:
	OverlayWindow();
	~OverlayWindow();

	int Run();

private:
	HWND m_hWnd;
	MONITORINFO m_monitorInfo;
	const wchar_t* const kClassName = L"SourceOverlayClass";

	// Device-dependant
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

	// Device-independant
	ComPtr<IDXGIFactory2> m_dxgiFactory;
	ComPtr<ID2D1Factory1> m_d2dFactory;
	ComPtr<IWICImagingFactory> m_wicFactory;

	// Rendering
	ComPtr<ID2D1Bitmap> m_d2dMapBitmap;

	// Logic
	OverlayState m_overlayState;
	std::unique_ptr<S2CInfoPacket> m_serverInfo;
	std::unique_ptr<S2CPlayerPacket> m_playerInfo;
	std::atomic<bool> m_isDataPresent;
	std::atomic<bool> m_isDataValid;
	std::atomic<bool> m_isConnecting;
	std::atomic<bool> m_isConnectionError;
	std::atomic<bool> m_isCancelled;
	std::future<void> m_connectionFuture;
	uint32_t m_networkTimeout = 5000;
	uint32_t m_cancelTimeout = 10;
	const std::wstring m_serverIp = L"46.174.48.86";
	const std::wstring m_serverPort = L"27015";
	static constexpr int m_receiveBufferSize = 4096;

	// Helper network functions
	bool ResolveServerAddress(const wchar_t* ip, const wchar_t* port, uint32_t connectTimeout, ADDRINFOEXW* serverAddress) const;
	int ReceiveSomeFrom(SOCKET socket, char* buffer, int length, const sockaddr& receiveAddress, int addressLength) const;
	bool SendAllTo(SOCKET socket, char* buffer, int length, const sockaddr& sendAddress, int addressLength) const;
	std::unique_ptr<Packet<SsqPacketType>> ReceivePacket(SOCKET socket, const sockaddr& receiveAddress, int addressLength) const;
	bool SendPacket(SOCKET socket, const Packet<SsqPacketType>& packet, const sockaddr& sendAddress, int addressLength) const;


	HRESULT CreateDeviceResources();
	HRESULT CreateDeviceIndependentResources();
	void DiscardDeviceResources();
	void DiscardDeviceIndependentResources();

	void OnUpdate();
	void OnRender();
	void OnResize();

	void FetchData();

	static LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
};
