#include <DxManager.h>
#include <SEED.h>
#include <Environment.h>
#include <MyMath.h>
#include <CS_Buffers.h>

void DxManager::Initialize(SEED* pSEED)
{
    // COMの初期化
    CoInitializeEx(0, COINIT_MULTITHREADED);

    // ポインタ代入
    pSEED_ = pSEED;
    // PSOManagerの作成
    psoManager_ = new PSOManager(this);
    // polygonManagerの作成
    polygonManager_ = new PolygonManager(this);
    pSEED_->SetPolygonManagerPtr(polygonManager_);
    // effectManagerの作成
    EffectManager* effectManagerInstance = new EffectManager(this);
    effectManager_.reset(effectManagerInstance);

    camera_ = new Camera();

    /*===========================================================================================*/
    /*                                   DirextXの初期化                                          */
    /*===========================================================================================*/

    /*------------ デバッグレイヤーの有効化 ---------------*/

#ifdef _DEBUG
    CreateDebugLayer();
#endif

    /* --------------  Deviceの生成 -------------------- */

    CreateDevice();

    /*---------- デバッグレイヤーでエラーが出た場合止める ----------*/

#ifdef _DEBUG
    CheckDebugLayer();
#endif


    /*===========================================================================================*/
    /*                                 GPUに送る命令の作成とか                                      */
    /*===========================================================================================*/

    /*----- コマンドに関わるものの生成 -------*/

    // GPUに命令を投げるやつとか命令の入れ物を作る
    CreateCommanders();

    /*===========================================================================================*/
    /*                               ダブルバッファリングにしよう                                    */
    /*===========================================================================================*/

    /*--------------------- スワップチェーンの作成 --------------------------*/

    // SwapChain ~ 画面を複数用意し、表示されていない画面で描画を同時進行で行う
    CreateRenderTargets();

    /*--------------- SwapChain から Resourceを引っ張ってくる ---------------*/

    // 【 Resource 】 ~ GPUのメモリ上にあるデータのうち、描画や表示に関わるものの総称。
    // 画面そのものもResourceなので、ここで取得
    GetSwapChainResources();

    /*---------------------- ディスクリプタヒープの作成 ----------------------*/

    /*
       【 View 】 ~ Resourceに対してどのような処理を行うのかをまとめたもの。Resourceを操作するにはViewが必要。
       【 Descriptor 】~ Viewを格納する場所。

        Viewの作成    : CPU
        Viewの置き場所 : CPU (descriptor)
        Viewの実行    : GPU (CPU上のdescriptorを参照できる)
    */

    CreateAllDescriptorHeap();

    // ディスクリプタのサイズを確認
    CheckDescriptorSize();

    /*----------------------------- RTVの作成 -----------------------------*/

    /*
        ここでは "描く" という処理を行いたいため、
        描画担当のView、"RTV"(RenderTargetVier) を作成
    */
    CreateRTV();

    /*------------------- CPUとGPUの同期のための変数作成 ---------------------*/

    /*
        GPUからfenceに対して、fenceValueの値が書き込まれるまでResetを待つようにする。
        そのために必要な変数を用意しておく
    */
    CreateFence();

    /*===========================================================================================*/
    /*                                ポリゴンを描画していくゾーン                                    */
    /*===========================================================================================*/


    /*---------------------- HLSLコンパイル --------------------------*/

    /*
        ポリゴンを描画するにはShaderの情報が必要。
        Shaderの中身はGPUで処理をするため、GPUが読める形式に
        変換しないといけない。
    */

    // Shaderコンパイルのためのコンパイラの初期化
    InitDxCompiler();

    // shaderをコンパイルする
    CompileShaders();

    /*------------------------PSOの生成-----------------------*/

    /*
                            ~ PSOとは ~
          長ったらしく工程数の多いレンダリングパイプラインの設定を
          個別で行うのではなくまとめて1つの変数に集約して行うことで
            アクセスの高速化、メンテナンス性などを向上させている。
        入力された情報を加工するのに関わるものがここに集約されている。工場。
    */

    // PSOManagerクラスに丸投げ
    InitPSO();

    /*------------------------- DepthStencilTextureResourceの作成 -------------------------*/

    depthStencilResource = CreateDepthStencilTextureResource(
        device.Get(),
        pSEED_->kClientWidth_,
        pSEED_->kClientHeight_
    );

    /*----------------------------------LightingのResource---------------------------------*/

    lightingResource = CreateBufferResource(device.Get(), sizeof(DirectionalLight));
    lightingResource->Map(0, nullptr, reinterpret_cast<void**>(&directionalLight));

    directionalLight->color_ = MyMath::FloatColor(0xffffffff);
    directionalLight->direction_ = { 0.0f,0.0f,1.0f };
    directionalLight->intensity = 1.0f;

    /*--------------------オフスクリーン用のテクスチャの初期化とSRVの作成--------------------*/

    InitializePostEffectTextures();

    /*----------------------------- Textureの初期化に関わる部分 -----------------------------*/

    // white1x1だけ読み込んでおく
    CreateTexture("resources/textures/white1x1.png");
    CreateTexture("resources/textures/uvChecker.png");

    /*------------------------------ DepthStencilViewの作成 -------------------------------*/

    // DSV用のヒ－プはヒープタイプが違うので別途作る
    dsvDescriptorHeap = CreateDescriptorHeap(
        device.Get(), D3D12_DESCRIPTOR_HEAP_TYPE_DSV, 1, false
    );

    // DSVの設定
    D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc{};
    dsvHandle = dsvDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
    dsvDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;

    // DSVHeapの先頭にDSVを作る
    device->CreateDepthStencilView(
        depthStencilResource.Get(),
        &dsvDesc,
        dsvDescriptorHeap->GetCPUDescriptorHandleForHeapStart()
    );

    /*--------------------------------- VewportとScissor ---------------------------------*/

    SettingViewportAndScissor(resolutionRate_);

    // 初期化時のものをデフォルト値として保存
    viewport_default = viewport;
    scissorRect_default = scissorRect;

    // ------------------------------------------------------------------------------------

    // カメラの情報
    camera_->transform_.scale_ = { 1.0f,1.0f,1.0f }; // scale
    camera_->transform_.rotate_ = { 0.0f,0.0f,0.0f }; // rotate
    camera_->transform_.translate_ = { 0.0f,1.0f,-10.0f }; // translate
    camera_->projectionMode_ = PERSPECTIVE;
    camera_->clipRange_ = kWindowSize;
    camera_->znear_ = 0.1f;
    camera_->zfar_ = 1000.0f;
    camera_->Update();

    // 情報がそろったのでpolygonManagerの初期化
    polygonManager_->InitResources();
}


/*===========================================================================================*/
/*                                      初期設定を行う関数                                      */
/*===========================================================================================*/

void DxManager::CreateDebugLayer()
{
    if(SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController)))) {
        // デバッグレイヤーを有効化する
        debugController->EnableDebugLayer();
        // さらにGPU側でもチェックを行うようにする
        debugController->SetEnableGPUBasedValidation(TRUE);
    }
}

void DxManager::CreateDevice()
{
    /*------------------------------- DXGIFactoryの生成 --------------------------------*/
    hr = CreateDXGIFactory(IID_PPV_ARGS(dxgiFactory.GetAddressOf()));
    // 作成失敗していたらアサート
    assert(SUCCEEDED(hr));

    /*--------------------------- 使用するアダプタ(GPU)の決定 -----------------------------*/

    // 性能のいいGPUからループが先に来るようfor文を回す
    for(
        int i = 0;
        dxgiFactory->EnumAdapterByGpuPreference(
            UINT(i),
            DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE,
            IID_PPV_ARGS(useAdapter.GetAddressOf())
        ) != DXGI_ERROR_NOT_FOUND;
        ++i
        ) {

        // アダプタの情報を取得する
        DXGI_ADAPTER_DESC3 adapterDesc{};
        hr = useAdapter->GetDesc3(&adapterDesc);

        // 取得できない場合アサート
        assert(SUCCEEDED(hr));

        // ソフトウェアアダプタでなければ採用
        if(adapterDesc.Flags != DXGI_ADAPTER_FLAG3_SOFTWARE) {

            // 使用アダプタを出力してループ終了
            Log(std::format(L"useAdapter:{}\n", adapterDesc.Description));
            break;

        }

        // ソフトウェアアダプタだった場合リセットして次のループへ  
        useAdapter = nullptr;
    }

    // アダプタが見つからなかった場合アサート
    assert(useAdapter != nullptr);

    /*------------------------------- D3D12Xデバイスの生成 --------------------------------*/

    // 
    D3D_FEATURE_LEVEL featureLevels[] = {
        D3D_FEATURE_LEVEL_12_2,D3D_FEATURE_LEVEL_12_1,D3D_FEATURE_LEVEL_12_0,
    };

    const char* featureLevelString[] = { "12.2","12.1","12.0" };

    for(size_t i = 0; i < _countof(featureLevels); ++i) {

        // 先ほど決定したアダプタを使用してデバイスを生成
        hr = D3D12CreateDevice(useAdapter.Get(), featureLevels[i], IID_PPV_ARGS(device.GetAddressOf()));

        // 生成に成功したらログを出力してループを終了
        if(SUCCEEDED(hr)) {
            Log(std::format("FeatureLevel : {}\n", featureLevelString[i]));
            break;
        }
    }

    // デバイス生成ができなかった場合アサート
    assert(device != nullptr);

    // 初期化成功をログに表示
    Log("Complete create D3D12Device!!!\n");
}

void DxManager::CheckDebugLayer()
{
    if(SUCCEEDED(device->QueryInterface(IID_PPV_ARGS(&infoQueue)))) {
        // ヤバエラー時に止まる
        infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_CORRUPTION, true);
        //エラー時に止まる
        infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_ERROR, true);
        // 警告時に止まる
        infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_WARNING, true);

        // 抑制するメッセージのID
        D3D12_MESSAGE_ID denyIds[] = {
            // Windows11でのDXGIデバッグレイヤーとDX12デバッグレイヤーの相互作用バグによるエラーメッセージ
            // https://stackoverflow.com/questions/69805245/directx-12-application-is-crashing-in-windows-11
            D3D12_MESSAGE_ID_RESOURCE_BARRIER_MISMATCHING_COMMAND_LIST_TYPE
        };

        // 抑制するレベル
        D3D12_MESSAGE_SEVERITY severities[] = { D3D12_MESSAGE_SEVERITY_INFO };
        D3D12_INFO_QUEUE_FILTER filter{};
        filter.DenyList.NumIDs = _countof(denyIds);
        filter.DenyList.pIDList = denyIds;
        filter.DenyList.NumSeverities = _countof(severities);
        filter.DenyList.pSeverityList = severities;

        // 指定したメッセージの表示を抑制する
        infoQueue->PushStorageFilter(&filter);

        // 解放
        infoQueue->Release();
    }
}

void DxManager::CreateCommanders()
{

    /*------------------------------- コマンドキューの生成 --------------------------------*/

    // 格納する変数
    D3D12_COMMAND_QUEUE_DESC commandQueueDesc{};

    // コマンドキューを生成する
    hr = device->CreateCommandQueue(&commandQueueDesc, IID_PPV_ARGS(commandQueue.GetAddressOf()));

    // コマンドキューの生成がうまくいかなかったので起動できない
    assert(SUCCEEDED(hr));


    /*------------------------------- コマンドリストの生成 --------------------------------*/

    // コマンドアロケータを生成する
    hr = device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(commandAllocator.GetAddressOf()));
    // コマンドアロケータの生成がうまくいかなかったので起動できない
    assert(SUCCEEDED(hr));

    // コマンドリストを生成する
    hr = device->CreateCommandList(
        0, D3D12_COMMAND_LIST_TYPE_DIRECT, commandAllocator.Get(), nullptr, IID_PPV_ARGS(commandList.GetAddressOf())
    );
    // コマンドリストの生成がうまくいかなかったので起動できない
    assert(SUCCEEDED(hr));
}

void DxManager::CreateRenderTargets()
{
    // 画面の縦横幅
    swapChainDesc.Width = pSEED_->kClientWidth_;
    swapChainDesc.Height = pSEED_->kClientHeight_;
    // 色の形式
    swapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    // マルチサンプルしない
    swapChainDesc.SampleDesc.Count = 1;
    swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT; // 描画のターゲットとして利用する
    swapChainDesc.BufferCount = 2; // ダブルバッファ
    swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD; // モニタに写したら中身を破棄
    // コマンドキュー、ウィンドウハンドル、設定を渡して生成する
    hr = dxgiFactory->CreateSwapChainForHwnd(
        commandQueue.Get(),
        SEED::GetHWND(),
        &swapChainDesc,
        nullptr, nullptr,
        reinterpret_cast<IDXGISwapChain1**>(swapChain.GetAddressOf())
    );

    // 生成失敗したらアサート
    assert(SUCCEEDED(hr));

    // offScreen用のレンダーターゲットを作成
    offScreenResource = CreateRenderTargetTextureResource(device.Get(), pSEED_->kClientWidth_, pSEED_->kClientHeight_);
}

void DxManager::GetSwapChainResources()
{
    // SwapChain から Resourceを引っ張ってくる
    // うまく取得できなければ起動できない
    hr = swapChain->GetBuffer(0, IID_PPV_ARGS(&swapChainResources[0]));
    assert(SUCCEEDED(hr));
    hr = swapChain->GetBuffer(1, IID_PPV_ARGS(&swapChainResources[1]));
    assert(SUCCEEDED(hr));

}

void DxManager::CreateAllDescriptorHeap()
{
    // RTVのディスクリプタヒープを作成
    rtvDescriptorHeap.Attach(CreateDescriptorHeap(
        device.Get(),
        D3D12_DESCRIPTOR_HEAP_TYPE_RTV,//レンダーターゲットビュー用に設定
        3,// ダブルバッファ用とオフスクリーンように計3つ。 多くても別に構わない
        false
    ));


    // SRVのディスクリプタヒープを作成
    SRV_UAV_DescriptorHeap.Attach(CreateDescriptorHeap(
        device.Get(),
        D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV,// SRV用に設定
        0xffff,// ディスクリプタ数
        true
    ));

}

void DxManager::CheckDescriptorSize()
{
    // ディスクリプタのサイズ
    descriptorSizeSRV_UAV =
        device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
    descriptorSizeRTV =
        device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
    descriptorSizeDSV =
        device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
}

void DxManager::CreateRTV()
{
    // RTVの設定
    rtvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB; // 出力結果をSRGBに変換して書き込むように設定
    rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D; // 2dテクスチャとして書き込むように設定

    // ダブルバッファなのでRTVを2つとオフスクリーン用に1つ作る。
    // ディスクリプタヒープの先頭から作成するので先頭アドレスを取得し、順番に入れていく
    rtvHandles[0] = rtvDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
    rtvHandles[1].ptr = rtvHandles[0].ptr + descriptorSizeRTV;
    offScreenRtvHandle.ptr = rtvHandles[0].ptr + (descriptorSizeRTV * 2);

    // 取得したアドレスにRTVを3つ作る
    device->CreateRenderTargetView(swapChainResources[0].Get(), &rtvDesc, rtvHandles[0]);// swapChain用 * 2
    device->CreateRenderTargetView(swapChainResources[1].Get(), &rtvDesc, rtvHandles[1]);
    device->CreateRenderTargetView(offScreenResource.Get(), &rtvDesc, offScreenRtvHandle); // オフスクリーン用 * 1
}

void DxManager::InitializePostEffectTextures()
{
    // SRVの設定を行う変数
    D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
    D3D12_CPU_DESCRIPTOR_HANDLE textureSrvHandleCPU;
    D3D12_GPU_DESCRIPTOR_HANDLE textureSrvHandleGPU;
    //uav
    D3D12_UNORDERED_ACCESS_VIEW_DESC uavDesc{};
    D3D12_CPU_DESCRIPTOR_HANDLE textureUavHandleCPU;

    //================================ オフスクリーンのキャプチャ用テクスチャの初期化 ================================//

    // Texture用SRVの作成
    srvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
    srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
    srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D; // 2Dテクスチャ
    srvDesc.Texture2D.MipLevels = 1;

    textureSrvHandleCPU = GetCPUDescriptorHandle(SRV_UAV_DescriptorHeap.Get(), descriptorSizeSRV_UAV, 1 + systemTextureCount);
    textureSrvHandleGPU = GetGPUDescriptorHandle(SRV_UAV_DescriptorHeap.Get(), descriptorSizeSRV_UAV, 1 + systemTextureCount);

    // SRVの生成
    device->CreateShaderResourceView(offScreenResource.Get(), &srvDesc, textureSrvHandleCPU);

    systemTextures_["offScreenTexture"] = systemTextureCount;
    systemTextureCount++;

    //========================= DepthStencilResourceをテクスチャとして参照する用のSRV作成 ===============================//

    // depthStencilを参照する用のSRV
    srvDesc.Format = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
    srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
    srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D; // 2Dテクスチャ
    srvDesc.Texture2D.MipLevels = 1;

    textureSrvHandleCPU = GetCPUDescriptorHandle(SRV_UAV_DescriptorHeap.Get(), descriptorSizeSRV_UAV, 1 + systemTextureCount);
    textureSrvHandleGPU = GetGPUDescriptorHandle(SRV_UAV_DescriptorHeap.Get(), descriptorSizeSRV_UAV, 1 + systemTextureCount);

    // SRVの生成
    device->CreateShaderResourceView(depthStencilResource.Get(), &srvDesc, textureSrvHandleCPU);

    systemTextures_["depthStencilSRV"] = systemTextureCount;
    systemTextureCount++;

    //========================= 深度情報をテクスチャとして表示する用のResource作成 ===============================//

    depthTextureResource.Attach(InitializeTextureResource(
        device.Get(),
        pSEED_->kClientWidth_,
        pSEED_->kClientHeight_,
        DXGI_FORMAT_R8G8B8A8_UNORM,
        STATE_UNORDERED_ACCESS
    ));

    // Texture用SRVの作成
    srvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
    srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D; // 2Dテクスチャ
    srvDesc.Texture2D.MipLevels = 1;
    textureSrvHandleCPU = GetCPUDescriptorHandle(SRV_UAV_DescriptorHeap.Get(), descriptorSizeSRV_UAV, 1 + systemTextureCount);
    textureSrvHandleGPU = GetGPUDescriptorHandle(SRV_UAV_DescriptorHeap.Get(), descriptorSizeSRV_UAV, 1 + systemTextureCount);

    // SRVの生成
    device->CreateShaderResourceView(depthTextureResource.Get(), &srvDesc, textureSrvHandleCPU);

    systemTextures_["depthTexture_SRV"] = systemTextureCount;
    systemTextureCount++;

    // UAVの作成
    uavDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    uavDesc.ViewDimension = D3D12_UAV_DIMENSION_TEXTURE2D;

    textureUavHandleCPU = GetCPUDescriptorHandle(SRV_UAV_DescriptorHeap.Get(), descriptorSizeSRV_UAV, 1 + systemTextureCount);
    device->CreateUnorderedAccessView(depthTextureResource.Get(), nullptr, &uavDesc, textureUavHandleCPU);

    systemTextures_["depthTexture_UAV"] = systemTextureCount;
    systemTextureCount++;

    //==================================== ぼけたテクスチャ用のResourceの初期化 =======================================//

    blurTextureResource.Attach(InitializeTextureResource(
        device.Get(),
        pSEED_->kClientWidth_,
        pSEED_->kClientHeight_,
        DXGI_FORMAT_R8G8B8A8_UNORM,
        STATE_UNORDERED_ACCESS
    ));


    // Texture用SRVの作成
    srvDesc.Format = blurTextureResource->GetDesc().Format;
    srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
    srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D; // 2Dテクスチャ
    srvDesc.Texture2D.MipLevels = 1;

    textureSrvHandleCPU = GetCPUDescriptorHandle(SRV_UAV_DescriptorHeap.Get(), descriptorSizeSRV_UAV, 1 + systemTextureCount);
    textureSrvHandleGPU = GetGPUDescriptorHandle(SRV_UAV_DescriptorHeap.Get(), descriptorSizeSRV_UAV, 1 + systemTextureCount);

    // SRVの生成
    device->CreateShaderResourceView(blurTextureResource.Get(), &srvDesc, textureSrvHandleCPU);

    systemTextures_["blurredTexture_SRV"] = systemTextureCount;
    systemTextureCount++;


    // UAVの作成
    uavDesc.Format = blurTextureResource->GetDesc().Format;
    uavDesc.ViewDimension = D3D12_UAV_DIMENSION_TEXTURE2D;

    textureUavHandleCPU = GetCPUDescriptorHandle(SRV_UAV_DescriptorHeap.Get(), descriptorSizeSRV_UAV,1 + systemTextureCount);
    device->CreateUnorderedAccessView(blurTextureResource.Get(), nullptr, &uavDesc, textureUavHandleCPU);

    systemTextures_["blurredTexture_UAV"] = systemTextureCount;
    systemTextureCount++;
}

void DxManager::CreateFence()
{
    //初期値でFenceを作る
    fenceValue = 0;
    hr = device->CreateFence(fenceValue, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(fence.GetAddressOf()));
    assert(SUCCEEDED(hr));

    // FenceのSignalを待つためのイベントを作成する
    fenceEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
    assert(fenceEvent != nullptr);
}

void DxManager::InitDxCompiler()
{
    // インスタンスの作成
    hr = DxcCreateInstance(CLSID_DxcUtils, IID_PPV_ARGS(dxcUtils.GetAddressOf()));
    assert(SUCCEEDED(hr));
    hr = DxcCreateInstance(CLSID_DxcCompiler, IID_PPV_ARGS(dxcCompiler.GetAddressOf()));
    assert(SUCCEEDED(hr));

    // 現時点でincludeはしないが、 includeに対応するための設定を行っておく
    hr = dxcUtils->CreateDefaultIncludeHandler(includeHandler.GetAddressOf());
    assert(SUCCEEDED(hr));
}

void DxManager::CompileShaders()
{
    // VertexShader
    vertexShaderBlob = CompileShader(
        L"resources/shaders/Object3d.VS.hlsl",
        L"vs_6_0",
        L"main",
        dxcUtils.Get(),
        dxcCompiler.Get(),
        includeHandler.Get()
    );
    assert(vertexShaderBlob != nullptr);

    // PixelShader
    pixelShaderBlob = CompileShader(
        L"resources/shaders/Object3d.PS.hlsl",
        L"ps_6_0",
        L"main",
        dxcUtils.Get(),
        dxcCompiler.Get(),
        includeHandler.Get()
    );
    assert(pixelShaderBlob != nullptr);

    // ComputeShader
    computeShaderBlob = CompileShader(
        L"resources/shaders/Blur.CS.hlsl",
        L"cs_6_0",
        L"CSMain",
        dxcUtils.Get(),
        dxcCompiler.Get(),
        includeHandler.Get()
    );
    assert(computeShaderBlob != nullptr);
}

void DxManager::InitPSO()
{
    //psoManager_->Create(
    //    commonRootSignature[(int)BlendMode::NORMAL][(int)PolygonTopology::TRIANGLE].GetAddressOf(),
    //    commonPipelineState[(int)BlendMode::NORMAL][(int)PolygonTopology::TRIANGLE].GetAddressOf(),
    //    PolygonTopology::TRIANGLE, BlendMode::NORMAL
    //);

    for(int blendMode = 0; blendMode < 5; blendMode++){
        for(int topology = 0; topology < 2; topology++){
            psoManager_->Create(
                commonRootSignature[blendMode][topology].GetAddressOf(),
                commonPipelineState[blendMode][topology].GetAddressOf(),
                PolygonTopology(topology), BlendMode(blendMode)
            );
        }
    }

    // ComputeShader用
    csRootSignature.Attach(psoManager_->SettingCSRootSignature());
    D3D12_COMPUTE_PIPELINE_STATE_DESC psoDesc = {};
    psoDesc.pRootSignature = csRootSignature.Get();
    psoDesc.CS = { reinterpret_cast<BYTE*>(computeShaderBlob->GetBufferPointer()), computeShaderBlob->GetBufferSize() };

    // CBVの初期化
    CS_ConstantBuffer.Attach(CreateBufferResource(device.Get(), sizeof(Blur_CS_ConstantBuffer)));

    device->CreateComputePipelineState(&psoDesc, IID_PPV_ARGS(csPipelineState.GetAddressOf()));
}

void DxManager::SettingViewportAndScissor(float resolutionRate)
{

    // ビューポート
    // クライアント領域のサイズと一緒にして画面全体に表示
    viewport.Width = std::ceil(float(pSEED_->kClientWidth_) * resolutionRate);
    viewport.Height = std::ceil(float(pSEED_->kClientHeight_) * resolutionRate);
    viewport.TopLeftX = 0;
    viewport.TopLeftY = 0;
    viewport.MinDepth = 0.0f;
    viewport.MaxDepth = 1.0f;

    // シザー矩形
    // 基本的にビューポートと同じ矩形が構成されるようにする
    scissorRect.left = 0;
    scissorRect.top = 0;
    scissorRect.right = (int)std::ceil(float(pSEED_->kClientWidth_) * resolutionRate);
    scissorRect.bottom = (int)std::ceil(float(pSEED_->kClientHeight_) * resolutionRate);
}


void DxManager::TransitionResourceState(ID3D12Resource* resource, D3D12_RESOURCE_STATES stateBefore, D3D12_RESOURCE_STATES stateAfter)
{
    // 同じなら処理しない
    if(stateAfter == stateBefore){ return; }

    // バリアを設定してリソースの状態を遷移させる
    CD3DX12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(
        resource,
        stateBefore,
        stateAfter
    );

    // リソースのstateを変更
    commandList->ResourceBarrier(1, &barrier);
}

void DxManager::ClearViewSettings()
{
    // 指定した色で画面全体をクリアする
    clearColor = MyMath::FloatColor(SEED::GetWindowColor());
    commandList->ClearRenderTargetView(rtvHandles[backBufferIndex], &clearColor.x, 0, nullptr);
    commandList->ClearRenderTargetView(offScreenRtvHandle, &clearColor.x, 0, nullptr);

    // DSVとオフスクリーンのRTVを結びつける
    commandList->OMSetRenderTargets(1, &offScreenRtvHandle, false, &dsvHandle);

    // フレームの最初にもっとも遠くにクリアする
    commandList->ClearDepthStencilView(dsvHandle, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);
}

void DxManager::WaitForGPU()
{
    // Fenceの値を更新
    fenceValue++;
    // GPUがここまでたどり着いたときに、 Fenceの値を指定した値に代入するようにSignalを送る
    commandQueue->Signal(fence.Get(), fenceValue);

    // Fenceの値が指定したSignal値にたどり着いているか確認する
    // GetCompletedValueの初期値はFence作成時に渡した初期値
    if(fence->GetCompletedValue() < fenceValue) {

        // 指定したSignalにたどりついていないので、GPUがたどり着くまで待つようにイベントを設定する
        fence->SetEventOnCompletion(fenceValue, fenceEvent);
        // CPUはイベント待つ
        WaitForSingleObject(fenceEvent, INFINITE);
    }
}

/*===========================================================================================*/
/*                                          描画関数                                          */
/*===========================================================================================*/


void DxManager::PreDraw()
{

    // フレームの初めに前フレームの値を保存
    SavePreVariable();

    /*------- これから書き込むバックバッファのインデックスを取得--------*/

    backBufferIndex = swapChain->GetCurrentBackBufferIndex();

    /*--------------- TransitionBarrierを張る処理----------------*/

    // RTVを実行するために表示されていない後ろの画面(バックバッファ)の状態を描画状態に遷移させる
    TransitionResourceState(swapChainResources[backBufferIndex].Get(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);

    /*----------画面、深度情報のクリア、DSVとRTVの結び直し-----------*/

    ClearViewSettings();

    /*----------------- SRVのヒープの再セット --------------------*/
    /*
        imGuiがフレーム単位でHeapの中身を操作するため
        SRVのHeapは毎フレームセットし直す
    */
    ID3D12DescriptorHeap* ppHeaps[] = { SRV_UAV_DescriptorHeap.Get() };
    commandList->SetDescriptorHeaps(1, ppHeaps);
}

void DxManager::DrawPolygonAll()
{
    polygonManager_->DrawPolygonAll();

    // ここでぼかした画面を作る
    effectManager_->TransfarToCS();

    // ここでオフスクリーンからバックバッファに描画対象を切り変える
    commandList->ClearRenderTargetView(rtvHandles[backBufferIndex], &clearColor.x, 0, nullptr);
    commandList->ClearDepthStencilView(dsvHandle, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);
    commandList->OMSetRenderTargets(1, &rtvHandles[backBufferIndex], false, &dsvHandle);

    // ぼかした画面を写せる状態に
    TransitionResourceState(
        blurTextureResource.Get(),
        D3D12_RESOURCE_STATE_UNORDERED_ACCESS,
        D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE
    );

    TransitionResourceState(
        offScreenResource.Get(),
        D3D12_RESOURCE_STATE_RENDER_TARGET,
        D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE
    );

    TransitionResourceState(
        depthStencilResource.Get(),
        D3D12_RESOURCE_STATE_DEPTH_WRITE,
        D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE
    );

    TransitionResourceState(
        depthTextureResource.Get(),
        D3D12_RESOURCE_STATE_UNORDERED_ACCESS,
        D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE
    );

    // 最終結果を画面サイズの矩形に貼り付けて表示
    polygonManager_->DrawResult();


    // すべてのリソースを基本の状態に戻す
    TransitionResourceState(
        blurTextureResource.Get(),
        D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
        D3D12_RESOURCE_STATE_UNORDERED_ACCESS
    );

    TransitionResourceState(
        offScreenResource.Get(),
        D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
        D3D12_RESOURCE_STATE_RENDER_TARGET
    );

    TransitionResourceState(
        depthTextureResource.Get(),
        D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
        D3D12_RESOURCE_STATE_UNORDERED_ACCESS
    );

    //TransitionResourceState(
    //    depthStencilResource.Get(),
    //    D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
    //    D3D12_RESOURCE_STATE_DEPTH_WRITE
    //);
    
    // すべての描画が終了したのでオフスクリーンのRTVをクリアする。
    commandList->ClearRenderTargetView(offScreenRtvHandle, &clearColor.x, 0, nullptr);
}

void DxManager::PostDraw()
{
    // 画面に描く処理はすべて終わり、 画面に映すので、状態を遷移
    // 今回はRenderTargetからPresent にする
    TransitionResourceState(swapChainResources[backBufferIndex].Get(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);


    /*----------------------------------------------------------*/

    // コマンドリストの内容を確定させる。 すべてのコマンドを積んでからCloseすること
    hr = commandList->Close();// コマンドリストの実行前に閉じる
    assert(SUCCEEDED(hr));

    // GPUにコマンドリストの実行を行わせる
    ID3D12CommandList* commandLists[] = { commandList.Get() };
    commandQueue->ExecuteCommandLists(1, commandLists);

    // GPUとOSに画面の交換を行うよう通知する
    swapChain->Present(1, 0);

    /*---------------------- CPUとGPUの同期 ----------------------*/

    WaitForGPU();// CPUはGPUの処理が追いつくまでここで待機！

    /*----------------- PolygonManagerのリセット -----------------*/

    polygonManager_->Reset();

    /*-------------　解像度の変更命令があればここで実行-　-------------*/

    if(changeResolutionOrder){
        ReCreateResolutionSettings();
    }

    /*------------------- commandListのリセット ------------------*/
    // リセットして次のフレーム用のコマンドリストを準備
    hr = commandAllocator->Reset();
    assert(SUCCEEDED(hr));
    hr = commandList->Reset(commandAllocator.Get(), nullptr);
    assert(SUCCEEDED(hr));
}

/*===========================================================================================*/
/*                                  外部から実行される関数                                       */
/*===========================================================================================*/

//////////////////////////////////////////////////////////////
//           テクスチャを作成してグラフハンドルを返す関数          //
//////////////////////////////////////////////////////////////

uint32_t DxManager::CreateTexture(std::string filePath)
{

    // 既にある場合
    if(textures_.find(filePath) != textures_.end()){ return textures_[filePath]; }

    /*----------------------------- TextureResourceの作成,転送 -----------------------------*/

    // 読み込み
    DirectX::ScratchImage mipImages = LoadTextureImage(filePath);
    // 作成
    const DirectX::TexMetadata& metadata = mipImages.GetMetadata();
    textureResource.push_back(CreateTextureResource(device.Get(), metadata));
    // 転送
    intermediateResource.push_back(
        UploadTextureData(textureResource.back().Get(), mipImages, device.Get(), commandList.Get())
    );

    /*-------------------------------- Texture用SRVの作成 ----------------------------------*/

    // metaDataをもとにSRVの設定
    D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
    srvDesc.Format = metadata.format;
    srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
    srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;//2Dテクスチャ
    srvDesc.Texture2D.MipLevels = UINT(metadata.mipLevels);

    // SRVを作成するDescriptorHeapの場所を決める(0番目,imgui 1番目,キャプチャ用なので2番目からスタート)
    D3D12_CPU_DESCRIPTOR_HANDLE textureSrvHandleCPU =
        GetCPUDescriptorHandle(SRV_UAV_DescriptorHeap.Get(), descriptorSizeSRV_UAV, 1 + systemTextureCount + textureCount_);
    D3D12_GPU_DESCRIPTOR_HANDLE textureSrvHandleGPU =
        GetGPUDescriptorHandle(SRV_UAV_DescriptorHeap.Get(), descriptorSizeSRV_UAV, 1 + systemTextureCount + textureCount_);

    // SRVの生成
    device->CreateShaderResourceView(textureResource.back().Get(), &srvDesc, textureSrvHandleCPU);

    // グラフハンドルもついでに返す
    textures_[filePath] = systemTextureCount + textureCount_;
    return systemTextureCount + textureCount_++;// システム画像の枚数 + 通常のテクスチャの枚数
}



//////////////////////////////////////////////////////////////
//           解像度を変更してRTVやScissorを設定し直す関数        ///
//////////////////////////////////////////////////////////////

void DxManager::ChangeResolutionRate(float resolutionRate)
{
    resolutionRate_ = std::clamp(resolutionRate, 0.0f, 1.0f);
    changeResolutionOrder = true;
}

void DxManager::ReCreateResolutionSettings()
{
    SettingViewportAndScissor(resolutionRate_);// scissorなどを再設定

    // 命令フラグを下げる
    changeResolutionOrder = false;
}

void DxManager::SavePreVariable()
{
    preResolutionRate_ = resolutionRate_;
}


/*===========================================================================================*/
/*                                          後処理                                            */
/*===========================================================================================*/

void DxManager::Finalize()
{
    CloseHandle(fenceEvent);
    polygonManager_->Finalize();

    delete psoManager_;
    psoManager_ = nullptr;
    delete polygonManager_;
    polygonManager_ = nullptr;
    delete camera_;
    camera_ = nullptr;

#ifdef _DEBUG
    debugController->Release();
#endif
    CloseWindow(SEED::GetHWND());
    // COMの終了
    CoUninitialize();
}

LeakChecker::~LeakChecker()
{
    // 解放漏れがないかチェック
    ComPtr<IDXGIDebug1> debug;
    if(SUCCEEDED(DXGIGetDebugInterface1(0, IID_PPV_ARGS(&debug)))) {
        debug->ReportLiveObjects(DXGI_DEBUG_ALL, DXGI_DEBUG_RLO_ALL);
        debug->ReportLiveObjects(DXGI_DEBUG_APP, DXGI_DEBUG_RLO_ALL);
        debug->ReportLiveObjects(DXGI_DEBUG_D3D12, DXGI_DEBUG_RLO_ALL);
        debug->Release();
    }
}
