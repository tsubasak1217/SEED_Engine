#include <SEED/Source/Manager/DxManager/DxManager.h>
#include <thread>
#include <SEED/Source/SEED.h>
#include <Environment/Environment.h>
#include <SEED/Lib/Functions/MyFunc/MyMath.h>
#include <SEED/Lib/Structs/CS_Buffers.h>
#include <SEED/Source/Manager/InputManager/InputManager.h>
#include <SEED/Source/Manager/DxManager/PSO/PSOManager.h>
#include <SEED/Source/Manager/DxManager/PostEffect.h>

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
    SEED::instance_->pPolygonManager_ = instance_->polygonManager_;
    // PostEffectのinstance作成
    PostEffect::GetInstance();


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

    instance_->CreateRenderTargets();

    /*------------------- CPUとGPUの同期のための変数作成 ---------------------*/

    instance_->CreateFence();

    /*===========================================================================================*/
    /*                                ポリゴンを描画していくゾーン                                    */
    /*===========================================================================================*/


    /*---------------------- HLSLコンパイル --------------------------*/

    ShaderDictionary::GetInstance()->Initialize();
    ShaderDictionary::GetInstance()->LoadFromDirectory(
        "resources/shaders/",
        instance_->device.Get()
    );

    /*--------------------初期化時読み込みリソ－ス--------------------*/

    instance_->StartUpload();

    /*------------------------PSOの生成-----------------------*/

    instance_->InitPSO();

    /*--------------------------------- VewportとScissor ---------------------------------*/

    instance_->SettingViewportAndScissor(instance_->resolutionRate_);

    // 初期化時のものをデフォルト値として保存
    instance_->viewport_default = instance_->viewport;
    instance_->scissorRect_default = instance_->scissorRect;

    // ------------------------------------------------------------------------------------


    // 情報がそろったのでpolygonManagerの初期化
    instance_->polygonManager_->InitResources();
}


/*===========================================================================================*/
/*                                      初期設定を行う関数                                      */
/*===========================================================================================*/

void DxManager::CreateDebugLayer(){
    if(SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController)))){
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
        ){

        // アダプタの情報を取得する
        DXGI_ADAPTER_DESC3 adapterDesc{};
        hr = useAdapter->GetDesc3(&adapterDesc);

        // 取得できない場合アサート
        assert(SUCCEEDED(hr));

        // ソフトウェアアダプタでなければ採用
        if(adapterDesc.Flags != DXGI_ADAPTER_FLAG3_SOFTWARE){

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

    for(size_t i = 0; i < _countof(featureLevels); ++i){

        // 先ほど決定したアダプタを使用してデバイスを生成
        hr = D3D12CreateDevice(useAdapter.Get(), featureLevels[i], IID_PPV_ARGS(device.GetAddressOf()));

        // 生成に成功したらログを出力してループを終了
        if(SUCCEEDED(hr)){
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

    if(SUCCEEDED(device->QueryInterface(IID_PPV_ARGS(&infoQueue)))){
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
#ifdef USE_SUB_WINDOW
    WindowManager::GetWindow(pSEED_->systemWindowTitle_)->CreateSwapChain(dxgiFactory.Get(), commandQueue.Get());
#endif // USE_SUB_WINDOW
}


void DxManager::StartUpload(){
    // システムで使用するものは先にテクスチャを作成しておく
    TextureManager::GetInstance();
    TextureManager::LoadTexture("DefaultAssets/white1x1.png");
    TextureManager::LoadTexture("DefaultAssets/uvChecker.png");

#ifdef _DEBUG
    // エンジン用リソースを自動で読み込む
    std::string directory = "../../SEED/EngineResources/Textures/";

    // resources/textures/ParticleTextures/ 以下の階層にあるテクスチャを自動で読む
    std::vector<std::string> fileNames;

    // 指定されたディレクトリ内のすべてのファイルを探索
    for(const auto& entry : std::filesystem::directory_iterator("SEED/EngineResources/Textures/")){
        if(entry.is_regular_file()){ // 通常のファイルのみ取得（ディレクトリを除外）
            // もしファイル名が".png"で終わっていたら
            if(entry.path().extension() == ".png"){
                // ファイル名を追加
                fileNames.push_back(directory + entry.path().filename().string()); // ファイル名のみ追加
            }
        }
    }

    // テクスチャの読み込み
    for(const auto& fileName : fileNames){
        // テクスチャの読み込み
        TextureManager::LoadTexture(fileName);
    }

#endif // _DEBUG
}

//================================ オフスクリーンの初期化 ================================//
void DxManager::CreateRenderResource(const std::string& cameraName){
    // オフスクリーンリソースがすでに存在する場合は何もしない
    if(offScreenResources.find(cameraName) != offScreenResources.end()){
        return;
    }
    {
        // レンダーターゲットとして作成
        DxResource& offScreenResource = offScreenResources[cameraName];
        std::string resourceName = "offScreen_" + cameraName;
        offScreenResource.resource = CreateRenderTargetTextureResource(device.Get(), pSEED_->kClientWidth_, pSEED_->kClientHeight_);
        offScreenResource.resource.Get()->SetName(MyFunc::ConvertString(resourceName).c_str());
        offScreenResource.InitState(D3D12_RESOURCE_STATE_RENDER_TARGET);

        // RTVの設定
        D3D12_RENDER_TARGET_VIEW_DESC rtvDesc{};
        rtvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB; // 出力結果をSRGBに変換して書き込むように設定
        rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D; // 2dテクスチャとして書き込むように設定

        // Texture用SRVの作成
        D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
        srvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
        srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
        srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D; // 2Dテクスチャ
        srvDesc.Texture2D.MipLevels = 1;

        // Viewの生成
        ViewManager::CreateView(VIEW_TYPE::RTV, offScreenResource.resource.Get(), &rtvDesc, resourceName);
        ViewManager::CreateView(VIEW_TYPE::SRV, offScreenResource.resource.Get(), &srvDesc, resourceName);

        // ハンドル取得
        offScreenHandles[cameraName] = ViewManager::GetHandleCPU(HEAP_TYPE::RTV, resourceName);
        offScreenNames[cameraName] = resourceName;
    }
    {
        // DepthStencil用のテクスチャとして作成
        DxResource& depthStencilResource = depthStencilResources[cameraName];
        std::string resourceName = "depthStencil_" + cameraName;
        depthStencilResource.resource = CreateDepthStencilTextureResource(device.Get(), pSEED_->kClientWidth_, pSEED_->kClientHeight_);
        depthStencilResource.resource->SetName(MyFunc::ConvertString(resourceName).c_str());
        depthStencilResource.InitState(D3D12_RESOURCE_STATE_DEPTH_WRITE);

        // DSVの設定
        D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc{};
        dsvDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
        dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;

        // depthStencilを参照する時用のSRV
        D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
        srvDesc.Format = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
        srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
        srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D; // 2Dテクスチャ
        srvDesc.Texture2D.MipLevels = 1;

        // Viewの生成
        ViewManager::CreateView(VIEW_TYPE::DSV, depthStencilResource.resource.Get(), &dsvDesc, resourceName);
        ViewManager::CreateView(VIEW_TYPE::SRV, depthStencilResource.resource.Get(), &srvDesc, resourceName);

        // ハンドルを得る
        dsvHandles[cameraName] = ViewManager::GetHandleCPU(HEAP_TYPE::DSV, resourceName);
        depthStencilNames[cameraName] = resourceName;
    }
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


void DxManager::InitPSO(){
    /*==================================================================================*/
    //                              通常のパイプラインの初期化
    /*==================================================================================*/

    PSOManager::CreatePipelines("CommonVSPipeline.pip");// VSのパイプライン。ふつうのやつ
    PSOManager::CreatePipelines("SkinningVSPipeline.pip");// VSのパイプライン。スキニング用
    PSOManager::CreatePipelines("TextVSPipeline.pip");// テキストのパイプライン。

    /*==================================================================================*/
    //                              CSのパイプラインの初期化
    /*==================================================================================*/

    // 内部でPSOManager::CreatePipelinesを呼び出している
    PostEffect::GetInstance()->Initialize();

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

    for(const auto& [cameraName, offScreenHandle] : offScreenHandles){
        // オフスクリーンのRTVをクリアする
        clearColor = MyMath::FloatColor(SEED::GetWindowColor());
        commandList->ClearRenderTargetView(
            offScreenHandle,
            &clearColor.x, 0, nullptr
        );

        // フレームの最初にもっとも遠くにクリアする
        commandList->ClearDepthStencilView(
            dsvHandles[cameraName],
            D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr
        );
    }
}

void DxManager::WaitForGPU(){
    // Fenceの値を更新
    fenceValue++;
    // GPUがここまでたどり着いたときに、 Fenceの値を指定した値に代入するようにSignalを送る
    commandQueue->Signal(fence.Get(), fenceValue);

    // Fenceの値が指定したSignal値にたどり着いているか確認する
    // GetCompletedValueの初期値はFence作成時に渡した初期値
    if(fence->GetCompletedValue() < fenceValue){

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

#ifdef USE_SUB_WINDOW
    TransitionResourceState(
        WindowManager::GetBackBuffer(pSEED_->systemWindowTitle_),
        D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET
    );
#endif // USE_SUB_WINDOW

    /*----------画面、深度情報のクリア、DSVとRTVの結び直し-----------*/

    ClearViewSettings();

    /*----------------- SRVのヒープの再セット --------------------*/
    /*
        imGuiがフレーム単位でHeapの中身を操作するため
        SRVのHeapは毎フレームセットし直す
    */
    ID3D12DescriptorHeap* ppHeaps[] = { ViewManager::GetHeap(HEAP_TYPE::SRV_CBV_UAV) };
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
#ifdef USE_SUB_WINDOW
    DrawGUI();
#endif // USE_SUB_WINDOW

    //////////////////////////////////////////////////////////////////////////
    //  オフスクリーンに描画を行う
    //////////////////////////////////////////////////////////////////////////
    for(const auto& [cameraName, camera] : cameras_){
        if(!camera->isActive_){ continue; }

        // レンダーターゲットを設定
        commandList->OMSetRenderTargets(
            1, &offScreenHandles[cameraName],
            false, &dsvHandles[cameraName]
        );

        // オフスクリーンに描画
        polygonManager_->BindCameraDatas(cameraName);
        polygonManager_->DrawToOffscreen(cameraName);
    }

    //////////////////////////////////////////////////////////////////////////
    // ここで描画結果に対して処理を行う(ポストエフェクト。メインのカメラにのみ)
    //////////////////////////////////////////////////////////////////////////

    //------------ 参照するリソースの状態を遷移させる--------------//

    for(const auto& [cameraName, camera] : cameras_){
        // オフスクリーンのリソースを参照するために状態を遷移させる
        offScreenResources[cameraName].TransitionState(D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE);
        depthStencilResources[cameraName].TransitionState(D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE);
    }

    /*----------------------ポストエフェクトを行う-------------------*/

    // ポストエフェクト用のパラメーターを更新
    PostEffect::Update();

    // 有効なポストエフェクトを適用
    PostEffect::GetInstance()->PostProcess();

    //---------------------- 元の状態に遷移 ---------------------//

    for(const auto& [cameraName, camera] : cameras_){
        // オフスクリーンのリソースを参照するために状態を遷移させる
        offScreenResources[cameraName].TransitionState(D3D12_RESOURCE_STATE_RENDER_TARGET);
        depthStencilResources[cameraName].TransitionState(D3D12_RESOURCE_STATE_DEPTH_WRITE);
    }

    //////////////////////////////////////////////////////////////////////////
    // 最終結果の描画
    //////////////////////////////////////////////////////////////////////////

    /*--------------- 切り替え ---------------*/
    commandList->OMSetRenderTargets(
        1, &WindowManager::GetRtvHandle(pSEED_->windowTitle_),
        false, &dsvHandles[mainCameraName_]
    );

    /*----------バックバッファをクリアする----------*/
    commandList->ClearRenderTargetView(
        WindowManager::GetRtvHandle(pSEED_->windowTitle_),
        &clearColor.x, 0, nullptr
    );

#ifdef USE_SUB_WINDOW
    commandList->ClearRenderTargetView(
        WindowManager::GetRtvHandle(pSEED_->systemWindowTitle_),
        &clearColor.x, 0, nullptr
    );
#endif // USE_SUB_WINDOW

    /*--------------深度値のクリア---------------*/
    commandList->ClearDepthStencilView(
        dsvHandles[mainCameraName_],
        D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr
    );


    //////////////////////////////////////////////////////////////////
    // 各リソースの状態を遷移させる
    //////////////////////////////////////////////////////////////////


    //PostEffect::GetInstance()->EndTransition();

    // オフスクリーンのリソースを参照するために状態を遷移させる
    for(const auto& [cameraName, camera] : cameras_){
        offScreenResources[cameraName].TransitionState(D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
    }


    //////////////////////////////////////////////////////////////////
    //   最終結果を画面サイズの矩形に貼り付けて表示
    //////////////////////////////////////////////////////////////////


    polygonManager_->DrawToBackBuffer();


    //////////////////////////////////////////////////////////////////
    // すべてのリソースを基本の状態に戻す
    //////////////////////////////////////////////////////////////////

    /*---------- メインゲーム画面 -> ImGui用のSystem画面に描画を切り替え----------*/
#ifdef USE_SUB_WINDOW
    commandList->OMSetRenderTargets(
        1, &WindowManager::GetRtvHandle(pSEED_->systemWindowTitle_),
        false, nullptr
    );
#endif // USE_SUB_WINDOW
}

void DxManager::DrawGUI(){
#ifdef USE_SUB_WINDOW

    PostEffect::instance_->Edit();

#endif // USE_SUB_WINDOW
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
#ifdef USE_SUB_WINDOW
    TransitionResourceState(WindowManager::GetBackBuffer(pSEED_->systemWindowTitle_), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);
#endif // USE_SUB_WINDOW

    // オフスクリーンのリソースもRenderTargetに戻す
    for(const auto& [cameraName, camera] : cameras_){
        // オフスクリーンのリソースを参照するために状態を遷移させる
        offScreenResources[cameraName].TransitionState(D3D12_RESOURCE_STATE_RENDER_TARGET);
    }

    /*----------------------------------------------------------*/

    // コマンドリストの内容を確定させる。 すべてのコマンドを積んでからCloseすること
    hr = commandList->Close();// コマンドリストの実行前に閉じる
    assert(SUCCEEDED(hr));

    // GPUにコマンドリストの実行を行わせる
    ID3D12CommandList* commandLists[] = { commandList.Get() };
    commandQueue->ExecuteCommandLists(1, commandLists);

    // GPUとOSに画面の交換を行うよう通知する
    WindowManager::Present(pSEED_->windowTitle_, 0, 0);
#ifdef USE_SUB_WINDOW
    WindowManager::Present(pSEED_->systemWindowTitle_, 0, 0);
#endif // USE_SUB_WINDOW

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
    TextureManager::Release();
    TextSystem::Release();
    WindowManager::Finalize();
    for(auto& [cameraName, camera] : cameras_){
        offScreenResources[cameraName].resource.Reset();
        depthStencilResources[cameraName].resource.Reset();
    }
    PostEffect::GetInstance()->Release();
    delete polygonManager_;
    polygonManager_ = nullptr;

    // ディスクリプタヒープ
    ViewManager::Finalize();

    // ルートシグネチャ・パイプライン・シェーダ情報解放
    ShaderDictionary::GetInstance()->Release();
    PSOManager::GetInstance()->Release();

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
    if(SUCCEEDED(DXGIGetDebugInterface1(0, IID_PPV_ARGS(&debug)))){
        debug->ReportLiveObjects(DXGI_DEBUG_ALL, DXGI_DEBUG_RLO_ALL);
        debug->ReportLiveObjects(DXGI_DEBUG_APP, DXGI_DEBUG_RLO_ALL);
        debug->ReportLiveObjects(DXGI_DEBUG_D3D12, DXGI_DEBUG_RLO_ALL);
    }

    // COMの終了
    CoUninitialize();
}
