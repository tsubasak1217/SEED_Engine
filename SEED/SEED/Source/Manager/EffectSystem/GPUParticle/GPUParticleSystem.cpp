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
    // 初期化状態の設定
    instance_->InitState();
    // 情報のバインド
    instance_->BindInfo();
    // 初期化CSの実行
    instance_->Dispatch("GPUParticle/GPUParticleInitCS.pip", 1, 1);
    // モデルの初期化
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
            DxManager::instance_->GetDevice(), sizeof(GPUEmitter) * instance_->maxEmitterCount_,
            D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_FLAG_NONE
        );

        // SRVとして初期化
        emitterBuffer_.bufferResource.InitState(D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE);

        // Mapする
        emitterBuffer_.Map();

        // SRVのdescの設定
        auto& srvDesc = emitterBuffer_.GetSRVDesc();
        srvDesc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
        srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
        srvDesc.Buffer.NumElements = instance_->maxEmitterCount_;
        srvDesc.Format = DXGI_FORMAT_UNKNOWN;
        srvDesc.Buffer.StructureByteStride = sizeof(GPUEmitter);

        // ヒープをセット
        ID3D12DescriptorHeap* ppHeaps[] = { ViewManager::GetHeap(HEAP_TYPE::SRV_CBV_UAV) };
        DxManager::instance_->commandList->SetDescriptorHeaps(1, ppHeaps);

        // Viewを作成
        emitterBuffer_.CreateSRV("EmitterBuffer_SRV");

        // emitterを追加
        for(int32_t i = 0; i < 1; ++i){
            auto& emitter = instance_->emitters_.emplace_back(GPUEmitter());
            emitter.position = Vector3(0.0f, 0.0f, 0.0f);
            emitter.emitRange = Vector3(1.0f, 1.0f, 1.0f);
            emitter.minScale = Vector3(1.0f, 1.0f, 1.0f);
            emitter.maxScale = Vector3(1.0f, 1.0f, 1.0f);
            emitter.minRotation = Vector3(0.0f, 0.0f, 0.0f);
            emitter.maxRotation = Vector3(0.0f, 0.0f, 0.0f);
            emitter.rotateAxis = Vector3(0.0f, 1.0f, 0.0f);
            emitter.useRotateAxis = false;
            emitter.minRotateSpeed = -10.0f;
            emitter.maxRotateSpeed = 10.0f;
            emitter.isBillboard = true;
            emitter.baseDirection = Vector3(0.0f, 1.0f, 0.0f);
            emitter.directionRange = 1.0f;
            emitter.minSpeed = 10.0f;
            emitter.maxSpeed = 10.0f;
            emitter.color = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
            emitter.minLifeTime = 1.0f;
            emitter.maxLifeTime = 2.0f;
            emitter.interval = 0.1f;
            emitter.currentTime = 0.0f;
            emitter.numEmitEvery = 32;
            emitter.emit = false;
            emitter.loop = true;
            emitter.alive = true;
            emitter.textureIndex = TextureManager::LoadTexture("DefaultAssets/white1x1.png");
        }
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
            DxManager::instance_->GetDevice(), sizeof(int32_t) * instance_->maxParticleCount_,
            D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS
        );

        // UAVとして初期化
        particleFreeListIndexBuffer_.bufferResource.InitState(D3D12_RESOURCE_STATE_UNORDERED_ACCESS);

        // UAVのdescの設定
        auto& uavDesc = particleFreeListIndexBuffer_.GetUAVDesc();
        uavDesc.ViewDimension = D3D12_UAV_DIMENSION_BUFFER;
        uavDesc.Buffer.NumElements = instance_->maxParticleCount_;
        uavDesc.Format = DXGI_FORMAT_UNKNOWN;
        uavDesc.Buffer.StructureByteStride = sizeof(int32_t);

        // SRVのdescの設定
        auto& srvDesc = particleFreeListIndexBuffer_.GetSRVDesc();
        srvDesc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
        srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
        srvDesc.Buffer.NumElements = instance_->maxParticleCount_;
        srvDesc.Format = DXGI_FORMAT_UNKNOWN;
        srvDesc.Buffer.StructureByteStride = sizeof(int32_t);

        // Viewを作成
        particleFreeListIndexBuffer_.CreateUAV("ParticleFreeListIndexBuffer_UAV");
        particleFreeListIndexBuffer_.CreateSRV("ParticleFreeListIndexBuffer_SRV");
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
// 初期化状態の設定
//////////////////////////////////////////////////////////////////////////
void GPUParticleSystem::InitState(){


    // UAVとして初期化


}


//////////////////////////////////////////////////////////////////////////
// 情報のバインド
//////////////////////////////////////////////////////////////////////////
void GPUParticleSystem::BindInfo(){

    // GPUParticleInitCS
    PSOManager::SetBindInfo("GPUParticle/GPUParticleInitCS.pip", "gParticles", particleBuffer_.GetUAVHandle());
    PSOManager::SetBindInfo("GPUParticle/GPUParticleInitCS.pip", "gFreeList", particleFreeListBuffer_.GetUAVHandle());
    PSOManager::SetBindInfo("GPUParticle/GPUParticleInitCS.pip", "gFreeListIndex", particleFreeListIndexBuffer_.GetUAVHandle());

    // EmitterUpdateCS
    PSOManager::SetBindInfo("GPUParticle/EmitterUpdateCS.pip", "gEmitters", emitterBuffer_.GetSRVHandle());
    PSOManager::SetBindInfo("GPUParticle/EmitterUpdateCS.pip", "gParticles", particleBuffer_.GetUAVHandle());
    PSOManager::SetBindInfo("GPUParticle/EmitterUpdateCS.pip", "gFreeListIndex", particleFreeListIndexBuffer_.GetUAVHandle());
    PSOManager::SetBindInfo("GPUParticle/EmitterUpdateCS.pip", "gFreeList", particleFreeListBuffer_.GetSRVHandle());
    PSOManager::SetBindInfo("GPUParticle/EmitterUpdateCS.pip", "gEmitterCount", &emitterCount_);
    PSOManager::SetBindInfo("GPUParticle/EmitterUpdateCS.pip", "gDeltaTime", &deltaTime_);
    PSOManager::SetBindInfo("GPUParticle/EmitterUpdateCS.pip", "gCurrentTime", &totalTime_);

    // ParticleUpdateCS
    PSOManager::SetBindInfo("GPUParticle/ParticleUpdateCS.pip", "gParticles", particleBuffer_.GetUAVHandle());
    PSOManager::SetBindInfo("GPUParticle/ParticleUpdateCS.pip", "gDeltaTime", &deltaTime_);
    PSOManager::SetBindInfo("GPUParticle/ParticleUpdateCS.pip", "gFreeList", particleFreeListBuffer_.GetUAVHandle());
    PSOManager::SetBindInfo("GPUParticle/ParticleUpdateCS.pip", "gFreeListIndex", particleFreeListIndexBuffer_.GetUAVHandle());

    // GPUParticleVS
    PSOManager::SetBindInfo("GPUParticle/GPUParticleVS.pip", "particles", particleBuffer_.GetSRVHandle());
    PSOManager::SetBindInfo("GPUParticle/GPUParticleVS.pip", "cameraInfo", cameraInfoBuffer_.bufferResource.resource->GetGPUVirtualAddress());
    PSOManager::SetBindInfo("GPUParticle/GPUParticleVS.pip", "gTexture", ViewManager::GetHandleGPU(HEAP_TYPE::SRV_CBV_UAV, 0));
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

    // リソースの状態を変更
    instance_->particleBuffer_.bufferResource.TransitionState(
        D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE
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
// 更新
//////////////////////////////////////////////////////////////////////////
void GPUParticleSystem::Update(){

    for(auto& emitter : instance_->emitters_){

        if(emitter.alive == false){
            continue; // エミッターが生存していない場合はスキップ
        }

        if(emitter.emit){
            if(emitter.loop){
                emitter.currentTime -= emitter.interval; // エミッターの時間をリセット
                emitter.emit = false; // エミットフラグを下ろす
            } else{
                emitter.alive = false; // ループしない場合はエミッターを終了
                emitter.emit = false; // エミットフラグを下ろす
            }
        }


        if(emitter.alive){
            // エミッターの更新
            emitter.currentTime += ClockManager::DeltaTime();

            if(emitter.currentTime >= emitter.interval){
                emitter.emit = true; // エミットフラグを立てる
            }
        }
    }


    // 時間の更新
    instance_->deltaTime_ = ClockManager::DeltaTime();
    instance_->totalTime_ += instance_->deltaTime_;

    // エミッターの数を更新
    instance_->emitterCount_ = (int32_t)instance_->emitters_.size();

    // エミッターの情報をリソースに書き込む
    auto* data = instance_->emitterBuffer_.data;
    std::memcpy(
        data,
        instance_->emitters_.data(),
        sizeof(GPUEmitter) * instance_->emitterCount_
    );

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

    // UAVの状態に変更
    instance_->particleBuffer_.bufferResource.TransitionState(
        D3D12_RESOURCE_STATE_UNORDERED_ACCESS
    );

    // SRVの状態に変更
    instance_->particleFreeListBuffer_.bufferResource.TransitionState(
        D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE
    );


    // エミット処理
    instance_->Dispatch("GPUParticle/EmitterUpdateCS.pip", 1, 1);

    // UAVの状態に変更
    instance_->particleFreeListBuffer_.bufferResource.TransitionState(
        D3D12_RESOURCE_STATE_UNORDERED_ACCESS
    );


    // パーティクルの更新
    instance_->Dispatch("GPUParticle/ParticleUpdateCS.pip", 1, 1);
}



//////////////////////////////////////////////////////////////////////////
// 解放
//////////////////////////////////////////////////////////////////////////
void GPUParticleSystem::Release(){

    if(instance_ == nullptr){
        return; // 既に解放されている場合は何もしない
    }

    // インスタンスの解放
    if(instance_->emitterBuffer_.IsCreated()){
        instance_->emitterBuffer_.bufferResource.resource.Reset();
    }
    if(instance_->particleBuffer_.IsCreated()){
        instance_->particleBuffer_.bufferResource.resource.Reset();
    }
    if(instance_->particleFreeListBuffer_.IsCreated()){
        instance_->particleFreeListBuffer_.bufferResource.resource.Reset();
    }
    if(instance_->particleFreeListIndexBuffer_.IsCreated()){
        instance_->particleFreeListIndexBuffer_.bufferResource.resource.Reset();
    }
    if(instance_->cameraInfoBuffer_.IsCreated()){
        instance_->cameraInfoBuffer_.bufferResource.resource.Reset();
    }
    if(instance_->vertexBuffer_.IsCreated()){
        instance_->vertexBuffer_.bufferResource.resource.Reset();
    }
    if(instance_->indexBuffer_.IsCreated()){
        instance_->indexBuffer_.bufferResource.resource.Reset();
    }
}