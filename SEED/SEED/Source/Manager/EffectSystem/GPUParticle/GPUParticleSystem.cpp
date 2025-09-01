#include "GPUParticleSystem.h"
#include <SEED/Source/Manager/DxManager/PSO/PSOManager.h>
#include <SEED/Source/Manager/DxManager/DxManager.h>
#include <SEED/Source/Manager/ClockManager/ClockManager.h>
#include <SEED/Source/SEED.h>

//////////////////////////////////////////////////////////////////////////
// インスタンスの取得
//////////////////////////////////////////////////////////////////////////
GPUParticleSystem* GPUParticleSystem::GetInstance(){
    if(!instance_){
        instance_ = new GPUParticleSystem();
    }
    return instance_;
}


//////////////////////////////////////////////////////////////////////////
// 初期化
//////////////////////////////////////////////////////////////////////////
void GPUParticleSystem::Initialize(){
    // インスタンスを取得
    GetInstance();
    // パイプラインの初期化
    instance_->InitPSO();
    // リソースの作成
    instance_->CreateResources();
    // 情報のバインド
    instance_->BindInfo();
    // 初期化CSの実行
    int32_t dispatchX = (instance_->maxParticleCount_ + 1023) / 1024;
    instance_->Dispatch("GPUParticle/GPUParticleInitCS.pip", dispatchX, 1);
    // モデルの初期化
    //instance_->currentModelData_ = ModelManager::GetInstance()->GetModelData("DefaultAssets/plane/plane.obj");
    instance_->currentModelData_ = ModelManager::GetInstance()->GetModelData("DefaultAssets/cube/cube.obj");
}


//////////////////////////////////////////////////////////////////////////
// パイプライン初期化
//////////////////////////////////////////////////////////////////////////
void GPUParticleSystem::InitPSO(){
    // パイプラインの初期化
    PSOManager::CreatePipelines("GPUParticle/GPUParticleInitCS.pip"); // パーティクルの初期化パイプライン
    PSOManager::CreatePipelines("GPUParticle/EmitterUpdateCS.pip"); // パーティクルのパイプライン
    PSOManager::CreatePipelines("GPUParticle/ParticleUpdateCS.pip"); // エミッターのパイプライン
    PSOManager::CreatePipelines("GPUParticle/GPUParticleVS.pip"); // パーティクルの描画パイプライン
}

//////////////////////////////////////////////////////////////////////////
// リソースやViewの生成
//////////////////////////////////////////////////////////////////////////
void GPUParticleSystem::CreateResources(){

    {// エミッターのバッファ作成

        // バッファリソースの作成(エミッターはUPLOADヒープに作成)
        emitterBuffer_.CreateBuffer(
            DxManager::instance_->GetDevice(), sizeof(GPUEmitter),
            D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_FLAG_NONE
        );

        // Mapする
        emitterBuffer_.Map();

        // emitterを初期化
        auto& emitter = instance_->emitter_;
        emitter.position = Vector3(0.0f, 0.0f, 0.0f);
        emitter.emitRange = Vector3(3.0f, 3.0f, 3.0f);
        emitter.minScale = Vector3(0.1f, 0.1f, 0.1f);
        emitter.maxScale = Vector3(1.0f, 1.0f, 1.0f);
        emitter.minRotation = 0.0f;
        emitter.maxRotation = 0.0f;
        emitter.minRotateSpeed = -10.0f;
        emitter.maxRotateSpeed = 10.0f;
        emitter.baseDirection = Vector3(0.0f, 1.0f, 0.0f);
        emitter.angleRange = 1.0f;
        emitter.minSpeed = 10.0f;
        emitter.maxSpeed = 10.0f;
        emitter.color = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
        emitter.minLifeTime = 1.0f;
        emitter.maxLifeTime = 2.0f;
        emitter.interval = 1.0f / 60.0f;
        emitter.currentTime = 0.0f;
        emitter.numEmitEvery = 50;
        emitter.emit = false;
        emitter.loop = true;
        emitter.alive = true;
        //emitter.textureIndex = TextureManager::LoadTexture("ParticleTextures/particle.png");
        emitter.textureIndex = TextureManager::LoadTexture("ParticleTextures/white.png");
    }


    {// パーティクルのバッファ作成

        // バッファリソースの作成
        particleBuffer_.CreateBuffer(
            DxManager::instance_->GetDevice(), sizeof(GPUParticle) * instance_->maxParticleCount_,
            D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS
        );

        // UAVとして初期化
        particleBuffer_.bufferResource.InitState(D3D12_RESOURCE_STATE_UNORDERED_ACCESS);


        // SRVのdescの設定
        auto& srvDesc = particleBuffer_.GetSRVDesc();
        srvDesc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
        srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
        srvDesc.Buffer.NumElements = instance_->maxParticleCount_;
        srvDesc.Format = DXGI_FORMAT_UNKNOWN;
        srvDesc.Buffer.StructureByteStride = sizeof(GPUParticle);

        // UAVのdescの設定
        auto& uavDesc = particleBuffer_.GetUAVDesc();
        uavDesc.ViewDimension = D3D12_UAV_DIMENSION_BUFFER;
        uavDesc.Buffer.NumElements = instance_->maxParticleCount_;
        uavDesc.Format = DXGI_FORMAT_UNKNOWN;
        uavDesc.Buffer.StructureByteStride = sizeof(GPUParticle);

        // Viewを作成
        particleBuffer_.CreateSRV("ParticleBuffer_SRV");
        particleBuffer_.CreateUAV("ParticleBuffer_UAV");
    }


    {// フリーリストのバッファ作成

        // バッファリソースを作成
        particleFreeListBuffer_.CreateBuffer(
            DxManager::instance_->GetDevice(), sizeof(uint32_t) * instance_->maxParticleCount_,
            D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS
        );

        // UAVとして初期化
        particleFreeListBuffer_.bufferResource.InitState(D3D12_RESOURCE_STATE_UNORDERED_ACCESS);

        // UAVのdescの設定
        auto& uavDesc = particleFreeListBuffer_.GetUAVDesc();
        uavDesc.ViewDimension = D3D12_UAV_DIMENSION_BUFFER;
        uavDesc.Buffer.NumElements = instance_->maxParticleCount_;
        uavDesc.Format = DXGI_FORMAT_UNKNOWN;
        uavDesc.Buffer.StructureByteStride = sizeof(uint32_t);

        auto& srvDesc = particleFreeListBuffer_.GetSRVDesc();
        srvDesc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
        srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
        srvDesc.Buffer.NumElements = instance_->maxParticleCount_;
        srvDesc.Format = DXGI_FORMAT_UNKNOWN;
        srvDesc.Buffer.StructureByteStride = sizeof(uint32_t);

        // Viewを作成
        particleFreeListBuffer_.CreateUAV("ParticleFreeListBuffer_UAV");
        particleFreeListBuffer_.CreateSRV("ParticleFreeListBuffer_SRV");
    }

    {// フリーリストのインデックスバッファ作成

        // バッファリソースを作成
        particleFreeListIndexBuffer_.CreateBuffer(
            DxManager::instance_->GetDevice(), sizeof(int32_t),
            D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS
        );

        // UAVとして初期化
        particleFreeListIndexBuffer_.bufferResource.InitState(D3D12_RESOURCE_STATE_UNORDERED_ACCESS);

        // UAVのdescの設定
        auto& uavDesc = particleFreeListIndexBuffer_.GetUAVDesc();
        uavDesc.ViewDimension = D3D12_UAV_DIMENSION_BUFFER;
        uavDesc.Buffer.NumElements = 1;
        uavDesc.Format = DXGI_FORMAT_UNKNOWN;
        uavDesc.Buffer.StructureByteStride = sizeof(int32_t);

        // SRVのdescの設定
        auto& srvDesc = particleFreeListIndexBuffer_.GetSRVDesc();
        srvDesc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
        srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
        srvDesc.Buffer.NumElements = 1;
        srvDesc.Format = DXGI_FORMAT_UNKNOWN;
        srvDesc.Buffer.StructureByteStride = sizeof(int32_t);

        // Viewを作成
        particleFreeListIndexBuffer_.CreateUAV("ParticleFreeListIndexBuffer_UAV");
        particleFreeListIndexBuffer_.CreateSRV("ParticleFreeListIndexBuffer_SRV");
    }

    {// パーティクルのカウント用のバッファの初期化
        // バッファリソースを作成
        particleCountBuffer_.CreateBuffer(
            DxManager::instance_->GetDevice(), sizeof(int32_t),
            D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS
        );

        // UAVとして初期化
        particleCountBuffer_.InitState(D3D12_RESOURCE_STATE_UNORDERED_ACCESS);

        // UAVのdescの設定
        auto& uavDesc = particleCountBuffer_.GetUAVDesc();
        uavDesc.ViewDimension = D3D12_UAV_DIMENSION_BUFFER;
        uavDesc.Buffer.NumElements = 1;
        uavDesc.Format = DXGI_FORMAT_UNKNOWN;
        uavDesc.Buffer.StructureByteStride = sizeof(int32_t);

        // Viewを作成
        particleCountBuffer_.CreateUAV("particleCountBuffer_UAV");
    }

    {// カメラ情報のバッファ作成
        // バッファリソースを作成
        cameraInfoBuffer_.CreateBuffer(
            DxManager::instance_->GetDevice(), sizeof(GPUCameraInfo),
            D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_FLAG_NONE
        );
        // Mapする
        cameraInfoBuffer_.Map();
    }

    {// 頂点バッファの作成
        // 頂点バッファのリソースを作成
        vertexBuffer_.CreateBuffer(
            DxManager::instance_->GetDevice(), sizeof(VertexData) * maxVertexCount_,
            D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_FLAG_NONE
        );
        // Mapする
        vertexBuffer_.Map();
    }

    {// インデックスバッファの作成
        // インデックスバッファのリソースを作成
        indexBuffer_.CreateBuffer(
            DxManager::instance_->GetDevice(), sizeof(uint32_t) * maxVertexCount_,
            D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_FLAG_NONE
        );
        // Mapする
        indexBuffer_.Map();
    }
}


//////////////////////////////////////////////////////////////////////////
// 情報のバインド
//////////////////////////////////////////////////////////////////////////
void GPUParticleSystem::BindInfo(){

    // GPUParticleInitCS
    PSOManager::SetBindInfo("GPUParticle/GPUParticleInitCS.pip", "gParticles", particleBuffer_.GetUAVHandle());
    PSOManager::SetBindInfo("GPUParticle/GPUParticleInitCS.pip", "gFreeList", particleFreeListBuffer_.GetUAVHandle());
    PSOManager::SetBindInfo("GPUParticle/GPUParticleInitCS.pip", "gFreeListIndex", particleFreeListIndexBuffer_.GetUAVHandle());
    PSOManager::SetBindInfo("GPUParticle/GPUParticleInitCS.pip", "gMaxParticleCount", &maxParticleCount_);

    // EmitterUpdateCS
    PSOManager::SetBindInfo("GPUParticle/EmitterUpdateCS.pip", "gEmitter", emitterBuffer_.GetGPUVirtualAddress());
    PSOManager::SetBindInfo("GPUParticle/EmitterUpdateCS.pip", "gParticles", particleBuffer_.GetUAVHandle());
    PSOManager::SetBindInfo("GPUParticle/EmitterUpdateCS.pip", "gFreeListIndex", particleFreeListIndexBuffer_.GetUAVHandle());
    PSOManager::SetBindInfo("GPUParticle/EmitterUpdateCS.pip", "gFreeList", particleFreeListBuffer_.GetSRVHandle());
    PSOManager::SetBindInfo("GPUParticle/EmitterUpdateCS.pip", "gParticleCount", particleCountBuffer_.GetUAVHandle());
    PSOManager::SetBindInfo("GPUParticle/EmitterUpdateCS.pip", "gDeltaTime", &deltaTime_);
    PSOManager::SetBindInfo("GPUParticle/EmitterUpdateCS.pip", "gTotalTime", &totalTime_);
    PSOManager::SetBindInfo("GPUParticle/EmitterUpdateCS.pip", "gMaxParticleCount", &maxParticleCount_);

    // ParticleUpdateCS
    PSOManager::SetBindInfo("GPUParticle/ParticleUpdateCS.pip", "gParticles", particleBuffer_.GetUAVHandle());
    PSOManager::SetBindInfo("GPUParticle/ParticleUpdateCS.pip", "gDeltaTime", &deltaTime_);
    PSOManager::SetBindInfo("GPUParticle/ParticleUpdateCS.pip", "gFreeList", particleFreeListBuffer_.GetUAVHandle());
    PSOManager::SetBindInfo("GPUParticle/ParticleUpdateCS.pip", "gFreeListIndex", particleFreeListIndexBuffer_.GetUAVHandle());
    PSOManager::SetBindInfo("GPUParticle/ParticleUpdateCS.pip", "gParticleCount", particleCountBuffer_.GetUAVHandle());
    PSOManager::SetBindInfo("GPUParticle/ParticleUpdateCS.pip", "gMaxParticleCount", &maxParticleCount_);

    // GPUParticleVS
    PSOManager::SetBindInfo("GPUParticle/GPUParticleVS.pip", "particles", particleBuffer_.GetSRVHandle());
    PSOManager::SetBindInfo("GPUParticle/GPUParticleVS.pip", "cameraInfo", cameraInfoBuffer_.GetGPUVirtualAddress());
    PSOManager::SetBindInfo("GPUParticle/GPUParticleVS.pip", "gTexture", ViewManager::GetHandleGPU(HEAP_TYPE::SRV_CBV_UAV, 0));

    // descriptorHeapのセット
    ID3D12DescriptorHeap* ppHeaps[] = { ViewManager::GetHeap(HEAP_TYPE::SRV_CBV_UAV) };
    DxManager::instance_->commandList->SetDescriptorHeaps(1, ppHeaps);

}


//////////////////////////////////////////////////////////////////////////
// フレームごとのバインド
//////////////////////////////////////////////////////////////////////////
void GPUParticleSystem::BindPerFrame(){
}


//////////////////////////////////////////////////////////////////////////
// 実行処理
//////////////////////////////////////////////////////////////////////////
void GPUParticleSystem::Dispatch(const std::string& pipelineName, int32_t dispatchX, int32_t dispatchY){

    // PSOの切り替え
    auto* pso = PSOManager::GetPSO_Compute(pipelineName);
    auto& commandList = DxManager::GetInstance()->commandList;
    commandList->SetComputeRootSignature(pso->rootSignature.get()->rootSignature.Get());
    commandList->SetPipelineState(pso->pipeline.get()->pipeline_.Get());

    // リソースのバインド
    pso->rootSignature->BindAll(commandList.Get(), true);

    // 実行
    commandList->Dispatch(dispatchX, dispatchY, 1);
}


//////////////////////////////////////////////////////////////////////////
// 更新
//////////////////////////////////////////////////////////////////////////
void GPUParticleSystem::Update(){

    // エミッターの時間を更新-----------------------------------------------

    auto& emitter = instance_->emitter_;
    emitter.currentTime += ClockManager::DeltaTime();

    if(emitter.currentTime >= emitter.interval){
        emitter.emit = true; // エミットフラグを立てる
    }



    // hlsl用の変数やバッファの更新------------------------------------------

    // 時間の更新
    instance_->deltaTime_ = ClockManager::DeltaTime();
    instance_->totalTime_ += instance_->deltaTime_;

    // 頂点情報、インデックス情報を書き込む
    if(instance_->currentModelData_){
        int vertexCount = 0;
        int indexCount = 0;

        for(auto& mesh : instance_->currentModelData_->meshes){
            // 頂点情報の書き込み
            std::memcpy(
                instance_->vertexBuffer_.data + vertexCount,
                mesh.vertices.data(),
                sizeof(VertexData) * mesh.vertices.size()
            );

            // インデックス情報の書き込み
            std::memcpy(
                instance_->indexBuffer_.data + indexCount,
                mesh.indices.data(),
                sizeof(uint32_t) * mesh.indices.size()
            );

            // 頂点数とインデックス数を更新
            vertexCount += (int32_t)mesh.vertices.size();
            indexCount += (int32_t)mesh.indices.size();
        }
    }

    // CSのDispatch処理--------------------------------------------------------

    // UAVの状態に変更
    instance_->particleBuffer_.bufferResource.TransitionState(
        D3D12_RESOURCE_STATE_UNORDERED_ACCESS
    );

    // SRVの状態に変更
    instance_->particleFreeListBuffer_.bufferResource.TransitionState(
        D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE
    );


    // エミットフラグが立っている時だけ
    if(emitter.emit){

        // エミッターの情報をリソースに書き込む
        auto* data = instance_->emitterBuffer_.data;
        std::memcpy(
            data,
            &emitter,
            sizeof(GPUEmitter)
        );

        // エミッターのフラグや時間を元に戻す
        if(emitter.loop){
            emitter.currentTime -= emitter.interval;
            emitter.emit = false;
        } else{
            emitter.alive = false;
            emitter.emit = false;
        }

        // dispatch
        int32_t dispatchX = (emitter.numEmitEvery + 1023) / 1024;
        instance_->Dispatch("GPUParticle/EmitterUpdateCS.pip", dispatchX, 1);
    }


    // UAVの状態に変更
    instance_->particleFreeListBuffer_.bufferResource.TransitionState(
        D3D12_RESOURCE_STATE_UNORDERED_ACCESS
    );

    // パーティクルの更新
    int32_t dispatchX = (instance_->maxParticleCount_ + 1023) / 1024;
    if(dispatchX != 0){
        instance_->Dispatch("GPUParticle/ParticleUpdateCS.pip", dispatchX, 1);
    }

    // コピーする
    instance_->particleCountBuffer_.CopyToReadbackBuffer(DxManager::instance_->commandList.Get());
}


//////////////////////////////////////////////////////////////////////////
// 描画命令発行
//////////////////////////////////////////////////////////////////////////
void GPUParticleSystem::Draw(const std::string& cameraName){

    if(instance_->currentModelData_ == nullptr){
        // モデルデータが設定されていない場合は何もしない
        return;
    }

    // カメラ情報を書き込む
    auto* data = instance_->cameraInfoBuffer_.data;
    auto* camera = SEED::GetCamera(cameraName);
    data->viewProjectionMatrix = camera->GetViewProjectionMat();
    data->cameraRotation = camera->GetRotation();
    data->cameraPosition = camera->GetTranslation();

    // パイプラインの設定
    auto* pso = PSOManager::GetPSO("GPUParticle/GPUParticleVS.pip", BlendMode::ADD, D3D12_CULL_MODE_BACK, PolygonTopology::TRIANGLE);
    auto& commandList = DxManager::GetInstance()->commandList;
    commandList->SetGraphicsRootSignature(pso->rootSignature.get()->rootSignature.Get());
    commandList->SetPipelineState(pso->pipeline.get()->pipeline_.Get());

    // リソースのバインド
    pso->rootSignature->BindAll(commandList.Get());

    // 形状の指定
    commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    // リソースの状態を変更
    instance_->particleBuffer_.bufferResource.TransitionState(
        D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE
    );

    // vbv,ibvの設定
    int vertexCount = 0;
    int indexCount = 0;
    for(const auto& mesh : instance_->currentModelData_->meshes){

        instance_->vbv_vertex.BufferLocation = instance_->vertexBuffer_.bufferResource.resource->GetGPUVirtualAddress() + vertexCount * sizeof(VertexData);
        instance_->vbv_vertex.StrideInBytes = sizeof(VertexData);
        instance_->vbv_vertex.SizeInBytes = sizeof(VertexData) * (uint32_t)mesh.vertices.size();
        commandList->IASetVertexBuffers(0, 1, &instance_->vbv_vertex);

        instance_->ibv.BufferLocation = instance_->indexBuffer_.bufferResource.resource->GetGPUVirtualAddress() + indexCount * sizeof(uint32_t);
        instance_->ibv.Format = DXGI_FORMAT_R32_UINT;
        instance_->ibv.SizeInBytes = sizeof(uint32_t) * (uint32_t)mesh.indices.size();
        commandList->IASetIndexBuffer(&instance_->ibv);

        // 一斉描画
        auto* pDxManager_ = DxManager::GetInstance();
        pDxManager_->commandList->DrawIndexedInstanced(
            (uint32_t)mesh.indices.size(),
            instance_->maxParticleCount_,
            0,
            0,
            0
        );

        // 頂点数とインデックス数を更新
        vertexCount += (int32_t)mesh.vertices.size();
        indexCount += (int32_t)mesh.indices.size();
    }
}


//////////////////////////////////////////////////////////////////////////
// 解放
//////////////////////////////////////////////////////////////////////////
void GPUParticleSystem::Release(){

    if(instance_ == nullptr){
        return; // 既に解放されている場合は何もしない
    }

    // インスタンスの解放
    instance_->emitterBuffer_.Release();
    instance_->particleBuffer_.Release();
    instance_->particleFreeListBuffer_.Release();
    instance_->particleFreeListIndexBuffer_.Release();
    instance_->particleCountBuffer_.Release();
    instance_->cameraInfoBuffer_.Release();
    instance_->vertexBuffer_.Release();
    instance_->indexBuffer_.Release();
}

//////////////////////////////////////////////////////////////////////////
// GUIの描画
//////////////////////////////////////////////////////////////////////////
void GPUParticleSystem::DrawGUI(){
#ifdef _DEBUG

    ImFunc::CustomBegin("GPUParticleSystem", MoveOnly_TitleBar);
    {
        ImGui::Text("パーティクル数: %d / %d", particleCountBuffer_.GetValue(), instance_->maxParticleCount_);
        
        if(currentModelData_){
            ImGui::Text("頂点数: %d", (int)currentModelData_->meshes[0].vertices.size());
            ImGui::Text("インデックス数: %d", (int)currentModelData_->meshes[0].indices.size());
        }

        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Spacing();

        ImGui::DragFloat3("Position", &emitter_.position.x, 0.01f);
        ImGui::DragFloat3("EmitRange", &emitter_.emitRange.x, 0.01f);
        ImGui::DragFloat3("MinScale", &emitter_.minScale.x, 0.01f);
        ImGui::DragFloat3("MaxScale", &emitter_.maxScale.x, 0.01f);
        ImGui::DragFloat("MinRotation", &emitter_.minRotation, 0.01f);
        ImGui::DragFloat("MaxRotation", &emitter_.maxRotation, 0.01f);
        ImGui::DragFloat("MinRotateSpeed", &emitter_.minRotateSpeed, 0.01f);
        ImGui::DragFloat("MaxRotateSpeed", &emitter_.maxRotateSpeed, 0.01f);
        ImGui::DragFloat("AngleRange", &emitter_.angleRange, 0.01f);
        ImGui::DragFloat("MinSpeed", &emitter_.minSpeed, 0.01f);
        ImGui::DragFloat("MaxSpeed", &emitter_.maxSpeed, 0.01f);
        ImGui::ColorEdit4("Color", &emitter_.color.x);
        ImGui::DragFloat("MinLifeTime", &emitter_.minLifeTime, 0.01f);
        ImGui::DragFloat("MaxLifeTime", &emitter_.maxLifeTime, 0.01f);
        ImGui::DragFloat("Interval", &emitter_.interval, 0.01f, 0.0f, 100.0f);
        ImGui::DragInt("NumEmitEvery", &emitter_.numEmitEvery, 1, 1);

        // モデル形状の選択
        static std::filesystem::path modelPath = "Resources/Models/";
        static std::filesystem::path rootPath = "Resources/Models/";
        std::string selected = ImFunc::FolderView("エミットモデル選択", modelPath, false, { ".obj", ".gltf",".glb"}, rootPath);
        if(!selected.empty()){
            // 選択されたモデルを読み込む
            instance_->currentModelData_ = ModelManager::GetInstance()->GetModelData(selected);
        }

        ImGui::End();
    }

#endif // _DEBUG
}