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
#include <dcomp.h>

#include <array>

using Microsoft::WRL::ComPtr; // TODO remove

class OverlayWindow final {
public:
	OverlayWindow();
	~OverlayWindow();

	int Run();

private:
	HWND m_hWnd;
	const wchar_t* const kClassName = L"SourceOverlayClass";

	// Device-dependant
	ComPtr<ID3D11Device> m_d3dDevice;

	ComPtr<IDXGIDevice> m_dxgiDevice;
	ComPtr<IDXGISwapChain1> m_dxgiSwapChain;
	ComPtr<IDXGISurface2> m_dxgiTargetSurface;

	ComPtr<ID2D1Device> m_d2dDevice;
	ComPtr<ID2D1DeviceContext> m_d2dDeviceContext;
	ComPtr<ID2D1Bitmap1> m_d2dTargetBitmap;

	ComPtr<IDCompositionDevice> m_dcompDevice;
	ComPtr<IDCompositionTarget> m_dcompTarget;
	ComPtr<IDCompositionVisual> m_dcompVisual;

	// Device-independant
	ComPtr<IDXGIFactory2> m_dxgiFactory;
	ComPtr<ID2D1Factory1> m_d2dFactory;


	HRESULT CreateDeviceResources();
	HRESULT CreateDeviceIndependentResources();
	void DiscardDeviceResources();
	void DiscardDeviceIndependentResources();

	void OnRender();
	void OnResize();


	static LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	static HRESULT FindAdapterWithFeatureLevel(ComPtr<IDXGIFactory2> dxgiFactory, const D3D_FEATURE_LEVEL* d3dFeatures, int featuresCount, ComPtr<IDXGIAdapter1>& dxgiAdapter);
};
