#pragma once

#include <d3d11.h>
#include <dcomp.h>
#include <dxgi1_2.h>
#include <wrl/client.h>
#include "IGuiComponent.h"

struct OverlayGraphics final : IGuiComponent {
    explicit OverlayGraphics(HWND hWnd);
    ~OverlayGraphics() override;

    bool Initialize() override;
    void Render() override;
    void Update() override;

    IDXGIFactory2* GetFactoryDxgi() const;
    ID2D1Factory1* GetFactory2D() const;

    ID3D11Device* GetDevice3D() const;
    IDXGIDevice* GetDeviceDxgi() const;
    ID2D1Device* GetDevice2D() const;
    IDCompositionDevice* GetDeviceDcomp() const;

    ID3D11DeviceContext* GetContext3D() const;
    ID2D1DeviceContext* GetContext2D() const;

    IDXGISwapChain1* GetDxgiSwapChain() const;
    IDXGISurface2* GetDxgiTargetSurface() const;

    IDCompositionTarget* GetDcompTarget() const;
    IDCompositionVisual* GetDcompVisual() const;

    ID2D1Bitmap1* GetTargetBitmap2D() const;

private:
    HWND m_hWnd;

    // Device-independent
    Microsoft::WRL::ComPtr<IDXGIFactory2> m_dxgiFactory;
    Microsoft::WRL::ComPtr<ID2D1Factory1> m_d2dFactory;

	// Device-dependent
	Microsoft::WRL::ComPtr<ID3D11Device> m_d3dDevice;
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> m_d3dDeviceContext;

	Microsoft::WRL::ComPtr<IDXGIDevice> m_dxgiDevice;
	Microsoft::WRL::ComPtr<IDXGISwapChain1> m_dxgiSwapChain;
	Microsoft::WRL::ComPtr<IDXGISurface2> m_dxgiTargetSurface;

	Microsoft::WRL::ComPtr<IDCompositionDevice> m_dcompDevice;
	Microsoft::WRL::ComPtr<IDCompositionTarget> m_dcompTarget;
	Microsoft::WRL::ComPtr<IDCompositionVisual> m_dcompVisual;

	Microsoft::WRL::ComPtr<ID2D1Device> m_d2dDevice;
	Microsoft::WRL::ComPtr<ID2D1DeviceContext> m_d2dDeviceContext;
	Microsoft::WRL::ComPtr<ID2D1Bitmap1> m_d2dTargetBitmap;

    bool CreateDeviceIndependentResources();
    void DiscardDeviceIndependentResources();
    bool CreateDeviceResources();
    void DiscardDeviceResources();
};
