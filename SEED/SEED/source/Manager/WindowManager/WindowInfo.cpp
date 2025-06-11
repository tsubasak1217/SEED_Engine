#include "WindowInfo.h"
#include <SEED/Source/Manager/DxManager/ViewManager.h>
#include <SEED/Lib/Functions/MyFunc/DxFunc.h>

WindowInfo::~WindowInfo(){
    Finalize();
}

WindowInfo::WindowInfo(HWND windowHandle, std::wstring windowName){
    this->windowHandle = windowHandle;
    this->windowName = windowName;

    // ウインドウの初期サイズを取得-----------------------------------------------------
    RECT clientRect{};
    GetClientRect(windowHandle, &clientRect);
    originalWindowSize.x = (float)clientRect.right;
    originalWindowSize.y = (float)clientRect.bottom;
    currentWindowSize = originalWindowSize;
    windowScale = Vector2(1.0f, 1.0f);

    // swapChainDescの情報-----------------------------------------------------------
    
    // 画面の縦横幅
    swapChainDesc.Width = (int)originalWindowSize.x;
    swapChainDesc.Height = (int)originalWindowSize.y;
    // 色の形式
    swapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    // マルチサンプルしない
    swapChainDesc.SampleDesc.Count = 1;
    swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT; // 描画のターゲットとして利用する
    swapChainDesc.BufferCount = 2; // ダブルバッファ
    swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD; // モニタに写したら中身を破棄
}

void WindowInfo::Update(){

    // ウインドウのサイズスケールを更新
    RECT clientRect{};
    GetClientRect(windowHandle, &clientRect);
    currentWindowSize.x = (float)clientRect.right;
    currentWindowSize.y = (float)clientRect.bottom;

    windowScale.x = currentWindowSize.x / originalWindowSize.x;
    windowScale.y = currentWindowSize.y / originalWindowSize.y;
}

void WindowInfo::Finalize(){
    // swapChainの解放
    if(swapChainResources[0] != nullptr){
        swapChainResources[0].Reset();
    }
    if(swapChainResources[1] != nullptr){
        swapChainResources[1].Reset();
    }
    if(swapChain != nullptr){
        swapChain.Reset();
    }
}


// swapChainの作成
void WindowInfo::CreateSwapChain(IDXGIFactory4* dxgiFactory, ID3D12CommandQueue* commandQueue){

    // コマンドキュー、ウィンドウハンドル、設定を渡して生成する
    HRESULT hr;
    hr = dxgiFactory->CreateSwapChainForHwnd(
        commandQueue,
        windowHandle,
        &swapChainDesc,
        nullptr, nullptr,
        reinterpret_cast<IDXGISwapChain1**>(swapChain.GetAddressOf())
    );
    assert(SUCCEEDED(hr));


    // SwapChain から Resourceを引っ張ってくる
    hr = swapChain->GetBuffer(0, IID_PPV_ARGS(&swapChainResources[0]));
    assert(SUCCEEDED(hr));
    hr = swapChain->GetBuffer(1, IID_PPV_ARGS(&swapChainResources[1]));
    assert(SUCCEEDED(hr));


    // swapChainのRTVを作成
    D3D12_RENDER_TARGET_VIEW_DESC rtvDesc{};
    rtvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB; // 出力結果をSRGBに変換して書き込むように設定
    rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D; // 2dテクスチャとして書き込むように設定

    // wstring->stringに変換
    std::string windowNameStr = ConvertString(windowName);

    // RTVを作成
    ViewManager::CreateView(VIEW_TYPE::RTV, swapChainResources[0].Get(), &rtvDesc, windowNameStr + "_doubleBuffer_0");// ダブルバッファ用
    ViewManager::CreateView(VIEW_TYPE::RTV, swapChainResources[1].Get(), &rtvDesc, windowNameStr + "_doubleBuffer_1");// ダブルバッファ用

    // ハンドル取得
    doubleBufferRtvHandles[0] = ViewManager::GetHandleCPU(HEAP_TYPE::RTV, windowNameStr + "_doubleBuffer_0");
    doubleBufferRtvHandles[1] = ViewManager::GetHandleCPU(HEAP_TYPE::RTV, windowNameStr + "_doubleBuffer_1");

    // バックバッファのインデックスを取得
    backBufferIndex = swapChain->GetCurrentBackBufferIndex();
}

// swapChainのPresent
void WindowInfo::Present(UINT syncInterval, UINT flags){
    // 同期・入れ替え
    swapChain->Present(syncInterval, flags);
    // バックバッファのインデックスを取得
    backBufferIndex = swapChain->GetCurrentBackBufferIndex();
}
