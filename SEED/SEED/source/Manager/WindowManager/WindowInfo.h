#pragma once
// windows
#include <windows.h>

// DirectX
#include <d3dx12.h>
#include <d3d12.h>
#include <dxgi1_6.h>
#include <wrl/client.h>
using Microsoft::WRL::ComPtr;

// lib
#include <string>

// local
#include <SEED/Lib/Tensor/Vector2.h>


class WindowInfo{
public:
    WindowInfo() = default;
    ~WindowInfo();
    WindowInfo(HWND windowHandle, std::wstring windowName);
    void Update();
    void Finalize();

public:
    void CreateSwapChain(IDXGIFactory4* dxgiFactory, ID3D12CommandQueue* commandQueue);
    void Present(UINT syncInterval, UINT flags);

public:
    HWND GetWindowHandle()const{return windowHandle;}
    Vector2 GetOriginalWindowSize()const{ return originalWindowSize; }
    Vector2 GetCurrentWindowSize()const{ return currentWindowSize; }
    Vector2 GetWindowScale()const{ return windowScale; }
    ID3D12Resource* GetBackBuffer()const{ return swapChainResources[backBufferIndex].Get(); }
    const D3D12_CPU_DESCRIPTOR_HANDLE& GetRtvHandle()const{ return doubleBufferRtvHandles[backBufferIndex]; }

private:
    HWND windowHandle;
    std::wstring windowName;
    Vector2 originalWindowSize;
    Vector2 currentWindowSize;
    Vector2 windowScale;

    // swapChainの情報
    DXGI_SWAP_CHAIN_DESC1 swapChainDesc{};
    ComPtr<IDXGISwapChain4> swapChain = nullptr;
    ComPtr<ID3D12Resource> swapChainResources[2] = { nullptr };
    uint32_t backBufferIndex;
    // RTV用
    D3D12_CPU_DESCRIPTOR_HANDLE doubleBufferRtvHandles[2]{};
};