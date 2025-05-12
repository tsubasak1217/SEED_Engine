#include <SEED/Source/Manager/DxManager/DxManager.h>
#include <thread>
#include <SEED/Source/SEED.h>
#include <Environment/Environment.h>
#include <SEED/Lib/Functions/MyFunc/MyMath.h>
#include <SEED/Lib/Structs/CS_Buffers.h>
#include <SEED/Source/Manager/InputManager/InputManager.h>
#include <SEED/Source/Manager/DxManager/PSO/PSOManager.h>

DxManager* DxManager::instance_ = nullptr;


void DxManager::Initialize(SEED* pSEED){
    // インスタンスの取得
    GetInstance();

    // COMの初期化
    CoInitializeEx(0, COINIT_MULTITHREADED);

    // ポインタ代入
    instance_->pSEED_ = pSEED;
    // PSOManagerの作成
    PSOManager::GetInstance();
    // polygonManagerの作成
    instance_->polygonManager_ = new PolygonManager(instance_);
    instance_->pSEED_->SetPolygonManagerPtr(instance_->polygonManager_);
    // effectManagerの作成
    EffectManager* effectManagerInstance = new EffectManager(instance_);
    instance_->effectManager_.reset(effectManagerInstance);


    /*===========================================================================================*/
    /*                                   DirextXの初期化                                          */
    /*===========================================================================================*/

    /*------------ デバッグレイヤーの有効化 ---------------*/

#ifdef _DEBUG
    instance_->CreateDebugLayer();
#endif

    /* --------------  Deviceの生成 -------------------- */

    instance_->CreateDevice();

    // view管理マネージャの初期化
    ViewManager::GetInstance();

    /*---------- デバッグレイヤーでエラーが出た場合止める ----------*/

#ifdef _DEBUG
    instance_->CheckDebugLayer();
#endif


    /*===========================================================================================*/
    /*                                 GPUに送る命令の作成とか                                      */
    /*===========================================================================================*/

    /*----- コマンドに関わるものの生成 -------*/

    // GPUに命令を投げるやつとか命令の入れ物を作る
    instance_->CreateCommanders();

    /*===========================================================================================*/
    /*                               ダブルバッファリングにしよう                                    */
    /*===========================================================================================*/

    /*--------------------- スワップチェーンの作成 --------------------------*/

    // SwapChain ~ 画面を複数用意し、表示されていない画面で描画を同時進行で行う
    instance_->CreateRenderTargets();


    /*----------------------------- RTVの作成 -----------------------------*/

    /*
        ここでは "描く" という処理を行いたいため、
        描画担当のView、"RTV"(RenderTargetVier) を作成
    */
    instance_->CreateRTV();

    /*------------------- CPUとGPUの同期のための変数作成 ---------------------*/

    /*
        GPUからfenceに対して、fenceValueの値が書き込まれるまでResetを待つようにする。
        そのために必要な変数を用意しておく
    */
    instance_->CreateFence();

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
    instance_->InitDxCompiler();

    // shaderをコンパイルする
    instance_->CompileShaders();

    /*------------------------PSOの生成-----------------------*/

    /*
                            ~ PSOとは ~
          長ったらしく工程数の多いレンダリングパイプラインの設定を
          個別で行うのではなくまとめて1つの変数に集約して行うことで
            アクセスの高速化、メンテナンス性などを向上させている。
        入力された情報を加工するのに関わるものがここに集約されている。工場。
    */

    // PSOManagerクラスに丸投げ
    instance_->InitPSO();

    /*------------------------- DepthStencilTextureResourceの作成 -------------------------*/

    instance_->depthStencilResource = CreateDepthStencilTextureResource(
        instance_->device.Get(),
        instance_->pSEED_->kClientWidth_,
        instance_->pSEED_->kClientHeight_
    );

    instance_->depthStencilResource->SetName(L"depthStencilResource");

    /*--------------------オフスクリーン用のテクスチャの初期化とSRVの作成--------------------*/

    instance_->InitializeSystemTextures();

    /*----------------------------- Textureの初期化に関わる部分 -----------------------------*/

    // white1x1だけ読み込んでおく
    instance_->CreateTexture("resources/textures/Assets/white1x1.png");
    instance_->CreateTexture("resources/textures/Assets/uvChecker.png");

    /*------------------------------ DepthStencilViewの作成 -------------------------------*/

    // DSVの設定
    D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc{};
    dsvDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;

    // DSVHeapの先頭にDSVを作る
    ViewManager::CreateView(VIEW_TYPE::DSV, instance_->depthStencilResource.Get(), &dsvDesc, "depthStencil_1");
    // ハンドルを得る
    instance_->dsvHandle = ViewManager::GetHandleCPU(DESCRIPTOR_HEAP_TYPE::DSV, "depthStencil_1");

    /*--------------------------------- VewportとScissor ---------------------------------*/

    instance_->SettingViewportAndScissor(instance_->resolutionRate_);

    // 初期化時のものをデフォルト値として保存
    instance_->viewport_default = instance_->viewport;
    instance_->scissorRect_default = instance_->scissorRect;

    // ------------------------------------------------------------------------------------

    instance_->camera_ = CameraManager::GetCamera("main");

    // 情報がそろったのでpolygonManagerの初期化
    instance_->polygonManager_->InitResources();
}


/*===========================================================================================*/
/*                                      初期設定を行う関数                                      */
/*===========================================================================================*/

void DxManager::CreateDebugLayer(){
    if(SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController)))) {
        // デバッグレイヤーを有効化する
        debugController->EnableDebugLayer();
        // さらにGPU側でもチェックを行うようにする
        debugController->SetEnableGPUBasedValidation(TRUE);
    }
}

void DxManager::CreateDevice(){
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

void DxManager::CheckDebugLayer(){

    if(SUCCEEDED(device->QueryInterface(IID_PPV_ARGS(&infoQueue)))) {
        // ヤバエラー時に止まる
        infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_CORRUPTION, true);
        //エラー時に止まる
        infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_ERROR, true);
        // 警告時に止まる
        //infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_WARNING, true);

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
    }
}

void DxManager::CreateCommanders(){

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

void DxManager::CreateRenderTargets(){

    // swapChainの作成
    WindowManager::GetWindow(pSEED_->windowTitle_)->CreateSwapChain(dxgiFactory.Get(), commandQueue.Get());
#ifdef _DEBUG
    WindowManager::GetWindow(pSEED_->systemWindowTitle_)->CreateSwapChain(dxgiFactory.Get(), commandQueue.Get());
#endif // _DEBUG

    // offScreen用のレンダーターゲットを作成
    offScreenResource = CreateRenderTargetTextureResource(device.Get(), pSEED_->kClientWidth_, pSEED_->kClientHeight_);

    // リソースに名前をつける
    offScreenResource.Get()->SetName(L"offScreenResource");
}


void DxManager::CreateRTV(){
    // RTVの設定
    D3D12_RENDER_TARGET_VIEW_DESC rtvDesc{};
    rtvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB; // 出力結果をSRGBに変換して書き込むように設定
    rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D; // 2dテクスチャとして書き込むように設定

    // RTVを作成
    ViewManager::CreateView(VIEW_TYPE::RTV, offScreenResource.Get(), &rtvDesc, "offScreen_0");// オフスクリーン用

    // ハンドル取得
    offScreenHandle = ViewManager::GetHandleCPU(DESCRIPTOR_HEAP_TYPE::RTV, "offScreen_0");
}

void DxManager::InitializeSystemTextures(){
    // SRVの設定を行う変数
    D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
    //uav
    D3D12_UNORDERED_ACCESS_VIEW_DESC uavDesc{};

    //================================ オフスクリーンのキャプチャ用テクスチャの初期化 ================================//

    // Texture用SRVの作成
    srvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
    srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
    srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D; // 2Dテクスチャ
    srvDesc.Texture2D.MipLevels = 1;

    // SRVの生成
    ViewManager::CreateView(VIEW_TYPE::SRV, offScreenResource.Get(), &srvDesc, "offScreen_0");

    //========================= DepthStencilResourceをテクスチャとして参照する用のSRV作成 ===============================//

    // depthStencilを参照する用のSRV
    srvDesc.Format = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
    srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
    srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D; // 2Dテクスチャ
    srvDesc.Texture2D.MipLevels = 1;

    // SRVの生成
    ViewManager::CreateView(VIEW_TYPE::SRV, depthStencilResource.Get(), &srvDesc, "depth_0");

    //========================= 深度情報をテクスチャとして表示する用のResource作成 ===============================//

    depthTextureResource = InitializeTextureResource(
        device.Get(),
        pSEED_->kClientWidth_,
        pSEED_->kClientHeight_,
        DXGI_FORMAT_R8G8B8A8_UNORM,
        STATE_UNORDERED_ACCESS
    );

    // 名前の設定
    depthTextureResource->SetName(L"depthTextureResource");

    // Texture用SRVの作成
    srvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
    srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D; // 2Dテクスチャ
    srvDesc.Texture2D.MipLevels = 1;

    // SRVの生成
    ViewManager::CreateView(VIEW_TYPE::SRV, depthTextureResource.Get(), &srvDesc, "depth_1");

    // UAVの作成
    uavDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    uavDesc.ViewDimension = D3D12_UAV_DIMENSION_TEXTURE2D;
    ViewManager::CreateView(VIEW_TYPE::UAV, depthTextureResource.Get(), &uavDesc, "depth_1_UAV");

    //==================================== ぼけたテクスチャ用のResourceの初期化 =======================================//

    blurTextureResource = InitializeTextureResource(
        device.Get(),
        pSEED_->kClientWidth_,
        pSEED_->kClientHeight_,
        DXGI_FORMAT_R8G8B8A8_UNORM,
        STATE_UNORDERED_ACCESS
    );

    // 名前の設定
    blurTextureResource->SetName(L"blurTextureResource");

    // Texture用SRVの作成
    srvDesc.Format = blurTextureResource->GetDesc().Format;
    srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
    srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D; // 2Dテクスチャ
    srvDesc.Texture2D.MipLevels = 1;

    // SRVの生成
    ViewManager::CreateView(VIEW_TYPE::SRV, blurTextureResource.Get(), &srvDesc, "blur_0");

    // UAVの作成
    uavDesc.Format = blurTextureResource->GetDesc().Format;
    uavDesc.ViewDimension = D3D12_UAV_DIMENSION_TEXTURE2D;

    // UAVの生成
    ViewManager::CreateView(VIEW_TYPE::UAV, blurTextureResource.Get(), &uavDesc, "blur_0_UAV");
}

void DxManager::CreateFence(){
    //初期値でFenceを作る
    fenceValue = 0;
    hr = device->CreateFence(fenceValue, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(fence.GetAddressOf()));
    assert(SUCCEEDED(hr));

    // FenceのSignalを待つためのイベントを作成する
    fenceEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
    assert(fenceEvent != nullptr);
}

void DxManager::InitDxCompiler(){
    // インスタンスの作成
    hr = DxcCreateInstance(CLSID_DxcUtils, IID_PPV_ARGS(dxcUtils.GetAddressOf()));
    assert(SUCCEEDED(hr));
    hr = DxcCreateInstance(CLSID_DxcCompiler, IID_PPV_ARGS(dxcCompiler.GetAddressOf()));
    assert(SUCCEEDED(hr));

    // 現時点でincludeはしないが、 includeに対応するための設定を行っておく
    hr = dxcUtils->CreateDefaultIncludeHandler(includeHandler.GetAddressOf());
    assert(SUCCEEDED(hr));
}

void DxManager::CompileShaders(){

    // 6.5に対応しているか確認
    {
        D3D12_FEATURE_DATA_SHADER_MODEL shaderModel = { D3D_SHADER_MODEL_6_5 };
        hr = device->CheckFeatureSupport(D3D12_FEATURE_SHADER_MODEL, &shaderModel, sizeof(shaderModel));
        assert(SUCCEEDED(hr));
    }

    // メッシュシェーダーに対応しているか確認
    {
        D3D12_FEATURE_DATA_D3D12_OPTIONS7 features = {};
        hr = device->CheckFeatureSupport(D3D12_FEATURE_D3D12_OPTIONS7, &features, sizeof(features));
        assert(SUCCEEDED(hr));
    }


    // VertexShader
    vsBlobs["commonVS"] = CompileShader(
        L"resources/shaders/Object3d.VS.hlsl",
        L"vs_6_0",
        L"main",
        dxcUtils.Get(),
        dxcCompiler.Get(),
        includeHandler.Get()
    );
    assert(vsBlobs["commonVS"] != nullptr);

    vsBlobs["skinningVS"] = CompileShader(
        L"resources/shaders/Skinning.VS.hlsl",
        L"vs_6_0",
        L"main",
        dxcUtils.Get(),
        dxcCompiler.Get(),
        includeHandler.Get()
    );
    assert(vsBlobs["skinningVS"] != nullptr);

    // PixelShader
    psBlobs["commonPS"] = CompileShader(
        L"resources/shaders/Object3d.PS.hlsl",
        L"ps_6_0",
        L"main",
        dxcUtils.Get(),
        dxcCompiler.Get(),
        includeHandler.Get()
    );
    assert(psBlobs["commonPS"] != nullptr);

    // ComputeShader
    csBlobs["blurCS"] = CompileShader(
        L"resources/shaders/Blur.CS.hlsl",
        L"cs_6_0",
        L"CSMain",
        dxcUtils.Get(),
        dxcCompiler.Get(),
        includeHandler.Get()
    );
    assert(csBlobs["blurCS"] != nullptr);

    // MeshShader
    msBlobs["commonMS"] = CompileShader(
        L"resources/shaders/SimpleMS.hlsl",
        L"ms_6_5",
        L"main",
        dxcUtils.Get(),
        dxcCompiler.Get(),
        includeHandler.Get()
    );
    assert(vsBlobs["skinningMS"] != nullptr);

    msBlobs["skinningMS"] = CompileShader(
        L"resources/shaders/SimpleMS.hlsl",
        L"ms_6_5",
        L"main",
        dxcUtils.Get(),
        dxcCompiler.Get(),
        includeHandler.Get()
    );
    assert(vsBlobs["skinningMS"] != nullptr);
}


void DxManager::InitPSO(){
    /*==================================================================================*/
    //                              通常のパイプラインの初期化
    /*==================================================================================*/
    for(int blendMode = 0; blendMode < (int)BlendMode::kBlendModeCount; blendMode++){
        for(int cullMode = 0; cullMode < kCullModeCount; cullMode++){
            for(int topology = 0; topology < kTopologyCount; topology++){

                // 通常のパイプライン
                pipelines[blendMode][topology][cullMode].Initialize(
                    (BlendMode)blendMode,
                    D3D12_CULL_MODE(cullMode + 1)
                );

                // テンプレートのパラメーターを作成
                PSOManager::GenerateTemplateParameter(
                    &rootSignatures[blendMode][topology][cullMode],
                    &pipelines[blendMode][topology][cullMode],
                    PippelineType::Normal
                );

                // PSOの作成
                PSOManager::Create(
                    &rootSignatures[blendMode][topology][cullMode],
                    &pipelines[blendMode][topology][cullMode]
                );
            }
        }
    }

    /*==================================================================================*/
    //                           スキニング用のパイプラインの初期化
    /*==================================================================================*/
    for(int blendMode = 0; blendMode < (int)BlendMode::kBlendModeCount; blendMode++){
        for(int cullMode = 0; cullMode < kCullModeCount; cullMode++){
            // スキニング用のパイプライン
            skinningPipelines[blendMode][cullMode] = MSPipeline(
                (BlendMode)blendMode,
                D3D12_CULL_MODE(cullMode + 1)
            );

            // ルートシグネチャの初期化
            skinningRootSignatures[blendMode][cullMode] = RootSignature();

            // テンプレートのパラメーターを作成
            PSOManager::GenerateTemplateParameter(
                &skinningRootSignatures[blendMode][cullMode],
                &skinningPipelines[blendMode][cullMode],
                PippelineType::Skinning
            );

            // PSOの作成
            PSOManager::Create(
                &skinningRootSignatures[blendMode][cullMode],
                &skinningPipelines[blendMode][cullMode]
            );
        }
    }

    /*==================================================================================*/
    //                         ComputeShader用のパイプラインの初期化
    /*==================================================================================*/

    // ComputeShader用
    csRootSignature = PSOManager::SettingCSRootSignature();
    D3D12_COMPUTE_PIPELINE_STATE_DESC psoDesc = {};
    psoDesc.pRootSignature = csRootSignature.Get();
    psoDesc.CS = { reinterpret_cast<BYTE*>(csBlobs["blurCS"]->GetBufferPointer()), csBlobs["blurCS"]->GetBufferSize() };

    // CBVの初期化
    CS_ConstantBuffer = CreateBufferResource(device.Get(), sizeof(Blur_CS_ConstantBuffer));

    device->CreateComputePipelineState(&psoDesc, IID_PPV_ARGS(csPipelineState.GetAddressOf()));

}

void DxManager::SettingViewportAndScissor(float resolutionRate){

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


void DxManager::TransitionResourceState(ID3D12Resource* resource, D3D12_RESOURCE_STATES stateBefore, D3D12_RESOURCE_STATES stateAfter){
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

void DxManager::ClearViewSettings(){
    //////////////////////////////////////////////////////////////////
    // RTVのクリア
    //////////////////////////////////////////////////////////////////

    // オフスクリーンのRTVをクリアする
    clearColor = MyMath::FloatColor(SEED::GetWindowColor());
    commandList->ClearRenderTargetView(
        offScreenHandle,
        &clearColor.x, 0, nullptr
    );

    //////////////////////////////////////////////////////////////////
    // DSVとオフスクリーンのRTVを結びつける
    //////////////////////////////////////////////////////////////////
    commandList->OMSetRenderTargets(
        1, &offScreenHandle,
        false, &dsvHandle
    );

    //////////////////////////////////////////////////////////////////
    // フレームの最初にもっとも遠くにクリアする
    //////////////////////////////////////////////////////////////////
    commandList->ClearDepthStencilView(
        dsvHandle,
        D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr
    );
}

void DxManager::WaitForGPU(){
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

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                                                        //
//                                                      描画前の処理                                                        //
//                                                                                                                        //
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


void DxManager::PreDraw(){

    // フレームの初めに前フレームの値を保存
    SavePreVariable();

    /*--------------- TransitionBarrierを張る処理----------------*/

    // RTVを実行するために表示されていない後ろの画面(バックバッファ)の状態を描画状態に遷移させる
    TransitionResourceState(
        WindowManager::GetBackBuffer(pSEED_->windowTitle_),
        D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET
    );

#ifdef _DEBUG
    TransitionResourceState(
        WindowManager::GetBackBuffer(pSEED_->systemWindowTitle_),
        D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET
    );
#endif // _DEBUG

    /*----------画面、深度情報のクリア、DSVとRTVの結び直し-----------*/

    ClearViewSettings();

    /*----------------- SRVのヒープの再セット --------------------*/
    /*
        imGuiがフレーム単位でHeapの中身を操作するため
        SRVのHeapは毎フレームセットし直す
    */
    ID3D12DescriptorHeap* ppHeaps[] = { ViewManager::GetHeap(DESCRIPTOR_HEAP_TYPE::SRV_CBV_UAV) };
    commandList->SetDescriptorHeaps(1, ppHeaps);
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                                                        //
//                                             すべての描画命令を積む関数                                                     //
//                                                                                                                        //
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


void DxManager::DrawPolygonAll(){
#ifdef _DEBUG
    DrawGUI();
#endif // DEBUG

    //////////////////////////////////////////////////////////////////////////
    //  カメラ・ライティングデータの書き込み
    //////////////////////////////////////////////////////////////////////////



    //////////////////////////////////////////////////////////////////////////
    //  オフスクリーンに描画を行う
    //////////////////////////////////////////////////////////////////////////

    polygonManager_->DrawToOffscreen();

    //////////////////////////////////////////////////////////////////////////
    // ここで描画結果に対して処理を行う
    //////////////////////////////////////////////////////////////////////////

    //------------ 参照するリソースの状態を遷移させる--------------//

    TransitionResourceState(
        offScreenResource.Get(),
        D3D12_RESOURCE_STATE_RENDER_TARGET,
        D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE
    );

    TransitionResourceState(
        depthStencilResource.Get(),
        D3D12_RESOURCE_STATE_DEPTH_WRITE,
        D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE
    );


    /*----------------------ぼかした画面を作る-------------------*/

    effectManager_->TransfarToCS();

    //---------------------- 元の状態に遷移 ---------------------//

    TransitionResourceState(
        offScreenResource.Get(),
        D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE,
        D3D12_RESOURCE_STATE_RENDER_TARGET
    );

    TransitionResourceState(
        depthStencilResource.Get(),
        D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE,
        D3D12_RESOURCE_STATE_DEPTH_WRITE
    );


    //////////////////////////////////////////////////////////////////////////
    //  オフスクリーン → バックバッファに描画対象を切り変える
    //////////////////////////////////////////////////////////////////////////

    /*--------------- 切り替え ---------------*/
    commandList->OMSetRenderTargets(
        1, &WindowManager::GetRtvHandle(pSEED_->windowTitle_),
        false, &dsvHandle
    );

    /*----------バックバッファをクリアする----------*/
    commandList->ClearRenderTargetView(
        WindowManager::GetRtvHandle(pSEED_->windowTitle_),
        &clearColor.x, 0, nullptr
    );

#ifdef _DEBUG
    commandList->ClearRenderTargetView(
        WindowManager::GetRtvHandle(pSEED_->systemWindowTitle_),
        &clearColor.x, 0, nullptr
    );
#endif // _DEBUG

    /*--------------深度値のクリア---------------*/
    commandList->ClearDepthStencilView(
        dsvHandle,
        D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr
    );


    //////////////////////////////////////////////////////////////////
    // 各リソースの状態を遷移させる
    //////////////////////////////////////////////////////////////////

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
        depthTextureResource.Get(),
        D3D12_RESOURCE_STATE_UNORDERED_ACCESS,
        D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE
    );

    //////////////////////////////////////////////////////////////////
    //   最終結果を画面サイズの矩形に貼り付けて表示
    //////////////////////////////////////////////////////////////////


    polygonManager_->DrawToBackBuffer();


    //////////////////////////////////////////////////////////////////
    // すべてのリソースを基本の状態に戻す
    //////////////////////////////////////////////////////////////////

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

    /*---------- メインゲーム画面 -> ImGui用のSystem画面に描画を切り替え----------*/
#ifdef _DEBUG
    commandList->OMSetRenderTargets(
        1, &WindowManager::GetRtvHandle(pSEED_->systemWindowTitle_),
        false, nullptr
    );
#endif // _DEBUG
}

void DxManager::DrawGUI(){
#ifdef _DEBUG


#endif // _DEBUG
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                                                        //
//                                                      最終描画処理                                                        //
//                                                                                                                        //
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void DxManager::PostDraw(){

    // 画面に描く処理はすべて終わり、 画面に映すので、状態を遷移 ----------------------
    // 今回はRenderTargetからPresent にする
    TransitionResourceState(WindowManager::GetBackBuffer(pSEED_->windowTitle_), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);
#ifdef _DEBUG
    TransitionResourceState(WindowManager::GetBackBuffer(pSEED_->systemWindowTitle_), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);
#endif // _DEBUG

    /*----------------------------------------------------------*/

    // コマンドリストの内容を確定させる。 すべてのコマンドを積んでからCloseすること
    hr = commandList->Close();// コマンドリストの実行前に閉じる
    assert(SUCCEEDED(hr));

    // GPUにコマンドリストの実行を行わせる
    ID3D12CommandList* commandLists[] = { commandList.Get() };
    commandQueue->ExecuteCommandLists(1, commandLists);

    // GPUとOSに画面の交換を行うよう通知する
    WindowManager::Present(pSEED_->windowTitle_,0,0);
#ifdef _DEBUG
    WindowManager::Present(pSEED_->systemWindowTitle_, 0, 0);
#endif // _DEBUG

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


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                                                        //
//                                                 外部から実行される関数                                                     //
//                                                                                                                        //
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////
//           テクスチャを作成してグラフハンドルを返す関数          //
//////////////////////////////////////////////////////////////

uint32_t DxManager::CreateTexture(std::string filePath, const aiTexture* embeddedTexture){

    // 既にある場合
    if(ViewManager::GetTextureHandle(filePath) != -1){ return ViewManager::GetTextureHandle(filePath); }

    /*----------------------------- TextureResourceの作成,転送 -----------------------------*/

    // 読み込み
    DirectX::ScratchImage mipImages;
    if(!embeddedTexture){
        mipImages = LoadTextureImage(filePath);// 通常のテクスチャ
    } else{
        mipImages = LoadEmbeddedTextureImage(embeddedTexture);// 埋め込みテクスチャ
    }

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

    // キューブマップかどうか
    if(!metadata.IsCubemap()){
        srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;//2Dテクスチャ
        srvDesc.Texture2D.MipLevels = UINT(metadata.mipLevels);
    } else{
        srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURECUBE;//キューブマップ
        srvDesc.TextureCube.MipLevels = UINT_MAX;
        srvDesc.TextureCube.MostDetailedMip = 0;
        srvDesc.TextureCube.ResourceMinLODClamp = 0.0f;
    }

    // SRVの作成(グラフハンドルを返す)
    return ViewManager::CreateView(VIEW_TYPE::SRV, textureResource.back().Get(), &srvDesc, filePath);
}



//////////////////////////////////////////////////////////////
//           解像度を変更してRTVやScissorを設定し直す関数        ///
//////////////////////////////////////////////////////////////

void DxManager::ChangeResolutionRate(float resolutionRate){
    resolutionRate_ = std::clamp(resolutionRate, 0.0f, 1.0f);
    changeResolutionOrder = true;
}

void DxManager::ReCreateResolutionSettings(){
    SettingViewportAndScissor(resolutionRate_);// scissorなどを再設定

    // 命令フラグを下げる
    changeResolutionOrder = false;
}

void DxManager::SavePreVariable(){
    preResolutionRate_ = resolutionRate_;
}



/*===========================================================================================*/
/*                                          後処理                                            */
/*===========================================================================================*/

DxManager::~DxManager(){}

/*-------------------- 終了処理 -------------------*/
void DxManager::Finalize(){

    // 解放
    instance_->Release();

    // インスタンスの解放
    delete instance_;
    instance_ = nullptr;

    // ウィンドウの終了
    CloseWindow(WindowManager::GetHWND(SEED::GetInstance()->windowTitle_));

}

/*-------------------- ComObjectの解放 -------------------*/
void DxManager::Release(){

    // リソース
    for(auto& textureRs : textureResource){ textureRs.Reset(); }
    for(auto& intermediateRs : intermediateResource){ intermediateRs.Reset(); }
    WindowManager::Finalize();
    offScreenResource.Reset();
    depthStencilResource.Reset();
    depthTextureResource.Reset();
    blurTextureResource.Reset();
    CS_ConstantBuffer.Reset();
    delete polygonManager_;
    polygonManager_ = nullptr;

    // ディスクリプタヒープ
    ViewManager::Finalize();

    // ルートシグネチャ・パイプラインの解放
    csRootSignature.Reset();
    csPipelineState.Reset();

    for(int i = 0; i < (int)BlendMode::kBlendModeCount; i++){
        for(int j = 0; j < kTopologyCount; j++){
            for(int k = 0; k < kCullModeCount; k++){
                rootSignatures[i][j][k].Release();
                skinningRootSignatures[i][k].Release();
                pipelines[i][j][k].Release();
                skinningPipelines[i][k].Release();
            }
        }
    }

    // コンパイル系
    for(auto& vsBlob : vsBlobs){ vsBlob.second.Reset(); }
    for(auto& psBlob : psBlobs){ psBlob.second.Reset(); }
    for(auto& csBlob : csBlobs){ csBlob.second.Reset(); }
    dxcCompiler.Reset();
    dxcUtils.Reset();
    includeHandler.Reset();

    // フェンス
    CloseHandle(instance_->fenceEvent);
    fence.Reset();

    // コマンドリスト系
    commandList.Reset();
    commandAllocator.Reset();
    commandQueue.Reset();

    // デバイス
    device.Reset();
    useAdapter.Reset();
    dxgiFactory.Reset();

    // デバッグ系
#ifdef _DEBUG
    infoQueue.Reset();
    debugController.Reset();
#endif // _DEBUG
}

/*-------------------- インスタンスの取得 -------------------*/
DxManager* DxManager::GetInstance(){
    if(!instance_){
        instance_ = new DxManager();
    }

    return instance_;
}

/*-------------------- リークチェッカー -------------------*/
LeakChecker::~LeakChecker(){
    // 解放漏れがないかチェック
    ComPtr<IDXGIDebug1> debug;
    if(SUCCEEDED(DXGIGetDebugInterface1(0, IID_PPV_ARGS(&debug)))) {
        debug->ReportLiveObjects(DXGI_DEBUG_ALL, DXGI_DEBUG_RLO_ALL);
        debug->ReportLiveObjects(DXGI_DEBUG_APP, DXGI_DEBUG_RLO_ALL);
        debug->ReportLiveObjects(DXGI_DEBUG_D3D12, DXGI_DEBUG_RLO_ALL);
    }

    // COMの終了
    CoUninitialize();
}
