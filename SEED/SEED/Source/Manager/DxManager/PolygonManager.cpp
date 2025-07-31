// local
#include <SEED/Source/Manager/DxManager/PolygonManager.h>
#include <SEED/Source/Manager/DxManager/DxManager.h>
#include <SEED/Source/SEED.h>
#include <SEED/Lib/Functions/MyFunc/MyMath.h>
#include <SEED/Lib/Functions/MyFunc/MyFunc.h>
#include <Environment/Environment.h>
#include <SEED/Source/Manager/ModelManager/ModelManager.h>
#include <SEED/Source/Manager/TextureManager/TextureManager.h>
#include <SEED/Source/Manager/DxManager/PostEffect.h>

// external
#include <assert.h>

/*------------------------ static, グローバル変数の初期化 ------------------------------*/

uint32_t PolygonManager::triangleIndexCount_ = 0;
uint32_t PolygonManager::quadIndexCount_ = 0;
uint32_t PolygonManager::modelIndexCount_ = 0;
uint32_t PolygonManager::spriteCount_ = 0;
uint32_t PolygonManager::lineCount_ = 0;

std::unordered_map<uint64_t, int32_t> ModelDrawData::modelSwitchIdx_Vertex;
std::unordered_map<uint64_t, int32_t> ModelDrawData::modelSwitchIdx_Index;

D3D12_VERTEX_BUFFER_VIEW ModelDrawData::vbv_vertex;
D3D12_VERTEX_BUFFER_VIEW ModelDrawData::vbv_instance;
D3D12_VERTEX_BUFFER_VIEW ModelDrawData::vbv_skinning;
D3D12_INDEX_BUFFER_VIEW ModelDrawData::ibv;


std::string blendName[(int)BlendMode::kBlendModeCount] = {
    "_NONE",
    "_MUL",
    "_SUB",
    "_NORMAL",
    "_ADD",
    "_SCREEN"
};

std::string cullName[3] = {
    "_NONE",
    "_FRONT",
    "_BACK"
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*                                                                                                               */
/*                                             初期化処理・終了処理                                                 */
/*                                                                                                               */
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

PolygonManager::PolygonManager(DxManager* pDxManager){
    pDxManager_ = pDxManager;
}

PolygonManager::~PolygonManager(){
    //Finalize();
}

void PolygonManager::InitResources(){

    // model
    modelVertexResource_ =
        CreateBufferResource(pDxManager_->device.Get(), sizeof(VertexData) * kMaxVerticesCountInResource_);
    modelVertexResource_->SetName(L"modelVertexResource");
    modelIndexResource_ =
        CreateBufferResource(pDxManager_->device.Get(), sizeof(uint32_t) * kMaxModelVertexCount);
    modelIndexResource_->SetName(L"modelIndexResource");
    modelMaterialResource_ =
        CreateBufferResource(pDxManager_->device.Get(), sizeof(MaterialForGPU) * kMaxMeshCount_);
    modelMaterialResource_->SetName(L"modelMaterialResource");
    modelWvpResource_ =
        CreateBufferResource(pDxManager_->device.Get(), sizeof(TransformMatrix) * 0xffff);
    modelWvpResource_->SetName(L"modelWvpResource");
    offsetResource_ =
        CreateBufferResource(pDxManager_->device.Get(), sizeof(OffsetData) * 0xffff);
    offsetResource_->SetName(L"offsetResource");

    // Skinning
    vertexInfluenceResource_ =
        CreateBufferResource(pDxManager_->device.Get(), sizeof(VertexInfluence) * kMaxVerticesCountInResource_);
    vertexInfluenceResource_->SetName(L"vertexInfluenceResource");
    paletteResource_ =
        CreateBufferResource(pDxManager_->device.Get(), sizeof(WellForGPU) * 0xffff);
    paletteResource_->SetName(L"paletteResource");

    // Camera
    cameraResource_ =
        CreateBufferResource(pDxManager_->device.Get(), sizeof(CameraForGPU) * 16);
    cameraResource_->SetName(L"cameraResource");

    // Lighting
    directionalLightResource_ =
        CreateBufferResource(pDxManager_->device.Get(), sizeof(DirectionalLight) * 0xff);
    directionalLightResource_->SetName(L"lightingResource");
    pointLightResource_ =
        CreateBufferResource(pDxManager_->device.Get(), sizeof(PointLight) * 0xff);
    pointLightResource_->SetName(L"pointLightResource");
    spotLightResource_ =
        CreateBufferResource(pDxManager_->device.Get(), sizeof(SpotLight) * 0xff);
    spotLightResource_->SetName(L"spotLightResource");

    // resourceのMapping
    MapOnce();

    // primitiveの初期化
    InitializePrimitive();


    ////////////////////////////////////////////////
    // viewの作成
    ////////////////////////////////////////////////

    // SRVのDescの設定
    D3D12_SHADER_RESOURCE_VIEW_DESC instancingSrvDesc[7];
    instancingSrvDesc[0].Format = DXGI_FORMAT_UNKNOWN;
    instancingSrvDesc[0].Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
    instancingSrvDesc[0].ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
    instancingSrvDesc[0].Buffer.FirstElement = 0;
    instancingSrvDesc[0].Buffer.Flags = D3D12_BUFFER_SRV_FLAG_NONE;
    instancingSrvDesc[0].Buffer.NumElements = 0xffff;

    instancingSrvDesc[1].Format = DXGI_FORMAT_UNKNOWN;
    instancingSrvDesc[1].Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
    instancingSrvDesc[1].ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
    instancingSrvDesc[1].Buffer.FirstElement = 0;
    instancingSrvDesc[1].Buffer.Flags = D3D12_BUFFER_SRV_FLAG_NONE;
    instancingSrvDesc[1].Buffer.NumElements = 0xffff;

    instancingSrvDesc[2].Format = DXGI_FORMAT_UNKNOWN;
    instancingSrvDesc[2].Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
    instancingSrvDesc[2].ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
    instancingSrvDesc[2].Buffer.FirstElement = 0;
    instancingSrvDesc[2].Buffer.Flags = D3D12_BUFFER_SRV_FLAG_NONE;
    instancingSrvDesc[2].Buffer.NumElements = 0xffff;

    instancingSrvDesc[3].Format = DXGI_FORMAT_UNKNOWN;
    instancingSrvDesc[3].Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
    instancingSrvDesc[3].ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
    instancingSrvDesc[3].Buffer.FirstElement = 0;
    instancingSrvDesc[3].Buffer.Flags = D3D12_BUFFER_SRV_FLAG_NONE;
    instancingSrvDesc[3].Buffer.NumElements = 0xf;

    instancingSrvDesc[4].Format = DXGI_FORMAT_UNKNOWN;
    instancingSrvDesc[4].Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
    instancingSrvDesc[4].ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
    instancingSrvDesc[4].Buffer.FirstElement = 0;
    instancingSrvDesc[4].Buffer.Flags = D3D12_BUFFER_SRV_FLAG_NONE;
    instancingSrvDesc[4].Buffer.NumElements = 0xff;

    instancingSrvDesc[5].Format = DXGI_FORMAT_UNKNOWN;
    instancingSrvDesc[5].Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
    instancingSrvDesc[5].ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
    instancingSrvDesc[5].Buffer.FirstElement = 0;
    instancingSrvDesc[5].Buffer.Flags = D3D12_BUFFER_SRV_FLAG_NONE;
    instancingSrvDesc[5].Buffer.NumElements = 0xff;

    instancingSrvDesc[6].Format = DXGI_FORMAT_UNKNOWN;
    instancingSrvDesc[6].Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
    instancingSrvDesc[6].ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
    instancingSrvDesc[6].Buffer.FirstElement = 0;
    instancingSrvDesc[6].Buffer.Flags = D3D12_BUFFER_SRV_FLAG_NONE;
    instancingSrvDesc[6].Buffer.NumElements = 0xff;

    /*------------- Transform用 --------------*/
    instancingSrvDesc[0].Buffer.StructureByteStride = sizeof(TransformMatrix);
    ViewManager::CreateView(
        VIEW_TYPE::SRV, modelWvpResource_.Get(),
        &instancingSrvDesc[0], "instancingResource_Transform"
    );

    /*------------- Material用 --------------*/
    instancingSrvDesc[1].Buffer.StructureByteStride = sizeof(MaterialForGPU);
    ViewManager::CreateView(
        VIEW_TYPE::SRV, modelMaterialResource_.Get(),
        &instancingSrvDesc[1], "instancingResource_Material"
    );

    /*-------------- Palette用 --------------*/
    instancingSrvDesc[2].Buffer.StructureByteStride = sizeof(WellForGPU);
    ViewManager::CreateView(
        VIEW_TYPE::SRV, paletteResource_.Get(),
        &instancingSrvDesc[2], "SkinningResource_Palette"
    );

    /*------------- Camera用 --------------*/
    instancingSrvDesc[3].Buffer.StructureByteStride = sizeof(CameraForGPU);
    ViewManager::CreateView(
        VIEW_TYPE::SRV, cameraResource_.Get(),
        &instancingSrvDesc[3], "CameraResource"
    );

    /*--------- DirectionalLight用 ----------*/
    instancingSrvDesc[4].Buffer.StructureByteStride = sizeof(DirectionalLight);
    ViewManager::CreateView(
        VIEW_TYPE::SRV, directionalLightResource_.Get(),
        &instancingSrvDesc[4], "directionalLight"
    );

    /*------------ PointLight用 --------------*/
    instancingSrvDesc[5].Buffer.StructureByteStride = sizeof(PointLight);
    ViewManager::CreateView(
        VIEW_TYPE::SRV, pointLightResource_.Get(),
        &instancingSrvDesc[5], "pointLight"
    );

    /*------------ SpotLight用 --------------*/
    instancingSrvDesc[6].Buffer.StructureByteStride = sizeof(SpotLight);
    ViewManager::CreateView(
        VIEW_TYPE::SRV, spotLightResource_.Get(),
        &instancingSrvDesc[6], "spotLight"
    );


    //////////////////////////////////////////////////
    // GPUハンドルの取得
    //////////////////////////////////////////////////
    gpuHandles_.resize(8);
    gpuHandles_[(int)HANDLE_TYPE::TextureTable] =
        ViewManager::GetHandleGPU(HEAP_TYPE::SRV_CBV_UAV, 0);
    gpuHandles_[(int)HANDLE_TYPE::InstancingResource_Transform] =
        ViewManager::GetHandleGPU(HEAP_TYPE::SRV_CBV_UAV, "instancingResource_Transform");
    gpuHandles_[(int)HANDLE_TYPE::InstancingResource_Material] =
        ViewManager::GetHandleGPU(HEAP_TYPE::SRV_CBV_UAV, "instancingResource_Material");
    gpuHandles_[(int)HANDLE_TYPE::SkinningResource_Palette] =
        ViewManager::GetHandleGPU(HEAP_TYPE::SRV_CBV_UAV, "SkinningResource_Palette");
    gpuHandles_[(int)HANDLE_TYPE::CameraResource] =
        ViewManager::GetHandleGPU(HEAP_TYPE::SRV_CBV_UAV, "CameraResource");
    gpuHandles_[(int)HANDLE_TYPE::DirectionalLight] =
        ViewManager::GetHandleGPU(HEAP_TYPE::SRV_CBV_UAV, "directionalLight");
    gpuHandles_[(int)HANDLE_TYPE::PointLight] =
        ViewManager::GetHandleGPU(HEAP_TYPE::SRV_CBV_UAV, "pointLight");
    gpuHandles_[(int)HANDLE_TYPE::SpotLight] =
        ViewManager::GetHandleGPU(HEAP_TYPE::SRV_CBV_UAV, "spotLight");

    //////////////////////////////////////////////////
    // バインドする情報を設定
    //////////////////////////////////////////////////

    // CommonVSPipeline
    PSOManager::SetBindInfo("CommonVSPipeline.pip", "transforms", gpuHandles_[(int)HANDLE_TYPE::InstancingResource_Transform]);
    PSOManager::SetBindInfo("CommonVSPipeline.pip", "gCamera", gpuHandles_[(int)HANDLE_TYPE::CameraResource]);
    PSOManager::SetBindInfo("CommonVSPipeline.pip", "gMaterial", gpuHandles_[(int)HANDLE_TYPE::InstancingResource_Material]);
    PSOManager::SetBindInfo("CommonVSPipeline.pip", "gDirectionalLight", gpuHandles_[(int)HANDLE_TYPE::DirectionalLight]);
    PSOManager::SetBindInfo("CommonVSPipeline.pip", "gPointLight", gpuHandles_[(int)HANDLE_TYPE::PointLight]);
    PSOManager::SetBindInfo("CommonVSPipeline.pip", "gSpotLight", gpuHandles_[(int)HANDLE_TYPE::SpotLight]);
    PSOManager::SetBindInfo("CommonVSPipeline.pip", "gTexture", gpuHandles_[(int)HANDLE_TYPE::TextureTable]);
    PSOManager::SetBindInfo("CommonVSPipeline.pip", "gDirectionalLightCount", &directionalLightCount_);
    PSOManager::SetBindInfo("CommonVSPipeline.pip", "gPointLightCount", &pointLightCount_);
    PSOManager::SetBindInfo("CommonVSPipeline.pip", "gSpotLightCount", &spotLightCount_);

    // SkinningVSPipeline
    PSOManager::SetBindInfo("SkinningVSPipeline.pip", "transforms", gpuHandles_[(int)HANDLE_TYPE::InstancingResource_Transform]);
    PSOManager::SetBindInfo("SkinningVSPipeline.pip", "gMatrixPalette", gpuHandles_[(int)HANDLE_TYPE::SkinningResource_Palette]);
    PSOManager::SetBindInfo("SkinningVSPipeline.pip", "gCamera", gpuHandles_[(int)HANDLE_TYPE::CameraResource]);
    PSOManager::SetBindInfo("SkinningVSPipeline.pip", "gMaterial", gpuHandles_[(int)HANDLE_TYPE::InstancingResource_Material]);
    PSOManager::SetBindInfo("SkinningVSPipeline.pip", "gDirectionalLight", gpuHandles_[(int)HANDLE_TYPE::DirectionalLight]);
    PSOManager::SetBindInfo("SkinningVSPipeline.pip", "gPointLight", gpuHandles_[(int)HANDLE_TYPE::PointLight]);
    PSOManager::SetBindInfo("SkinningVSPipeline.pip", "gSpotLight", gpuHandles_[(int)HANDLE_TYPE::SpotLight]);
    PSOManager::SetBindInfo("SkinningVSPipeline.pip", "gTexture", gpuHandles_[(int)HANDLE_TYPE::TextureTable]);
    PSOManager::SetBindInfo("SkinningVSPipeline.pip", "gDirectionalLightCount", &directionalLightCount_);
    PSOManager::SetBindInfo("SkinningVSPipeline.pip", "gPointLightCount", &pointLightCount_);
    PSOManager::SetBindInfo("SkinningVSPipeline.pip", "gSpotLightCount", &spotLightCount_);

    // TextVSPipeline
    PSOManager::SetBindInfo("TextVSPipeline.pip", "transforms", gpuHandles_[(int)HANDLE_TYPE::InstancingResource_Transform]);
    PSOManager::SetBindInfo("TextVSPipeline.pip", "gCamera", gpuHandles_[(int)HANDLE_TYPE::CameraResource]);
    PSOManager::SetBindInfo("TextVSPipeline.pip", "gMaterial", gpuHandles_[(int)HANDLE_TYPE::InstancingResource_Material]);
    PSOManager::SetBindInfo("TextVSPipeline.pip", "gDirectionalLight", gpuHandles_[(int)HANDLE_TYPE::DirectionalLight]);
    PSOManager::SetBindInfo("TextVSPipeline.pip", "gPointLight", gpuHandles_[(int)HANDLE_TYPE::PointLight]);
    PSOManager::SetBindInfo("TextVSPipeline.pip", "gSpotLight", gpuHandles_[(int)HANDLE_TYPE::SpotLight]);
    PSOManager::SetBindInfo("TextVSPipeline.pip", "gTexture", gpuHandles_[(int)HANDLE_TYPE::TextureTable]);
    PSOManager::SetBindInfo("TextVSPipeline.pip", "gDirectionalLightCount", &directionalLightCount_);
    PSOManager::SetBindInfo("TextVSPipeline.pip", "gPointLightCount", &pointLightCount_);
    PSOManager::SetBindInfo("TextVSPipeline.pip", "gSpotLightCount", &spotLightCount_);

    // SkyBoxVSPipeline
    PSOManager::SetBindInfo("SkyBoxVSPipeline.pip", "transforms", gpuHandles_[(int)HANDLE_TYPE::InstancingResource_Transform]);
    PSOManager::SetBindInfo("SkyBoxVSPipeline.pip", "gMaterial", gpuHandles_[(int)HANDLE_TYPE::InstancingResource_Material]);

}

// 毎フレームセットする情報
void PolygonManager::BindFrameDatas(){
    // CommonVSPipeline
    PSOManager::SetBindInfo("CommonVSPipeline.pip", "gEnvironmentTexture", TextureManager::GetHandleGPU(SkyBox::textureName_));
    // SkinningVSPipeline
    PSOManager::SetBindInfo("SkinningVSPipeline.pip", "gEnvironmentTexture", TextureManager::GetHandleGPU(SkyBox::textureName_));
    // SkyBoxVSPipeline
    PSOManager::SetBindInfo("SkyBoxVSPipeline.pip", "gEnvironmentTexture", TextureManager::GetHandleGPU(SkyBox::textureName_));
}

// カメラは別途バインドする必要があるので、BindCameraDatasを呼び出す
void PolygonManager::BindCameraDatas(const std::string& cameraName){
    // カメラのインデックスを設定
    PSOManager::SetBindInfo("CommonVSPipeline.pip", "gCameraIndex", &cameraOrder_[cameraName]);
    PSOManager::SetBindInfo("SkinningVSPipeline.pip", "gCameraIndex", &cameraOrder_[cameraName]);
    PSOManager::SetBindInfo("TextVSPipeline.pip", "gCameraIndex", &cameraOrder_[cameraName]);
    // カメラが切り替わるインスタンス数を設定
    PSOManager::SetBindInfo("CommonVSPipeline.pip", "cameraIndexOffset", &cameraSwitchInstanceCount_[cameraName]);
    PSOManager::SetBindInfo("SkinningVSPipeline.pip", "cameraIndexOffset", &cameraSwitchInstanceCount_[cameraName]);
    PSOManager::SetBindInfo("TextVSPipeline.pip", "cameraIndexOffset", &cameraSwitchInstanceCount_[cameraName]);
    PSOManager::SetBindInfo("SkyBoxVSPipeline.pip", "cameraIndexOffset", &cameraSwitchInstanceCount_[cameraName]);
}

void PolygonManager::Finalize(){}

void PolygonManager::Reset(){

    // モデルの情報をリセット
    for(auto& modelDrawData : modelDrawData_){
        modelDrawData.second->indexCount = 0;
        modelDrawData.second->totalDrawCount = 0;
    }

    // ライティングの情報をリセット
    directionalLights_.clear();
    pointLights_.clear();
    spotLights_.clear();

    // カウントのリセット
    triangleIndexCount_ = 0;
    quadIndexCount_ = 0;
    modelIndexCount_ = 0;
    spriteCount_ = 0;
    lineCount_ = 0;
    objCounts_.fill(0);
    objCountCull_.fill(0);
    objCountBlend_.fill(0);
    cameraSwitchInstanceCount_.clear();
    cameraOrder_.clear();

    // 描画リストのリセット
    orderedDrawLists_.clear();
    for(auto& drawList : drawLists_){
        drawList.second.clear();
    }

    // フラグの初期化
    isWrited_ = false;
    skyBoxAdded_ = false;
}


// プリミティブ描画情報の初期化
void PolygonManager::InitializePrimitive(){

    // プリミティブの頂点などの情報を初期化
    for(int blendIdx = 0; blendIdx < (int)BlendMode::kBlendModeCount; blendIdx++){
        for(int cullIdx = 0; cullIdx < 3; ++cullIdx){
            for(auto& primitiveData : primitiveData_){
                primitiveData[blendIdx][cullIdx].meshes.clear();
                primitiveData[blendIdx][cullIdx].materials.clear();
            }
        }
    }

}

// 初期化時に一度のみマッピングを行う
void PolygonManager::MapOnce(){
    modelVertexResource_.Get()->Map(0, nullptr, reinterpret_cast<void**>(&mapVertexData));
    modelIndexResource_.Get()->Map(0, nullptr, reinterpret_cast<void**>(&mapIndexData));
    modelMaterialResource_.Get()->Map(0, nullptr, reinterpret_cast<void**>(&mapMaterialData));
    modelWvpResource_.Get()->Map(0, nullptr, reinterpret_cast<void**>(&mapTransformData));
    offsetResource_.Get()->Map(0, nullptr, reinterpret_cast<void**>(&mapOffsetData));
    vertexInfluenceResource_.Get()->Map(0, nullptr, reinterpret_cast<void**>(&mapVertexInfluenceData));
    paletteResource_.Get()->Map(0, nullptr, reinterpret_cast<void**>(&mapPaletteData));
    cameraResource_.Get()->Map(0, nullptr, reinterpret_cast<void**>(&mapCameraData));
    directionalLightResource_.Get()->Map(0, nullptr, reinterpret_cast<void**>(&mapDirectionalLightData));
    pointLightResource_.Get()->Map(0, nullptr, reinterpret_cast<void**>(&mapPointLightData));
    spotLightResource_.Get()->Map(0, nullptr, reinterpret_cast<void**>(&mapSpotLightData));
}



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                                                        //
//                                                     三角形の追加                                                         //
//                                                                                                                        //
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// DrawTriangleが呼び出されるごとに 三角形の情報を積み上げていく
void PolygonManager::AddTriangle(
    const Vector4& v1, const Vector4& v2, const Vector4& v3,
    const Matrix4x4& worldMat, const Vector4& color,
    int32_t lightingType, const Matrix4x4& uvTransform, bool view3D, uint32_t GH,
    BlendMode blendMode, D3D12_CULL_MODE cullMode, bool isStaticDraw,
    DrawLocation drawLocation, int32_t layer
){

    assert(triangleIndexCount_ < kMaxTriangleCount_);

    Vector3 transformed[3];
    float layerZ = layer * 0.001f;

    transformed[0] = Multiply(TransformToVec3(v1), worldMat);
    transformed[1] = Multiply(TransformToVec3(v2), worldMat);
    transformed[2] = Multiply(TransformToVec3(v3), worldMat);

    // レイヤー、描画場所に応じたZ値に設定
    if(drawLocation != DrawLocation::Not2D){
        if(drawLocation == DrawLocation::Back && !isStaticDraw){
            float zFar = pDxManager_->mainCamera_->GetZFar() - layerZ;
            transformed[0].z = zFar;
            transformed[1].z = zFar;
            transformed[2].z = zFar;
        } else{
            float zNear = pDxManager_->mainCamera_->GetZNear() - layerZ;
            transformed[0].z = zNear;
            transformed[1].z = zNear;
            transformed[2].z = zNear;
        }
    }

    Vector3 normalVec =
        MyMath::Normalize(MyMath::Cross(
            v2.ToVec3() - v1.ToVec3(),
            v3.ToVec3() - v2.ToVec3(),
            view3D ? kWorld : kScreen
        ));

    ///////////////////////////////////////////////////////////////////
    /*-------------------- ModelDataに情報を追加する--------------------*/
    ///////////////////////////////////////////////////////////////////

    // 頂点情報の取得
    auto* modelData = view3D ?
        &primitiveData_[PRIMITIVE_TRIANGLE][(int)blendMode][(int)cullMode - 1] :
        &primitiveData_[PRIMITIVE_TRIANGLE2D][(int)blendMode][(int)cullMode - 1];
    isStaticDraw ? modelData = &primitiveData_[PRIMITIVE_STATIC_TRIANGLE2D][(int)blendMode][(int)cullMode - 1] : modelData;

    // 描画データの取得
    static std::string drawDataName[2];
    uint64_t hash;
    drawDataName[0].clear();
    drawDataName[0].reserve(128);
    drawDataName[0] += "T";
    drawDataName[0] += blendName[(int)blendMode];
    drawDataName[0] += cullName[(int)cullMode - 1];
    drawDataName[1].clear();
    drawDataName[1].reserve(128);
    drawDataName[1] += isStaticDraw ? "ST2" : "T2";
    drawDataName[1] += blendName[(int)blendMode];
    drawDataName[1] += cullName[(int)cullMode - 1];

    // もし該当する描画データがなければ作成する
    view3D ? hash = MyFunc::Hash64(drawDataName[0]) : hash = MyFunc::Hash64(drawDataName[1]);
    if(modelDrawData_.find(hash) == modelDrawData_.end()){

        // 頂点データの取得
        modelDrawData_[hash] = std::make_unique<ModelDrawData>();
        modelDrawData_[hash]->modelData = modelData;

        // パイプラインの設定
        modelDrawData_[hash]->pso =
            PSOManager::GetPSO(
                "CommonVSPipeline.pip",
                blendMode, cullMode, PolygonTopology::TRIANGLE
            );

        // 描画種類の設定
        modelDrawData_[hash]->drawOrder = view3D ?
            (int8_t)DrawOrder::Triangle : (int8_t)DrawOrder::Triangle2D;
        modelDrawData_[hash]->drawOrder = isStaticDraw ?
            (int8_t)DrawOrder::StaticTriangle2D : modelDrawData_[hash]->drawOrder;
        // 名前の設定
        modelDrawData_[hash]->name = view3D ? drawDataName[0] : drawDataName[1];
        // hashの設定
        modelDrawData_[hash]->hash = hash;
    }

    // indexCount
    auto* drawData = modelDrawData_[hash].get();
    int drawCount = drawData->totalDrawCount;
    int indexCount = drawCount * 3;
    // vertexResource
    modelData->meshes.resize(1);
    auto& mesh = modelData->meshes[0];
    if(mesh.vertices.size() < indexCount + 3){ mesh.vertices.resize(indexCount + 3); }
    mesh.vertices[indexCount] = VertexData(transformed[0].ToVec4(), Vector2(0.5f, 0.0f), normalVec);
    mesh.vertices[indexCount + 1] = VertexData(transformed[1].ToVec4(), Vector2(1.0f, 1.0f), normalVec);
    mesh.vertices[indexCount + 2] = VertexData(transformed[2].ToVec4(), Vector2(0.0f, 1.0f), normalVec);
    //indexResource
    if(mesh.indices.size() < indexCount + 3){ mesh.indices.resize(indexCount + 3); }
    mesh.indices[indexCount] = indexCount + 0;
    mesh.indices[indexCount + 1] = indexCount + 1;
    mesh.indices[indexCount + 2] = indexCount + 2;
    // 都合上頂点数に合わせる
    mesh.vertexInfluences.resize(mesh.vertices.size());
    // materialResource(適当な値に初期化)
    if(modelData->materials.size() == 0){
        modelData->materials.resize(1);
        auto& baseMaterial = modelData->materials[0];
        baseMaterial.textureFilePath_ = "";
        baseMaterial.UV_scale_ = { 1.0f,1.0f,1.0f };
        baseMaterial.UV_offset_ = { 0.0f,0.0f,0.0f };
        baseMaterial.UV_translate_ = { 0.0f,0.0f,0.0f };
    }

    ///////////////////////////////////////////////////////////////////
    /*-------------------- 描画データに情報を書き込む --------------------*/
    ///////////////////////////////////////////////////////////////////

    // material
    drawData->materials.resize(1);
    auto& material = drawData->materials.back();
    if(material.size() <= drawCount){ material.resize(drawCount + 1); }
    material[drawCount].color_ = color;
    material[drawCount].lightingType_ = lightingType;
    material[drawCount].uvTransform_ = uvTransform;
    material[drawCount].GH_ = GH;

    // transform
    for(const auto& [cameraName, camera] : pDxManager_->cameras_){
        auto& transform = drawData->transforms[cameraName];
        if(transform.size() <= drawCount){ transform.resize(drawCount + 1); }
        transform[drawCount].world = worldMat;
        transform[drawCount].WVP = view3D ? camera->GetViewProjectionMat() : camera->GetProjectionMat2D();
        transform[drawCount].worldInverseTranspose = IdentityMat4();
    }

    // offsetResourceの数を更新(描画数分)
    auto& offsetData = drawData->offsetData;
    offsetData.resize(1);// primitiveはmesh数1固定
    if(offsetData.back().size() <= drawCount){
        offsetData.back().resize(drawCount + 1);
    }

    // カウントを更新
    if(isStaticDraw){
        objCounts_[(int)DrawOrder::StaticTriangle2D]++;
    } else{
        if(view3D){
            objCounts_[(int)DrawOrder::Triangle]++;
        } else{
            objCounts_[(int)DrawOrder::Triangle2D]++;
        }
    }

    objCountCull_[(int)cullMode - 1]++;
    objCountBlend_[(int)blendMode]++;
    triangleIndexCount_++;
    drawData->indexCount += 3;
    drawData->totalDrawCount++;
}

void PolygonManager::AddTriangle3DPrimitive(
    const Vector4& v1, const Vector4& v2, const Vector4& v3,
    const Vector2& texCoordV1, const Vector2& texCoordV2, const Vector2& texCoordV3, const Vector4& color,
    uint32_t GH, BlendMode blendMode, int32_t lightingType, const Matrix4x4& uvTransform, D3D12_CULL_MODE cullMode
){
    assert(triangleIndexCount_ < kMaxTriangleCount_);

    Vector3 normalVec =
        MyMath::Normalize(MyMath::Cross(
            v2.ToVec3() - v1.ToVec3(),
            v3.ToVec3() - v2.ToVec3(),
            kWorld
        ));

    ///////////////////////////////////////////////////////////////////
    /*-------------------- ModelDataに情報を追加する--------------------*/
    ///////////////////////////////////////////////////////////////////

    auto* modelData = &primitiveData_[PRIMITIVE_TRIANGLE][(int)blendMode][(int)cullMode - 1];
    static std::string drawDataName;
    drawDataName.clear();
    drawDataName.reserve(128);
    drawDataName += "T";
    drawDataName += blendName[(int)blendMode];
    drawDataName += cullName[(int)cullMode - 1];
    uint64_t hash = MyFunc::Hash64(drawDataName);

    // 無かったら作成する
    if(modelDrawData_.find(hash) == modelDrawData_.end()){
        // 頂点データの取得
        modelDrawData_[hash] = std::make_unique<ModelDrawData>();
        modelDrawData_[hash]->modelData = modelData;

        // パイプラインの設定
        modelDrawData_[hash]->pso =
            PSOManager::GetPSO(
                "CommonVSPipeline.pip",
                blendMode, cullMode, PolygonTopology::TRIANGLE
            );

        // 描画種類の設定
        modelDrawData_[hash]->drawOrder = (int8_t)DrawOrder::Triangle2D;
        // 名前の設定
        modelDrawData_[hash]->name = drawDataName;
        // hashの設定
        modelDrawData_[hash]->hash = hash;
    }

    auto* drawData = modelDrawData_[hash].get();

    // indexCount
    int drawCount = drawData->totalDrawCount;
    int indexCount = drawCount * 3;
    // vertexResource
    modelData->meshes.resize(1);
    auto& mesh = modelData->meshes[0];
    if(mesh.vertices.size() < indexCount + 3){ mesh.vertices.resize(indexCount + 3); }
    mesh.vertices[indexCount] = VertexData(v1, texCoordV1, normalVec);
    mesh.vertices[indexCount + 1] = VertexData(v2, texCoordV2, normalVec);
    mesh.vertices[indexCount + 2] = VertexData(v3, texCoordV3, normalVec);
    //indexResource
    if(mesh.indices.size() < indexCount + 3){ mesh.indices.resize(indexCount + 3); }
    mesh.indices[indexCount] = indexCount + 0;
    mesh.indices[indexCount + 1] = indexCount + 1;
    mesh.indices[indexCount + 2] = indexCount + 2;
    // 頂点数に合わせる
    mesh.vertexInfluences.resize(mesh.vertices.size());
    // materialResource
    if(modelData->materials.size() == 0){
        modelData->materials.resize(1);
        auto& baseMaterial = modelData->materials[0];
        baseMaterial.textureFilePath_ = "";
        baseMaterial.UV_scale_ = { 1.0f,1.0f,1.0f };
        baseMaterial.UV_offset_ = { 0.0f,0.0f,0.0f };
        baseMaterial.UV_translate_ = { 0.0f,0.0f,0.0f };
    }

    ///////////////////////////////////////////////////////////////////
    /*-------------------- 描画データに情報を書き込む --------------------*/
    ///////////////////////////////////////////////////////////////////

    // material
    drawData->materials.resize(1);// primitiveはmesh数1固定
    auto& material = drawData->materials.back();
    if(material.size() <= drawCount){ material.resize(drawCount + 1); }
    material[drawCount].color_ = color;
    material[drawCount].lightingType_ = lightingType;
    material[drawCount].uvTransform_ = uvTransform;
    material[drawCount].GH_ = GH;

    // transform
    for(const auto& [cameraName, camera] : pDxManager_->cameras_){
        auto& transform = drawData->transforms[cameraName];
        if(transform.size() <= drawCount){ transform.resize(drawCount + 1); }
        transform[drawCount].world = IdentityMat4();
        transform[drawCount].WVP = camera->GetViewProjectionMat();
        transform[drawCount].worldInverseTranspose = IdentityMat4();
    }


    // offsetResourceの数を更新
    auto& offsetData = drawData->offsetData;
    offsetData.resize(1);// primitiveはmesh数1固定
    if(offsetData.back().size() <= drawCount){
        offsetData.back().resize(drawCount + 1);
    }


    // カウントを更新
    objCounts_[(int)DrawOrder::Triangle]++;
    objCountCull_[(int)cullMode - 1]++;
    objCountBlend_[(int)blendMode]++;
    triangleIndexCount_++;
    drawData->indexCount += 3;
    drawData->totalDrawCount++;
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                                                        //
//                                                       矩形の追加                                                         //
//                                                                                                                        //
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void PolygonManager::AddQuad(
    const Vector3& v1, const Vector3& v2, const Vector3& v3, const Vector3& v4,
    const Vector2& texCoordV1, const Vector2& texCoordV2, const Vector2& texCoordV3, const Vector2& texCoordV4,
    const Matrix4x4& worldMat, const Vector4& color,
    int32_t lightingType, const Matrix4x4& uvTransform, bool view3D, uint32_t GH,
    BlendMode blendMode, bool isText, D3D12_CULL_MODE cullMode, bool isStaticDraw,
    DrawLocation drawLocation, int32_t layer
){

    assert(triangleIndexCount_ < kMaxTriangleCount_);

    Vector3 transformed[4];
    float layerZ = layer * 0.001f;
    transformed[0] = Multiply(v1, worldMat);
    transformed[1] = Multiply(v2, worldMat);
    transformed[2] = Multiply(v3, worldMat);
    transformed[3] = Multiply(v4, worldMat);

    // レイヤー、描画場所に応じたZ値に設定
    if(drawLocation != DrawLocation::Not2D){
        if(drawLocation == DrawLocation::Back && !isStaticDraw){
            float zFar = pDxManager_->mainCamera_->GetZFar() - layerZ;
            transformed[0].z = zFar;
            transformed[1].z = zFar;
            transformed[2].z = zFar;
            transformed[3].z = zFar;
        } else{
            float zNear = pDxManager_->mainCamera_->GetZNear() - layerZ;
            transformed[0].z = zNear;
            transformed[1].z = zNear;
            transformed[2].z = zNear;
            transformed[3].z = zNear;
        }
    }

    Vector3 normalVec =
        MyMath::Normalize(MyMath::Cross(
            Vector3(transformed[1].x, transformed[1].y, transformed[1].z) - Vector3(transformed[0].x, transformed[0].y, transformed[0].z),
            Vector3(transformed[2].x, transformed[2].y, transformed[2].z) - Vector3(transformed[1].x, transformed[1].y, transformed[1].z),
            view3D ? kWorld : kScreen
        ));

    ///////////////////////////////////////////////////////////////////
    /*-------------------- ModelDataに情報を追加する--------------------*/
    ///////////////////////////////////////////////////////////////////

    ModelData* modelData;
    if(isText){
        modelData = view3D ? &primitiveData_[PRIMITIVE_TEXT][(int)blendMode][(int)cullMode - 1] :
            &primitiveData_[PRIMITIVE_TEXT2D][(int)blendMode][(int)cullMode - 1];
    } else{
        modelData = view3D ? &primitiveData_[PRIMITIVE_QUAD][(int)blendMode][(int)cullMode - 1] :
            &primitiveData_[PRIMITIVE_QUAD2D][(int)blendMode][(int)cullMode - 1];
    }

    if(isStaticDraw){
        if(!isText){
            modelData = &primitiveData_[PRIMITIVE_STATIC_QUAD2D][(int)blendMode][(int)cullMode - 1];
        } else{
            modelData = &primitiveData_[PRIMITIVE_STATIC_TEXT2D][(int)blendMode][(int)cullMode - 1];
        }
    }

    static std::string drawDataName[2];
    drawDataName[0].clear();
    drawDataName[0].reserve(128);
    drawDataName[0] += isText ? "TX" : "Q";
    drawDataName[0] += blendName[(int)blendMode];
    drawDataName[0] += cullName[(int)cullMode - 1];
    drawDataName[1].clear();
    drawDataName[1].reserve(128);
    if(isText){
        drawDataName[1] += isStaticDraw ? "STX2" : "TX2";
    } else{
        drawDataName[1] += isStaticDraw ? "SQ2" : "Q2";
    }
    drawDataName[1] += blendName[(int)blendMode];
    drawDataName[1] += cullName[(int)cullMode - 1];

    // もし該当する描画データがなければ作成する
    uint64_t hash = view3D ? MyFunc::Hash64(drawDataName[0]) : MyFunc::Hash64(drawDataName[1]);

    if(modelDrawData_.find(hash) == modelDrawData_.end()){
        // 頂点データの取得
        modelDrawData_[hash] = std::make_unique<ModelDrawData>();
        modelDrawData_[hash]->modelData = modelData;

        // パイプラインの設定
        if(!isText){
            modelDrawData_[hash]->pso =
                PSOManager::GetPSO(
                    "CommonVSPipeline.pip",
                    blendMode, cullMode, PolygonTopology::TRIANGLE
                );
        } else{
            modelDrawData_[hash]->pso =
                PSOManager::GetPSO(
                    "TextVSPipeline.pip",
                    blendMode, cullMode, PolygonTopology::TRIANGLE
                );
        }

        // 描画種類の設定
        if(!isText){
            modelDrawData_[hash]->drawOrder = view3D ?
                (int8_t)DrawOrder::Quad : (int8_t)DrawOrder::Quad2D;
            modelDrawData_[hash]->drawOrder = isStaticDraw ?
                (int8_t)DrawOrder::StaticQuad2D : modelDrawData_[hash]->drawOrder;
        } else{
            modelDrawData_[hash]->drawOrder = view3D ?
                (int8_t)DrawOrder::Text : (int8_t)DrawOrder::Text2D;
            modelDrawData_[hash]->drawOrder = isStaticDraw ?
                (int8_t)DrawOrder::StaticText2D : modelDrawData_[hash]->drawOrder;
        }

        // 名前の設定
        modelDrawData_[hash]->name = view3D ? drawDataName[0] : drawDataName[1];
        // hashの設定
        modelDrawData_[hash]->hash = hash;
    }

    // Count
    auto* drawData = modelDrawData_[hash].get();
    int drawCount = drawData->totalDrawCount;
    int vertexCount = drawCount * 4;
    int indexCount = drawCount * 6;
    // vertexResource
    modelData->meshes.resize(1);
    auto& mesh = modelData->meshes[0];
    if(mesh.vertices.size() < vertexCount + 4){ mesh.vertices.resize(vertexCount + 4); }
    mesh.vertices[vertexCount] = VertexData(transformed[0].ToVec4(), texCoordV1, normalVec);
    mesh.vertices[vertexCount + 1] = VertexData(transformed[1].ToVec4(), texCoordV2, normalVec);
    mesh.vertices[vertexCount + 2] = VertexData(transformed[2].ToVec4(), texCoordV3, normalVec);
    mesh.vertices[vertexCount + 3] = VertexData(transformed[3].ToVec4(), texCoordV4, normalVec);

    //indexResource
    if(mesh.indices.size() <= indexCount){ mesh.indices.resize(indexCount + 6); }
    mesh.indices[indexCount] = vertexCount;
    mesh.indices[indexCount + 1] = vertexCount + 1;
    mesh.indices[indexCount + 2] = vertexCount + 3;
    mesh.indices[indexCount + 3] = vertexCount + 0;
    mesh.indices[indexCount + 4] = vertexCount + 3;
    mesh.indices[indexCount + 5] = vertexCount + 2;

    // materialResource
    if(modelData->materials.size() == 0){
        modelData->materials.resize(1);
        auto& baseMaterial = modelData->materials[0];
        baseMaterial.textureFilePath_ = "";
        baseMaterial.UV_scale_ = { 1.0f,1.0f,1.0f };
        baseMaterial.UV_offset_ = { 0.0f,0.0f,0.0f };
        baseMaterial.UV_translate_ = { 0.0f,0.0f,0.0f };
    }

    // 頂点数に合わせる(使わないけど規格化のために)
    mesh.vertexInfluences.resize(mesh.vertices.size());

    ///////////////////////////////////////////////////////////////////
    /*-------------------- 描画データに情報を書き込む --------------------*/
    ///////////////////////////////////////////////////////////////////

    // material
    drawData->materials.resize(1);// primitiveはmesh数1固定
    auto& material = drawData->materials.back();
    if(material.size() <= drawCount){ material.resize(drawCount + 1); }
    material[drawCount].color_ = color;
    material[drawCount].lightingType_ = lightingType;
    material[drawCount].uvTransform_ = uvTransform;
    material[drawCount].GH_ = GH;

    // transform
    for(const auto& [cameraName, camera] : pDxManager_->cameras_){
        auto& transform = drawData->transforms[cameraName];
        if(transform.size() <= drawCount){ transform.resize(drawCount + 1); }
        transform[drawCount].world = worldMat;
        transform[drawCount].WVP = view3D ? camera->GetViewProjectionMat() : camera->GetProjectionMat2D();
        transform[drawCount].worldInverseTranspose = IdentityMat4();
    }

    // offsetResourceの数を更新
    auto& offsetData = drawData->offsetData;
    offsetData.resize(1);// primitiveはmesh数1固定
    if(offsetData.back().size() <= drawCount){
        offsetData.back().resize(drawCount + 1);
    }

    // カウントを更新
    if(!isText){
        if(isStaticDraw){
            objCounts_[(int)DrawOrder::StaticQuad2D]++;
        } else{
            if(view3D){
                objCounts_[(int)DrawOrder::Quad]++;
            } else{
                objCounts_[(int)DrawOrder::Quad2D]++;
            }
        }
    } else{
        if(isStaticDraw){
            objCounts_[(int)DrawOrder::StaticText2D]++;
        } else{
            if(view3D){
                objCounts_[(int)DrawOrder::Text]++;
            } else{
                objCounts_[(int)DrawOrder::Text2D]++;
            }
        }
    }

    objCountCull_[(int)cullMode - 1]++;
    objCountBlend_[(int)blendMode]++;
    quadIndexCount_++;
    drawData->indexCount += 6;
    drawData->totalDrawCount++;
}


void PolygonManager::AddQuad3DPrimitive(
    const Vector4& v1, const Vector4& v2, const Vector4& v3, const Vector4& v4,
    const Vector2& texCoordV1, const Vector2& texCoordV2, const Vector2& texCoordV3, const Vector2& texCoordV4,
    const Vector4& color, uint32_t GH, BlendMode blendMode, int32_t lightingType,
    const Matrix4x4& uvTransform, D3D12_CULL_MODE cullMode
){
    assert(triangleIndexCount_ < kMaxTriangleCount_);

    // レイヤー、描画場所に応じたZ値に設定
    Vector3 normalVec =
        MyMath::Normalize(MyMath::Cross(
            v2.ToVec3() - v1.ToVec3(),
            v3.ToVec3() - v2.ToVec3(),
            kWorld
        ));

    ///////////////////////////////////////////////////////////////////
    /*-------------------- ModelDataに情報を追加する--------------------*/
    ///////////////////////////////////////////////////////////////////

    auto* modelData = &primitiveData_[PRIMITIVE_QUAD][(int)blendMode][(int)cullMode - 1];

    static std::string drawDataName;
    drawDataName.clear();
    drawDataName.reserve(128);
    drawDataName += "Q";
    drawDataName += blendName[(int)blendMode];
    drawDataName += cullName[(int)cullMode - 1];
    uint64_t hash = MyFunc::Hash64(drawDataName);

    // 無かったら作成する
    if(modelDrawData_.find(hash) == modelDrawData_.end()){
        // 頂点データの取得
        modelDrawData_[hash] = std::make_unique<ModelDrawData>();
        modelDrawData_[hash]->modelData = modelData;

        // パイプラインの設定
        modelDrawData_[hash]->pso =
            PSOManager::GetPSO(
                "CommonVSPipeline.pip",
                blendMode, cullMode, PolygonTopology::TRIANGLE
            );

        // 描画種類の設定
        modelDrawData_[hash]->drawOrder = (int8_t)DrawOrder::Quad;
        // 名前の設定
        modelDrawData_[hash]->name = drawDataName;
        // hashの設定
        modelDrawData_[hash]->hash = hash;
    }

    // Count
    auto* drawData = modelDrawData_[hash].get();
    int drawCount = drawData->totalDrawCount;
    int vertexCount = drawCount * 4;
    int indexCount = drawCount * 6;

    // vertexResource
    modelData->meshes.resize(1);
    auto& mesh = modelData->meshes[0];
    if(mesh.vertices.size() < vertexCount + 4){ mesh.vertices.resize(vertexCount + 4); }
    mesh.vertices[vertexCount] = VertexData(v1, texCoordV1, normalVec);
    mesh.vertices[vertexCount + 1] = VertexData(v2, texCoordV2, normalVec);
    mesh.vertices[vertexCount + 2] = VertexData(v3, texCoordV3, normalVec);
    mesh.vertices[vertexCount + 3] = VertexData(v4, texCoordV4, normalVec);

    //indexResource
    if(mesh.indices.size() <= indexCount){ mesh.indices.resize(indexCount + 6); }
    mesh.indices[indexCount] = vertexCount;
    mesh.indices[indexCount + 1] = vertexCount + 1;
    mesh.indices[indexCount + 2] = vertexCount + 3;
    mesh.indices[indexCount + 3] = vertexCount + 0;
    mesh.indices[indexCount + 4] = vertexCount + 3;
    mesh.indices[indexCount + 5] = vertexCount + 2;

    // materialResource
    if(modelData->materials.size() == 0){
        modelData->materials.resize(1);
        auto& baseMaterial = modelData->materials[0];
        baseMaterial.textureFilePath_ = "";
        baseMaterial.UV_scale_ = { 1.0f,1.0f,1.0f };
        baseMaterial.UV_offset_ = { 0.0f,0.0f,0.0f };
        baseMaterial.UV_translate_ = { 0.0f,0.0f,0.0f };
    }

    // 頂点数に合わせる(使わないけど規格化のために)
    mesh.vertexInfluences.resize(mesh.vertices.size());

    ///////////////////////////////////////////////////////////////////
    /*-------------------- 描画データに情報を書き込む --------------------*/
    ///////////////////////////////////////////////////////////////////

    // material
    drawData->materials.resize(1);// primitiveはmesh数1固定
    auto& material = drawData->materials.back();
    if(material.size() <= drawCount){ material.resize(drawCount + 1); }
    material[drawCount].color_ = color;
    material[drawCount].lightingType_ = lightingType;
    material[drawCount].uvTransform_ = uvTransform;
    material[drawCount].GH_ = GH;


    // transform
    for(const auto& [cameraName, camera] : pDxManager_->cameras_){
        auto& transform = drawData->transforms[cameraName];
        if(transform.size() <= drawCount){ transform.resize(drawCount + 1); }
        transform[drawCount].world = IdentityMat4();
        transform[drawCount].WVP = camera->GetViewProjectionMat();
        transform[drawCount].worldInverseTranspose = IdentityMat4();
    }

    // offsetResourceの数を更新
    auto& offsetData = drawData->offsetData;
    offsetData.resize(1);
    if(offsetData.back().size() <= drawCount){
        offsetData.back().resize(drawCount + 1);
    }

    // カウントを更新
    objCounts_[(int)DrawOrder::Quad]++;
    objCountCull_[(int)cullMode - 1]++;
    objCountBlend_[(int)blendMode]++;
    quadIndexCount_++;
    drawData->indexCount += 6;
    drawData->totalDrawCount++;
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                                                        //
//                                                   スプライトの追加                                                        //
//                                                                                                                        //
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void PolygonManager::AddSprite(
    const Vector2& size, const Matrix4x4& worldMat,
    uint32_t GH, const Vector4& color, const Matrix4x4& uvTransform, bool flipX, bool flipY,
    const Vector2& anchorPoint, const Vector2& clipLT, const Vector2& clipSize, BlendMode blendMode, D3D12_CULL_MODE cullMode,
    bool isStaticDraw, DrawLocation drawLocation, int32_t layer, bool isSystemDraw
){
    assert(spriteCount_ < kMaxSpriteCount);
    blendMode;


    ///////////////////////////////////////////////////////////////////
    /*---------------------- 頂点・法線などを求める ---------------------*/
    ///////////////////////////////////////////////////////////////////

    // 遠近
    float layerZ = 0.001f * layer;
    float zNear = pDxManager_->mainCamera_->GetZNear() - layerZ;
    float zfar = pDxManager_->mainCamera_->GetZFar() - layerZ;

    // スプライトの四頂点を格納する変数
    Vector4 v[4]{};

    // アンカーポイントを考慮したワールド座標を作成する行列
    Matrix4x4 adjustedWorldMat;
    if(MyMath::Length(clipSize) == 0.0f){
        adjustedWorldMat = Multiply(
            worldMat,
            TranslateMatrix({ size.x * -anchorPoint.x,size.y * -anchorPoint.y,0.0f })
        );

        // 4頂点
        v[0] = { size.x * -anchorPoint.x,size.y * -anchorPoint.y,zNear,1.0f };
        if(isSystemDraw){ v[0].z = zfar; }
        v[1] = v[0] + Vector4(size.x, 0.0f, 0.0f, 0.0f);
        v[2] = v[0] + Vector4(0.0f, size.y, 0.0f, 0.0f);
        v[3] = v[0] + Vector4(size.x, size.y, 0.0f, 0.0f);

    } else{// 描画範囲指定がある場合

        adjustedWorldMat = Multiply(
            worldMat,
            TranslateMatrix({ clipSize.x * -anchorPoint.x,clipSize.y * -anchorPoint.y,0.0f })
        );

        // 4頂点
        v[0] = { clipSize.x * -anchorPoint.x,clipSize.y * -anchorPoint.y,zNear,1.0f };
        if(isSystemDraw){ v[0].z = zfar; }
        v[1] = v[0] + Vector4(clipSize.x, 0.0f, 0.0f, 0.0f);
        v[2] = v[0] + Vector4(0.0f, clipSize.y, 0.0f, 0.0f);
        v[3] = v[0] + Vector4(clipSize.x, clipSize.y, 0.0f, 0.0f);
    }

    // 法線ベクトル
    Vector3 normalVec =
        MyMath::Normalize(MyMath::Cross(
            Vector3(v[1].x, v[1].y, v[1].z) - Vector3(v[0].x, v[0].y, v[0].z),
            Vector3(v[2].x, v[2].y, v[2].z) - Vector3(v[1].x, v[1].y, v[1].z),
            kScreen
        ));

    // 座標を変換する
    for(int i = 0; i < 4; i++){
        v[i] *= adjustedWorldMat;
    }


    ///////////////////////////////////////////////////////////////////
    /*-------------------- ModelDataに情報を追加する--------------------*/
    ///////////////////////////////////////////////////////////////////

    auto* modelData = isStaticDraw ?
        &primitiveData_[PRIMITIVE_STATIC_SPRITE][(int)blendMode][(int)cullMode - 1] :
        &primitiveData_[PRIMITIVE_SPRITE][(int)blendMode][(int)cullMode - 1];

    // 背景描画の場合は背景用のモデルデータを使用
    modelData = drawLocation == DrawLocation::Back ?
        &primitiveData_[PRIMITIVE_BACKSPRITE][(int)blendMode][(int)cullMode - 1] : modelData;

    static std::string drawDataName;
    // 背面描画の場合はzFarに設定
    if(drawLocation == DrawLocation::Back && !isStaticDraw){
        v[0].z = zfar;
        v[1].z = zfar;
        v[2].z = zfar;
        v[3].z = zfar;
    }

    // 描画データの取得
    drawDataName.clear();
    drawDataName.reserve(128);
    drawDataName += isStaticDraw ? "SSP" : "SP";
    drawLocation == DrawLocation::Back ? drawDataName = "BSP" : drawDataName;
    drawDataName += blendName[(int)blendMode];
    drawDataName += cullName[(int)cullMode - 1];
    uint64_t hash = MyFunc::Hash64(drawDataName);

    // もし該当する描画データがなければ作成する
    if(modelDrawData_.find(hash) == modelDrawData_.end()){
        // 頂点データの取得
        modelDrawData_[hash] = std::make_unique<ModelDrawData>();
        modelDrawData_[hash]->modelData = modelData;

        // パイプラインの設定
        modelDrawData_[hash]->pso =
            PSOManager::GetPSO(
                "CommonVSPipeline.pip",
                blendMode, cullMode, PolygonTopology::TRIANGLE
            );

        // 描画種類の設定
        modelDrawData_[hash]->drawOrder = isStaticDraw ?
            (int8_t)DrawOrder::StaticSprite : (int8_t)DrawOrder::Sprite;
        modelDrawData_[hash]->drawOrder = drawLocation == DrawLocation::Back ?
            (int8_t)DrawOrder::BackSprite : modelDrawData_[hash]->drawOrder;
        // 名前の設定
        modelDrawData_[hash]->name = drawDataName;
        // hashの設定
        modelDrawData_[hash]->hash = hash;
    }

    //count
    auto* drawData = modelDrawData_[hash].get();
    int drawCount = drawData->totalDrawCount;
    int vertexCount = drawCount * 4;
    int indexCount = drawCount * 6;
    // vertexResource
    modelData->meshes.resize(1);// primitiveはmesh数1固定
    auto& mesh = modelData->meshes[0];
    if(mesh.vertices.size() <= vertexCount){ mesh.vertices.resize(vertexCount + 4); }

    if(MyMath::Length(clipSize) == 0.0f){// 描画範囲指定がない場合
        mesh.vertices[vertexCount] = VertexData(v[0], Vector2(0.0f, 0.0f), normalVec);
        mesh.vertices[vertexCount + 1] = VertexData(v[1], Vector2(1.0f, 0.0f), normalVec);
        mesh.vertices[vertexCount + 2] = VertexData(v[2], Vector2(0.0, 1.0f), normalVec);
        mesh.vertices[vertexCount + 3] = VertexData(v[3], Vector2(1.0f, 1.0f), normalVec);

    } else{// 描画範囲指定がある場合
        mesh.vertices[vertexCount] = VertexData(v[0], Vector2(clipLT.x / size.x, clipLT.y / size.y), normalVec);
        mesh.vertices[vertexCount + 1] = VertexData(v[1], Vector2((clipLT.x + clipSize.x) / size.x, clipLT.y / size.y), normalVec);
        mesh.vertices[vertexCount + 2] = VertexData(v[2], Vector2(clipLT.x / size.x, (clipLT.y + clipSize.y) / size.y), normalVec);
        mesh.vertices[vertexCount + 3] = VertexData(v[3], Vector2((clipLT.x + clipSize.x) / size.x, (clipLT.y + clipSize.y) / size.y), normalVec);
    }

    // 反転の指定がある場合
    if(flipX){
        Vector2 temp[2] = { mesh.vertices[vertexCount].texcoord_, mesh.vertices[vertexCount + 2].texcoord_ };
        mesh.vertices[vertexCount].texcoord_.x = mesh.vertices[vertexCount + 1].texcoord_.x;
        mesh.vertices[vertexCount + 1].texcoord_.x = temp[0].x;
        mesh.vertices[vertexCount + 2].texcoord_.x = mesh.vertices[vertexCount + 3].texcoord_.x;
        mesh.vertices[vertexCount + 3].texcoord_.x = temp[1].x;
    }

    if(flipY){
        Vector2 temp[2] = { mesh.vertices[vertexCount].texcoord_, mesh.vertices[vertexCount + 1].texcoord_ };
        mesh.vertices[vertexCount].texcoord_.y = mesh.vertices[vertexCount + 2].texcoord_.y;
        mesh.vertices[vertexCount + 1].texcoord_.y = mesh.vertices[vertexCount + 3].texcoord_.y;
        mesh.vertices[vertexCount + 2].texcoord_.y = temp[0].y;
        mesh.vertices[vertexCount + 3].texcoord_.y = temp[1].y;
    }

    //indexResource
    if(mesh.indices.size() <= indexCount){ mesh.indices.resize(indexCount + 6); }
    mesh.indices[indexCount] = vertexCount + 0;
    mesh.indices[indexCount + 1] = vertexCount + 1;
    mesh.indices[indexCount + 2] = vertexCount + 3;
    mesh.indices[indexCount + 3] = vertexCount + 0;
    mesh.indices[indexCount + 4] = vertexCount + 3;
    mesh.indices[indexCount + 5] = vertexCount + 2;

    // materialResource
    if(modelData->materials.size() == 0){
        modelData->materials.resize(1);
        auto& baseMaterial = modelData->materials[0];
        baseMaterial.textureFilePath_ = "";
        baseMaterial.UV_scale_ = { 1.0f,1.0f,1.0f };
        baseMaterial.UV_offset_ = { 0.0f,0.0f,0.0f };
        baseMaterial.UV_translate_ = { 0.0f,0.0f,0.0f };
    }

    // 頂点数に合わせる(使わないけど規格化のために)
    mesh.vertexInfluences.resize(mesh.vertices.size());

    ///////////////////////////////////////////////////////////////////
    /*-------------------- 描画データに情報を書き込む --------------------*/
    ///////////////////////////////////////////////////////////////////

    // material
    drawData->materials.resize(1);
    auto& material = drawData->materials.back();
    if(material.size() <= drawCount){ material.resize(drawCount + 1); }
    material[drawCount].color_ = color;
    material[drawCount].lightingType_ = LIGHTINGTYPE_NONE;
    material[drawCount].uvTransform_ = uvTransform;
    material[drawCount].GH_ = GH;

    // transform
    for(const auto& [cameraName, camera] : pDxManager_->cameras_){
        auto& transform = drawData->transforms[cameraName];
        if(transform.size() <= drawCount){ transform.resize(drawCount + 1); }
        transform[drawCount].world = IdentityMat4();
        transform[drawCount].WVP = camera->GetProjectionMat2D();
        transform[drawCount].worldInverseTranspose = IdentityMat4();
    }

    // offsetResourceの数を更新
    auto& offsetData = drawData->offsetData;
    offsetData.resize(1);
    if(offsetData.back().size() <= drawCount){
        offsetData.back().resize(drawCount + 1);
    }


    // カウントを更新
    if(isStaticDraw){
        objCounts_[(int)DrawOrder::StaticSprite]++;
    } else{
        if(drawLocation == DrawLocation::Back){
            objCounts_[(int)DrawOrder::BackSprite]++;
        } else{
            objCounts_[(int)DrawOrder::Sprite]++;
        }
    }


    objCountCull_[(int)cullMode - 1]++;
    objCountBlend_[(int)blendMode]++;
    spriteCount_++;
    drawData->indexCount += 6;
    drawData->totalDrawCount++;

}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                                                        //
//                                                     モデルの追加                                                         //
//                                                                                                                        //
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void PolygonManager::AddModel(Model* model){

    if(!model){ return; }

    //////////////////////////////////////////////////////////////////////////
    // モデルの名前の決定
    //////////////////////////////////////////////////////////////////////////
    static std::string modelName;
    modelName.clear();
    modelName.reserve(128);
    modelName = model->modelName_;
    if(modelName == ""){ return; }
    modelName += blendName[(int)model->blendMode_];
    modelName += cullName[(int)model->cullMode_ - 1];


    if(model->isAnimation_){
        // アニメーションしているモデルは別のデータとして扱う
        modelName += "_Skinning";
    } else if(model->isParticle_){
        // パーティクルは別のデータとして扱う
        modelName += "_Particle";
    }

    //////////////////////////////////////////////////////////////////////////
    //                     モデル描画データがが存在しないときのみ
    //////////////////////////////////////////////////////////////////////////
    uint64_t hash = MyFunc::Hash64(modelName);
    if(modelDrawData_.find(hash) == modelDrawData_.end()){
        // 頂点データの取得
        modelDrawData_[hash] = std::make_unique<ModelDrawData>();
        modelDrawData_[hash]->modelData = ModelManager::GetModelData(model->modelName_);
        // 名前の設定
        modelDrawData_[hash]->name = modelName;
        // hashの設定
        modelDrawData_[hash]->hash = hash;

        // 描画種類の設定
        if(model->isAnimation_){
            modelDrawData_[hash]->drawOrder = (int)DrawOrder::AnimationModel;
        } else{
            modelDrawData_[hash]->drawOrder = (int)DrawOrder::Model;
        }
        if(model->isParticle_){
            modelDrawData_[hash]->drawOrder = (int)DrawOrder::Particle;
        }

        // パイプラインの設定
        if(model->isAnimation_){
            modelDrawData_[hash]->pso =
                PSOManager::GetPSO(
                    "SkinningVSPipeline.pip",
                    model->blendMode_, model->cullMode_, PolygonTopology::TRIANGLE
                );
        } else{
            modelDrawData_[hash]->pso =
                PSOManager::GetPSO(
                    "CommonVSPipeline.pip",
                    model->blendMode_, model->cullMode_, PolygonTopology::TRIANGLE
                );
        }
    }

    // サイズ確保
    auto& drawData = modelDrawData_[hash];
    int meshSize = (int)drawData->modelData->meshes.size();

    // count(instance数)
    int drawCount = drawData->totalDrawCount;

    /////////////////////////////////////////////////////////////////////////
    //                          materialResourceの設定
    /////////////////////////////////////////////////////////////////////////

    // 各meshごとにinstance数分のマテリアルを持つ。ここではmesh数分の配列を確保
    drawData->materials.resize(meshSize);
    for(int meshIdx = 0; meshIdx < meshSize; meshIdx++){

        // ここではinstance数分のマテリアルを確保
        auto& material = drawData->materials[meshIdx];
        if(material.size() <= drawCount){ material.resize(drawCount + 1); }

        // マテリアルの設定
        material[drawCount].color_ = model->masterColor_ * model->materials_[meshIdx].color;
        material[drawCount].shininess_ = model->materials_[meshIdx].shininess;
        material[drawCount].lightingType_ = model->lightingType_;
        material[drawCount].uvTransform_ = model->materials_[meshIdx].uvTransform;
        material[drawCount].GH_ = model->materials_[meshIdx].GH;
        material[drawCount].environmentCoef_ = model->materials_[meshIdx].environmentCoef;
    }

    //////////////////////////////////////////////////////////////////////////
    //                          transformResourceの設定
    //////////////////////////////////////////////////////////////////////////

    // instance数分のtransformを確保(meshごとには必要ない)
    Matrix4x4 wvp;
    Matrix4x4 worldInverseTranspose = model->isAnimation_ ?
        Transpose(InverseMatrix(model->GetWorldMat())) : IdentityMat4();
    for(const auto& [cameraName, camera] : pDxManager_->cameras_){

        auto& transform = drawData->transforms[cameraName];
        if(transform.size() <= drawCount){ transform.resize(drawCount + 1); }

        // transformの設定
        wvp = Multiply(
            model->worldMat_,
            camera->GetViewProjectionMat()
        );

        transform[drawCount].world = model->worldMat_;
        transform[drawCount].WVP = wvp;
        transform[drawCount].worldInverseTranspose = worldInverseTranspose;
    }

    //////////////////////////////////////////////////////////////////////////
    //                              offset情報の設定
    //////////////////////////////////////////////////////////////////////////

    // mesh数 * instance数分のoffsetを確保(各instanceごとにオフセットが必要なため)
    drawData->offsetData.resize(meshSize);
    for(int meshIdx = 0; meshIdx < meshSize; meshIdx++){
        auto& offsetData = drawData->offsetData[meshIdx];
        offsetData.resize(drawCount + 1);
    }

    //////////////////////////////////////////////////////////////////////////
    //                              palette情報の設定
    //////////////////////////////////////////////////////////////////////////
    if(model->isAnimation_){
        // instance数分のpalette(行列などの入った配列)を確保
        auto& palette = drawData->paletteData;
        if(palette.size() <= drawCount){ palette.resize(drawCount + 1); }
        palette[drawCount] = model->palette_;
    }

    //////////////////////////////////////////////////////////////////////////
    //                              カウントの更新
    //////////////////////////////////////////////////////////////////////////

    // 要素数を更新
    if(model->isAnimation_){
        objCounts_[(int)DrawOrder::AnimationModel]++;
    } else{
        model->isParticle_ ? objCounts_[(int)DrawOrder::Particle]++ : objCounts_[(int)DrawOrder::Model]++;
    }

    objCountCull_[(int)model->cullMode_ - 1]++;
    objCountBlend_[(int)model->blendMode_]++;
    drawData->totalDrawCount++;
    modelIndexCount_++;

    //////////////////////////////////////////////////////////////////////////
    //                  モデルのスケルトンを描画(オプションがあれば)
    //////////////////////////////////////////////////////////////////////////
    //if(model->isSkeletonVisible_){

    //    const auto& modeldata = ModelManager::GetModelData(model->modelName_);
    //    const ModelSkeleton& skeleton = ModelManager::AnimatedSkeleton(
    //        modeldata->animations[model->animationName_],
    //        modeldata->defaultSkeleton,
    //        model->animationTime_
    //    );

    //    for(int i = 0; i < skeleton.joints.size(); i++){

    //        if(skeleton.joints[i].parent){
    //            Vector3 point[2];
    //            point[0] = Vector3(0.0f, 0.0f, 0.0f) * skeleton.joints[i].skeletonSpaceMatrix;
    //            point[0] = Multiply(point[0], wvp);

    //            point[1] = Vector3(0.0f, 0.0f, 0.0f) * skeleton.joints[skeleton.joints[i].parent.value()].skeletonSpaceMatrix;
    //            point[1] = Multiply(point[1], wvp);


    //            SEED::DrawLine(point[0], point[1], { 0.0f,0.0f,1.0f,1.0f });
    //        }
    //    }
    //}
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                                                        //
//                                                     ラインの追加                                                         //
//                                                                                                                        //
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void PolygonManager::AddLine(
    const Vector4& v1, const Vector4& v2, const Matrix4x4& worldMat,
    const Vector4& color, bool view3D, BlendMode blendMode, bool isStaticDraw,
    DrawLocation drawLocation, int32_t layer
){

    assert(lineCount_ < kMaxLineCount_);
    Vector3 normalVec = { 0.0f,0.0f,-1.0f };
    Vector4 v[2]{ v1,v2 };
    float layerZ = 0.001f * layer;

    // レイヤー、描画場所に応じたZ値に設定
    if(drawLocation != DrawLocation::Not2D){
        if(drawLocation == DrawLocation::Back){
            float zFar = pDxManager_->mainCamera_->GetZFar() - layerZ;
            v[0].z = zFar;
            v[1].z = zFar;
        } else{
            float zNear = pDxManager_->mainCamera_->GetZNear() - layerZ;
            v[0].z = zNear;
            v[1].z = zNear;
        }
    }

    ///////////////////////////////////////////////////////////////////
    /*-------------------- ModelDataに情報を追加する--------------------*/
    ///////////////////////////////////////////////////////////////////

    auto* modelData = view3D ?
        &primitiveData_[PRIMITIVE_LINE][(int)blendMode][0] :
        &primitiveData_[PRIMITIVE_LINE2D][(int)blendMode][0];

    if(isStaticDraw){
        modelData = &primitiveData_[PRIMITIVE_STATIC_LINE2D][(int)blendMode][0];
    }

    // 描画データの取得
    static std::string drawDataName[2];
    drawDataName[0].clear();
    drawDataName[0].reserve(128);
    drawDataName[0] += "L";
    drawDataName[0] += blendName[(int)blendMode];
    drawDataName[0] += cullName[0];
    drawDataName[1].clear();
    drawDataName[1].reserve(128);
    drawDataName[1] += isStaticDraw ? "SL2" : "L2";
    drawDataName[1] += blendName[(int)blendMode];
    drawDataName[1] += cullName[0];
    uint64_t hash = view3D ? MyFunc::Hash64(drawDataName[0]) : MyFunc::Hash64(drawDataName[1]);

    // もし該当する描画データがなければ作成する
    if(modelDrawData_.find(hash) == modelDrawData_.end()){
        // 頂点データの取得
        modelDrawData_[hash] = std::make_unique<ModelDrawData>();
        modelDrawData_[hash]->modelData = modelData;

        // パイプラインの設定
        modelDrawData_[hash]->pso =
            PSOManager::GetPSO(
                "CommonVSPipeline.pip",
                blendMode, D3D12_CULL_MODE_NONE, PolygonTopology::LINE
            );

        // 描画種類の設定
        modelDrawData_[hash]->drawOrder = isStaticDraw ?
            (int8_t)DrawOrder::StaticLine2D : (int8_t)DrawOrder::Line2D;
        if(view3D){
            modelDrawData_[hash]->drawOrder = (int8_t)DrawOrder::Line;
        }
        // 名前の設定
        modelDrawData_[hash]->name = view3D ? drawDataName[0] : drawDataName[1];
        // hashの設定
        modelDrawData_[hash]->hash = hash;
    }


    // Count
    auto* drawData = modelDrawData_[hash].get();
    int drawCount = drawData->totalDrawCount;
    int vertexCount = drawCount * 2;
    int indexCount = drawCount * 2;

    // vertexResource
    modelData->meshes.resize(1);
    auto& mesh = modelData->meshes[0];
    if(mesh.vertices.size() <= vertexCount){ mesh.vertices.resize(vertexCount + 2); }
    mesh.vertices[vertexCount] = VertexData(v[0], Vector2(0.0f, 0.0f), normalVec);
    mesh.vertices[vertexCount + 1] = VertexData(v[1], Vector2(1.0f, 1.0f), normalVec);
    //indexResource
    if(mesh.indices.size() <= indexCount){ mesh.indices.resize(indexCount + 2); }
    mesh.indices[indexCount] = indexCount;
    mesh.indices[indexCount + 1] = indexCount + 1;
    // materialResource
    if(modelData->materials.size() == 0){
        modelData->materials.resize(1);
        auto& baseMaterial = modelData->materials[0];
        baseMaterial.textureFilePath_ = "";
        baseMaterial.UV_scale_ = { 1.0f,1.0f,1.0f };
        baseMaterial.UV_offset_ = { 0.0f,0.0f,0.0f };
        baseMaterial.UV_translate_ = { 0.0f,0.0f,0.0f };
    }
    // 頂点数に合わせる(使わないけど規格化のために)
    mesh.vertexInfluences.resize(mesh.vertices.size());

    ///////////////////////////////////////////////////////////////////
    /*-------------------- 描画データに情報を書き込む --------------------*/
    ///////////////////////////////////////////////////////////////////

    // material (mesh * instance数分)
    auto& material = drawData->materials;
    static int GH = TextureManager::LoadTexture("DefaultAssets/white1x1.png");
    material.resize(1);// primitiveはmesh数1固定
    if(material[0].size() <= drawCount){ material[0].resize(drawCount + 1); }
    material[0][drawCount].color_ = color;
    material[0][drawCount].lightingType_ = LIGHTINGTYPE_NONE;
    material[0][drawCount].uvTransform_ = IdentityMat4();
    material[0][drawCount].GH_ = GH;

    // transform (instance数分)
    Matrix4x4 worldInverseTranspose = IdentityMat4();
    for(const auto& [cameraName, camera] : pDxManager_->cameras_){
        auto& transform = drawData->transforms[cameraName];
        if(transform.size() <= drawCount){ transform.resize(drawCount + 1); }
        transform[drawCount].world = worldMat;
        transform[drawCount].WVP = view3D ? camera->GetViewProjectionMat() : camera->GetProjectionMat2D();
        transform[drawCount].worldInverseTranspose = worldInverseTranspose;
    }

    // offsetResourceの数を更新 (instance数分)
    auto& offsetData = drawData->offsetData;
    offsetData.resize(1);
    if(offsetData.back().size() <= drawCount){
        offsetData.back().resize(drawCount + 1);
    }

    //////////////////////////////////////////////////////////////////////////
    //                              カウントの更新
    //////////////////////////////////////////////////////////////////////////

    // カウントを更新
    if(isStaticDraw){
        objCounts_[(int)DrawOrder::StaticLine2D]++;
    } else{
        if(view3D){
            objCounts_[(int)DrawOrder::Line]++;
        } else{
            objCounts_[(int)DrawOrder::Line2D]++;
        }
    }

    objCountCull_[0]++;
    objCountBlend_[(int)blendMode]++;
    lineCount_++;
    drawData->indexCount += 2;
    drawData->totalDrawCount++;
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                                                        //
//                                    オフスクリーンの描画結果を描画処理に追加する関数                                            //
//                                                                                                                        //
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void PolygonManager::AddOffscreenResult(uint32_t GH, BlendMode blendMode){

    Vector2 windowSize = { float(kWindowSizeX),float(kWindowSizeY) };
    float scaleRate = float(pDxManager_->GetPreResolutionRate());
    Matrix4x4 uvTransform = AffineMatrix({ scaleRate,scaleRate,0.0f }, { 0.0f,0.0f,0.0f }, { 0.0f,0.0f,0.0f });
    float zfar = pDxManager_->mainCamera_->GetZFar();

    // 4頂点
    Vector4 v[4]{
        { 0.0f,0.0f,zfar,1.0f },
        { windowSize.x,0.0f,zfar,1.0f },
        { 0.0f,windowSize.y,zfar,1.0f },
        { windowSize.x,windowSize.y,zfar,1.0f }
    };

    ///////////////////////////////////////////////////////////////////
    /*-------------------- ModelDataに情報を追加する--------------------*/
    ///////////////////////////////////////////////////////////////////

    // 描画データの取得
    auto* modelData = &primitiveData_[PRIMITIVE_OFFSCREEN][(int)blendMode][0];
    static std::string drawDataName;
    drawDataName.clear();
    drawDataName.reserve(128);
    drawDataName += "O";
    drawDataName += blendName[(int)blendMode];
    drawDataName += cullName[0];
    uint64_t hash = MyFunc::Hash64(drawDataName);

    // もし該当する描画データがなければ作成する
    if(modelDrawData_.find(hash) == modelDrawData_.end()){
        modelDrawData_[hash] = std::make_unique<ModelDrawData>();
        modelDrawData_[hash]->modelData = modelData;
        modelDrawData_[hash]->pso =
            PSOManager::GetPSO(
                "CommonVSPipeline.pip",
                blendMode, D3D12_CULL_MODE_BACK, PolygonTopology::TRIANGLE
            );
        modelDrawData_[hash]->drawOrder = (int8_t)DrawOrder::Offscreen;
        modelDrawData_[hash]->name = drawDataName;
        // hashの設定
        modelDrawData_[hash]->hash = hash;
    }

    // Count
    auto* drawData = modelDrawData_[hash].get();
    int drawCount = drawData->totalDrawCount;
    int vertexCount = drawCount * 4;
    int indexCount = drawCount * 6;

    // vertexResource
    modelData->meshes.resize(1);
    auto& mesh = modelData->meshes[0];
    if(mesh.vertices.size() <= vertexCount){ mesh.vertices.resize(vertexCount + 4); }
    mesh.vertices[vertexCount] = VertexData(v[0], Vector2(0.0f, 0.0f), { 0.0f,0.0f,-1.0f });
    mesh.vertices[vertexCount + 1] = VertexData(v[1], Vector2(1.0f, 0.0f), { 0.0f,0.0f,-1.0f });
    mesh.vertices[vertexCount + 2] = VertexData(v[2], Vector2(0.0f, 1.0f), { 0.0f,0.0f,-1.0f });
    mesh.vertices[vertexCount + 3] = VertexData(v[3], Vector2(1.0f, 1.0f), { 0.0f,0.0f,-1.0f });

    //indexResource
    if(mesh.indices.size() <= indexCount){ mesh.indices.resize(indexCount + 6); }
    mesh.indices[indexCount] = drawData->indexCount + 0;
    mesh.indices[indexCount + 1] = drawData->indexCount + 1;
    mesh.indices[indexCount + 2] = drawData->indexCount + 3;
    mesh.indices[indexCount + 3] = drawData->indexCount + 0;
    mesh.indices[indexCount + 4] = drawData->indexCount + 3;
    mesh.indices[indexCount + 5] = drawData->indexCount + 2;

    // materialResource
    if(modelData->materials.size() == 0){
        modelData->materials.resize(1);
        auto& baseMaterial = modelData->materials[0];
        baseMaterial.textureFilePath_ = "";
        baseMaterial.UV_scale_ = { 1.0f,1.0f,1.0f };
        baseMaterial.UV_offset_ = { 0.0f,0.0f,0.0f };
        baseMaterial.UV_translate_ = { 0.0f,0.0f,0.0f };
    }

    // 頂点数に合わせる(使わないけど規格化のために)
    mesh.vertexInfluences.resize(mesh.vertices.size());

    // material
    drawData->materials.resize(1);
    auto& material = drawData->materials.back();
    if(material.size() <= drawCount){ material.resize(drawCount + 1); }
    material[drawCount].color_ = { 1.0f,1.0f,1.0f,1.0f };
    material[drawCount].lightingType_ = LIGHTINGTYPE_NONE;
    material[drawCount].uvTransform_ = uvTransform;
    material[drawCount].GH_ = GH;

    // transform
    for(const auto& [cameraName, camera] : pDxManager_->cameras_){
        auto& transform = drawData->transforms[cameraName];
        if(transform.size() <= drawCount){ transform.resize(drawCount + 1); }
        transform[drawCount].world = IdentityMat4();
        transform[drawCount].WVP = camera->GetProjectionMat2D();
        transform[drawCount].worldInverseTranspose = IdentityMat4();
    }

    // offsetResourceの数を更新
    auto& offsetData = drawData->offsetData;
    offsetData.resize(1);
    if(offsetData.back().size() <= drawCount){
        offsetData.back().resize(drawCount + 1);
    }

    // カウントを更新
    objCounts_[(int)DrawOrder::Offscreen]++;
    objCountBlend_[(int)blendMode]++;
    objCountCull_[0]++;
    drawData->indexCount += 6;
    drawData->totalDrawCount++;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                                                        //
//                                                     SkyBoxの追加                                                        //
//                                                                                                                        //
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void PolygonManager::AddSkyBox(){

    //////////////////////////////////////////////////////////////////////////
    // モデルの名前の決定
    //////////////////////////////////////////////////////////////////////////

    uint64_t hash = MyFunc::Hash64("System_SkyBox");
    if(modelDrawData_.find(hash) == modelDrawData_.end()){
        // 頂点データの取得
        modelDrawData_[hash] = std::make_unique<ModelDrawData>();
        ModelManager::LoadModel("DefaultAssets/SkyBox/SkyBox.obj");
        modelDrawData_[hash]->modelData = ModelManager::GetModelData("DefaultAssets/SkyBox/SkyBox.obj");
        // 名前の設定
        modelDrawData_[hash]->name = "System_SkyBox";
        // hashの設定
        modelDrawData_[hash]->hash = hash;

        // 描画種類の設定
        modelDrawData_[hash]->drawOrder = (int)DrawOrder::SkyBox;

        // パイプラインの設定
        modelDrawData_[hash]->pso =
            PSOManager::GetPSO(
                "SkyBoxVSPipeline.pip",
                BlendMode::NORMAL, D3D12_CULL_MODE_BACK, PolygonTopology::TRIANGLE
            );
    }

    // サイズ確保
    auto& drawData = modelDrawData_[hash];
    int meshSize = (int)drawData->modelData->meshes.size();

    // count(instance数)
    int drawCount = drawData->totalDrawCount;

    /////////////////////////////////////////////////////////////////////////
    //                          materialResourceの設定
    /////////////////////////////////////////////////////////////////////////

    // 各meshごとにinstance数分のマテリアルを持つ。ここではmesh数分の配列を確保
    drawData->materials.resize(meshSize);
    for(int meshIdx = 0; meshIdx < meshSize; meshIdx++){

        // ここではinstance数分のマテリアルを確保
        auto& material = drawData->materials[meshIdx];
        if(material.size() <= drawCount){ material.resize(drawCount + 1); }

        // マテリアルの設定
        material[drawCount].color_ = SkyBox::color_;
        material[drawCount].shininess_ = 1.0f;
        material[drawCount].lightingType_ = LIGHTINGTYPE_NONE;
        material[drawCount].uvTransform_ = IdentityMat4();
        material[drawCount].GH_ = SkyBox::textureGH_;
    }

    //////////////////////////////////////////////////////////////////////////
    //                          transformResourceの設定
    //////////////////////////////////////////////////////////////////////////

    // instance数分のtransformを確保(meshごとには必要ない)
    Matrix4x4 wvp;
    Matrix4x4 worldInverseTranspose = IdentityMat4();
    Matrix4x4 skyBoxMat = AffineMatrix(Vector3(SkyBox::scale_), Quaternion(), SkyBox::translate_);
    for(const auto& [cameraName, camera] : pDxManager_->cameras_){

        auto& transform = drawData->transforms[cameraName];
        if(transform.size() <= drawCount){ transform.resize(drawCount + 1); }

        Matrix4x4 worldMat;
        if(SkyBox::isFollowCameraPos_){
            Matrix4x4 cameraTranslateMat = TranslateMatrix(camera->GetTranslation());
            worldMat = skyBoxMat * cameraTranslateMat;
        } else{
            worldMat = skyBoxMat;
        }

        // transformの設定
        wvp = Multiply(
            worldMat,
            camera->GetViewProjectionMat()
        );

        transform[drawCount].world = worldMat;
        transform[drawCount].WVP = wvp;
        transform[drawCount].worldInverseTranspose = worldInverseTranspose;
    }

    //////////////////////////////////////////////////////////////////////////
    //                              offset情報の設定
    //////////////////////////////////////////////////////////////////////////

    // mesh数 * instance数分のoffsetを確保(各instanceごとにオフセットが必要なため)
    drawData->offsetData.resize(meshSize);
    for(int meshIdx = 0; meshIdx < meshSize; meshIdx++){
        auto& offsetData = drawData->offsetData[meshIdx];
        offsetData.resize(drawCount + 1);
    }

    //////////////////////////////////////////////////////////////////////////
    //                              カウントの更新
    //////////////////////////////////////////////////////////////////////////

    // 要素数を更新
    objCounts_[(int)DrawOrder::SkyBox]++;

    objCountCull_[(int)D3D12_CULL_MODE_BACK - 1]++;
    objCountBlend_[(int)BlendMode::NORMAL]++;
    drawData->totalDrawCount++;
    modelIndexCount_++;
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                                                        //
//                                        Resourceに描画データを書き込む関数                                                  //
//                                                                                                                        //
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void PolygonManager::WriteRenderData(){
    // モノがなければreturn
    if(modelDrawData_.size() == 0){ return; }
    int instanceCountAll = 0;
    int indexCountAll = 0;
    int animationJointCount = 0;
    vertexCountAll = 0;

    // 一列に格納する用の配列
    std::vector<MaterialForGPU> materialArray;


    for(auto& drawData : modelDrawData_){

        // カメラごとのtransformのサイズが違う場合、小さいほうに合わせる
        int max = 0;
        for(const auto& [cameraName, transforms] : drawData.second->transforms){
            if(transforms.size() > max){ max = (int)transforms.size(); }
        }

        // 各カメラのtransformのサイズを揃える
        for(auto& [cameraName, transforms] : drawData.second->transforms){
            if(transforms.size() < max){
                transforms.resize(max);
            }
        }

    }

    ///////////////////////////////////////////////////////////////////////////////////////////////

    /*                                ライティング・カメラ情報を書き込む                              */

    ///////////////////////////////////////////////////////////////////////////////////////////////

    // ライティング情報の書き込み
    std::memcpy(
        mapDirectionalLightData,
        directionalLights_.data(),
        sizeof(DirectionalLight) * directionalLights_.size()
    );

    std::memcpy(
        mapPointLightData,
        pointLights_.data(),
        sizeof(PointLight) * pointLights_.size()
    );

    std::memcpy(
        mapSpotLightData,
        spotLights_.data(),
        sizeof(SpotLight) * spotLights_.size()
    );

    // ライティング情報の数を格納
    directionalLightCount_ = (int)directionalLights_.size();
    pointLightCount_ = (int)pointLights_.size();
    spotLightCount_ = (int)spotLights_.size();

    ///////////////////////////////////////////////////////////////////////////////////////////////

    /*                                      頂点情報を書き込む                                      */

    ///////////////////////////////////////////////////////////////////////////////////////////////


    for(auto itr = modelDrawData_.begin(); itr != modelDrawData_.end();){

        int modelVertexCount = 0;
        int modelIndexCount = 0;
        auto& modelData = *itr;
        auto& item = modelData.second;

        // 要素が0なら削除してcontinue
        if(item->totalDrawCount == 0){
            itr = modelDrawData_.erase(itr);
            continue;
        }

        ModelDrawData::modelSwitchIdx_Vertex[modelData.first] = vertexCountAll;
        ModelDrawData::modelSwitchIdx_Index[modelData.first] = indexCountAll;

        // 描画リストに追加
        drawLists_[item->pso].push_back(item.get());

        /*--------------------------------------*/
        //      modelの頂点,index情報を書き込む
        /*--------------------------------------*/
        for(int meshIdx = 0; meshIdx < item->modelData->meshes.size(); meshIdx++){

            int meshVertexCount = (int)item->modelData->meshes[meshIdx].vertices.size();
            int meshIndexCount = (int)item->modelData->meshes[meshIdx].indices.size();

            item->meshSwitchIdx_Vertex[meshIdx] = modelVertexCount;// メッシュが切り替わる頂点番号を記録
            item->meshSwitchIdx_Index[meshIdx] = modelIndexCount;// メッシュが切り替わるインデックス番号を記録

            // 頂点情報を書き込む
            std::memcpy(
                mapVertexData + ModelDrawData::modelSwitchIdx_Vertex[modelData.first] + item->meshSwitchIdx_Vertex[meshIdx],// に
                item->modelData->meshes[meshIdx].vertices.data(),// から
                sizeof(VertexData) * meshVertexCount// のサイズコピー
            );


            // インデックス情報を書き込む
            std::memcpy(
                mapIndexData + ModelDrawData::modelSwitchIdx_Index[modelData.first] + item->meshSwitchIdx_Index[meshIdx],// に
                item->modelData->meshes[meshIdx].indices.data(),// から
                sizeof(uint32_t) * meshIndexCount// のサイズコピー
            );

            // 頂点の影響度(VertexInfluence)を書き込む
            std::memcpy(
                mapVertexInfluenceData + ModelDrawData::modelSwitchIdx_Vertex[modelData.first] + item->meshSwitchIdx_Vertex[meshIdx],// に
                item->modelData->meshes[meshIdx].vertexInfluences.data(),// から
                sizeof(VertexInfluence) * meshVertexCount// のサイズコピー
            );


            // 数をインクリメント
            modelVertexCount += meshVertexCount;
            modelIndexCount += meshIndexCount;
            vertexCountAll += meshVertexCount;
            indexCountAll += meshIndexCount;
        }

        // イテレータを進める
        itr++;
    }


    // drawListsをorderedListsにblendMode順に並べていく
    for(int i = 0; i < (int)BlendMode::kBlendModeCount; i++){
        for(auto& drawData : drawLists_){
            if(drawData.first->GetBlendMode() == (BlendMode)i){
                orderedDrawLists_.push_back(&drawData.second);
            }
        }
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////

    /*                          transform、material情報・を書き込む                                 */

    ///////////////////////////////////////////////////////////////////////////////////////////////

    for(auto& drawList : orderedDrawLists_){
        for(auto& drawData : *drawList){

            // 各モデルの現在のインスタンス数確認
            int instanceCount = (int)drawData->transforms["default"].size();
            int jointSize = static_cast<int>(drawData->modelData->defaultSkeleton.joints.size());
            if(instanceCount == 0){ continue; }// インスタンスがない場合はスキップ
            if((int)drawData->materials.size() == 0){ continue; }// プリミティブ用の例外処理

            for(int meshIdx = 0; meshIdx < drawData->modelData->meshes.size(); meshIdx++){

                /*--------------------------------------*/
                /*        materialを一列に格納            */
                /*--------------------------------------*/

                // すべてのマテリアルを一列に並べる
                materialArray.insert(
                    materialArray.end(),
                    drawData->materials[meshIdx].begin(),
                    drawData->materials[meshIdx].end()
                );
            }


            /*--------------------------------------*/
            /*      アニメーション用の情報を格納        */
            /*--------------------------------------*/
            if(drawData->drawOrder == (int)DrawOrder::AnimationModel){

                for(int instanceIdx = 0; instanceIdx < drawData->paletteData.size(); instanceIdx++){
                    std::memcpy(
                        mapPaletteData + animationJointCount + (jointSize * instanceIdx),
                        drawData->paletteData[instanceIdx].data(),
                        sizeof(WellForGPU) * drawData->paletteData[instanceIdx].size()
                    );
                }
            }

            // アニメーション用の情報を更新
            if(drawData->drawOrder == (int)DrawOrder::AnimationModel){
                animationJointCount += jointSize * instanceCount;
            }

            // インスタンス数をインクリメント
            instanceCountAll += instanceCount;
        }
    }

    instanceCountAll = 0;
    int cameraIdx = 0;
    for(auto& [cameraName, camera] : pDxManager_->cameras_){

        // カメラ情報
        BaseCamera* pCamera = pDxManager_->GetCamera(cameraName);
        CameraForGPU cameraData;
        cameraData.position = pCamera->GetTranslation();
        std::memcpy(
            mapCameraData + cameraIdx,
            &cameraData,
            sizeof(CameraForGPU)
        );

        // カメラが切り替わるインスタンス数を記録
        cameraSwitchInstanceCount_[cameraName] = instanceCountAll;
        cameraOrder_[cameraName] = cameraIdx;

        for(auto& drawList : orderedDrawLists_){
            for(auto& drawData : *drawList){
                /*--------------------------------------*/
                //      トランスフォーム情報を書き込む
                /*--------------------------------------*/
                int instanceCount = (int)drawData->transforms[cameraName].size();
                std::memcpy(
                    mapTransformData + instanceCountAll,
                    drawData->transforms[cameraName].data(),
                    sizeof(TransformMatrix) * instanceCount
                );

                // インスタンス数をインクリメント
                instanceCountAll += instanceCount;
            }
        }
        cameraIdx++;
    }


    /*--------------------------------------*/
    //       マテリアル情報を書き込む
    /*--------------------------------------*/
    std::memcpy(
        mapMaterialData,
        materialArray.data(),
        sizeof(MaterialForGPU) * (int)materialArray.size()
    );
}



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                                                        //
//                                                 描画コールを行う関数                                                      //
//                                                                                                                        //
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void PolygonManager::SetRenderData(const std::string& cameraName, const DrawOrder& drawOrder){

    // モノがなければreturn
    if(modelDrawData_.size() == 0){ return; }
    if(objCounts_[(int)drawOrder] == 0){ return; }


    // インスタンスが切り替わる頂点間隔の決定(モデルは必要ないため0)
    int32_t instanceInterval = 0;
    if(drawOrder == DrawOrder::Model or drawOrder == DrawOrder::Particle or drawOrder == DrawOrder::AnimationModel){
        instanceInterval = 0;
    } else if(drawOrder == DrawOrder::Line or drawOrder == DrawOrder::Line2D){
        instanceInterval = 2;

    } else if(drawOrder == DrawOrder::Triangle or drawOrder == DrawOrder::Triangle2D){
        instanceInterval = 3;

    } else{
        instanceInterval = 4;
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////
    /*                                          共通の処理                                        */
    ///////////////////////////////////////////////////////////////////////////////////////////////

    // シザー矩形とviewport
    if((int)drawOrder < (int)DrawOrder::Offscreen){
        pDxManager_->commandList->RSSetViewports(1, &pDxManager_->viewport); // Viewport
        pDxManager_->commandList->RSSetScissorRects(1, &pDxManager_->scissorRect); // Scissor

    } else{// 解像度に影響されない描画用の設定
        pDxManager_->commandList->RSSetViewports(1, &pDxManager_->viewport_default); // Viewport
        pDxManager_->commandList->RSSetScissorRects(1, &pDxManager_->scissorRect_default); // Scissor
    }

    // 形状を設定
    if(drawOrder != DrawOrder::Line && drawOrder != DrawOrder::Line2D && drawOrder != DrawOrder::StaticLine2D){
        pDxManager_->commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    } else{
        pDxManager_->commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_LINELIST);
    }

    /*===========================================================================================*/

    /*                                   offset書き込み・描画処理                                   */

    /*===========================================================================================*/

    int meshCountAll = 0;
    int instanceCountAll = 0;
    int animationJointCount = 0;

    for(auto& pDrawList : orderedDrawLists_){

        // 描画リストが空ならcontinue
        auto& drawList = *pDrawList;
        if(drawList.size() == 0){ continue; }
        bool isBinded = false;


        ///////////////////////////////////////////////////////////////////////////////////////////////
        /*                                     Modelごとに見ていく                                      */
        ///////////////////////////////////////////////////////////////////////////////////////////////
        for(auto& drawData : drawList){

            // 各モデルの現在のブレンドモードのインスタンス数
            int instanceCount = (int)drawData->transforms[cameraName].size();
            int jointSize = (int)drawData->modelData->defaultSkeleton.joints.size();
            if(instanceCount == 0){ continue; }// インスタンスがない場合はスキップ
            if((int)drawData->materials.size() == 0){ continue; }// プリミティブ用の例外処理


            // 描画対象でない場合は書き込みを行わずインクリメントだけ行いスキップ
            if(drawData->drawOrder != (int8_t)drawOrder){
                meshCountAll += instanceCount * (int)drawData->modelData->meshes.size();
                instanceCountAll += instanceCount;
                continue;
            }

            ///////////////////////////////////////////////////////////////////////////////////////////////
            /*                                  パイプラインの切り替え                                      */
            ///////////////////////////////////////////////////////////////////////////////////////////////
            if(!isBinded){

                // キーがPSOなので、そこから取得し設定
                auto* rootSignature = drawList.front()->pso->rootSignature.get();
                auto* pipeline = drawList.front()->pso->pipeline.get();

                pDxManager_->commandList->SetGraphicsRootSignature(
                    rootSignature->rootSignature.Get()
                );
                pDxManager_->commandList->SetPipelineState(
                    pipeline->pipeline_.Get()
                );

                // Resourceのバインドを行う
                rootSignature->BindAll(pDxManager_->commandList.Get());

                isBinded = true;
            }


            // オフセットなどを計算し書き込み・描画を行う
            for(int meshIdx = 0; meshIdx < drawData->modelData->meshes.size(); meshIdx++){

                /*--------------------------------------*/
                //      オフセット情報を書き込む
                /*--------------------------------------*/

                // 書き込み
                for(auto& offset : drawData->offsetData[meshIdx]){
                    // 現在のモデルデータの先頭に到達するまでにどれだけinstanceがあったかのoffset
                    offset.instanceOffset = instanceCountAll;
                    // 現在のモデルデータの先頭に到達するまでにどれだけmeshがあったかのoffset
                    offset.meshOffset = meshCountAll;
                    // プリミティブの1インスタンスあたりの頂点数
                    offset.primitiveInterval = instanceInterval;
                    // SkinningAnimation用
                    offset.jointIndexOffset = animationJointCount;
                    offset.jointinterval = jointSize;
                }


                std::memcpy(
                    mapOffsetData + meshCountAll,
                    drawData->offsetData[meshIdx].data(),
                    sizeof(OffsetData) * drawData->totalDrawCount
                );


                //////////////////////////////////////////////////////////////////////
                // VSPipelineの場合は、VBVとIBVをセットして描画を行う
                //////////////////////////////////////////////////////////////////////
                if(drawData->pipelineType == PipelineType::VSPipeline){

                    /*///////////////////////////////////////////////////////////////////////////*/

                    /*                                VBVの設定                                   */

                    /*///////////////////////////////////////////////////////////////////////////*/


                    /*-------------------- 頂点ごとのデータ --------------------*/

                    D3D12_VERTEX_BUFFER_VIEW* vbv = &drawData->vbv_vertex;
                    int size = sizeof(VertexData);

                    // Resource上の開始位置設定
                    vbv->BufferLocation =
                        modelVertexResource_.Get()->GetGPUVirtualAddress() +
                        ((ModelDrawData::modelSwitchIdx_Vertex[drawData->hash] + drawData->meshSwitchIdx_Vertex[meshIdx]) * size);

                    // 総サイズ、刻み幅の設定
                    vbv->SizeInBytes = size * (int)drawData->modelData->meshes[meshIdx].vertices.size();
                    vbv->StrideInBytes = size;

                    // VBVのセット
                    pDxManager_->commandList->IASetVertexBuffers(0, 1, vbv);

                    /*-------------------- インスタンスごとのデータ --------------------*/

                    D3D12_VERTEX_BUFFER_VIEW* vbv2 = &drawData->vbv_instance;
                    size = sizeof(OffsetData);

                    // Resource上の開始位置設定
                    vbv2->BufferLocation =
                        offsetResource_.Get()->GetGPUVirtualAddress() + (meshCountAll * size);

                    // 総サイズ、刻み幅の設定
                    if(drawOrder == DrawOrder::Model or drawOrder == DrawOrder::AnimationModel or
                        drawOrder == DrawOrder::Particle or drawOrder == DrawOrder::SkyBox
                        ){
                        vbv2->SizeInBytes = size * drawData->totalDrawCount;
                    } else{
                        vbv2->SizeInBytes = size;
                    }
                    vbv2->StrideInBytes = size;

                    // VBVのセット
                    assert(meshCountAll < kMaxMeshCount_);// メッシュ数が上限を超えていないか確認
                    pDxManager_->commandList->IASetVertexBuffers(1, 1, vbv2);


                    /*-------------------- スキニング用のデータ --------------------*/

                    // アニメーションモデルの場合のみ
                    if(drawOrder == DrawOrder::AnimationModel){
                        D3D12_VERTEX_BUFFER_VIEW* vbv3 = &drawData->vbv_skinning;
                        size = sizeof(VertexInfluence);
                        int ofst = ((ModelDrawData::modelSwitchIdx_Vertex[drawData->hash] + drawData->meshSwitchIdx_Vertex[meshIdx]) * size);
                        // Resource上の開始位置設定
                        vbv3->BufferLocation =
                            vertexInfluenceResource_.Get()->GetGPUVirtualAddress() + ofst;

                        // 総サイズ、刻み幅の設定
                        vbv3->SizeInBytes = size * (UINT)drawData->modelData->meshes[meshIdx].vertices.size();
                        vbv3->StrideInBytes = size;

                        // VBVのセット
                        pDxManager_->commandList->IASetVertexBuffers(2, 1, vbv3);
                    }

                    /*///////////////////////////////////////////////////////////////////////////*/

                    /*                                IBVの設定                                   */

                    /*///////////////////////////////////////////////////////////////////////////*/


                    D3D12_INDEX_BUFFER_VIEW* ibv = &drawData->ibv;
                    size = sizeof(int32_t);

                    // Resource上の開始位置設定
                    ibv->BufferLocation =
                        modelIndexResource_.Get()->GetGPUVirtualAddress() +
                        ((ModelDrawData::modelSwitchIdx_Index[drawData->hash] + drawData->meshSwitchIdx_Index[meshIdx]) * size);

                    // 総サイズ、刻み幅の設定
                    ibv->SizeInBytes = size * (int)drawData->modelData->meshes[meshIdx].indices.size();
                    ibv->Format = DXGI_FORMAT_R32_UINT;

                    // IBVのセット
                    pDxManager_->commandList->IASetIndexBuffer(ibv);


                    /*/////////////////////////////////////////////////////////////////*/

                    /*      　                 メッシュごとに描画                         */

                    /*/////////////////////////////////////////////////////////////////*/

                    // 描画
                    if(drawOrder == DrawOrder::Model or drawOrder == DrawOrder::AnimationModel
                        or drawOrder == DrawOrder::Particle or drawOrder == DrawOrder::SkyBox
                        ){

                        pDxManager_->commandList->DrawIndexedInstanced(
                            (int)drawData->modelData->meshes[meshIdx].indices.size(),
                            drawData->totalDrawCount,
                            0,
                            0,
                            0
                        );

                    } else{

                        // プリミティブは一斉描画
                        pDxManager_->commandList->DrawIndexedInstanced(
                            (int)drawData->indexCount,
                            1,
                            0,
                            0,
                            0
                        );
                    }


                } else if(drawData->pipelineType == PipelineType::MSPipeline){

                    // meshlet数を求める
                    int meshletCount = drawData->modelData->meshes[meshIdx].meshletCount;

                    // 描画
                    pDxManager_->commandList->DispatchMesh(
                        meshletCount,// ここはMeshletの数
                        drawData->totalDrawCount,// ここはインスタンス数
                        1
                    );

                } else{
                    assert(false);// CSPipelineで描画すんな
                }


                // 描画総メッシュ数のインクリメント
                meshCountAll += instanceCount;

            }

            // アニメーション用のジョイント数のインクリメント
            if(drawOrder == DrawOrder::AnimationModel){
                animationJointCount += jointSize * instanceCount;
            }


            // 描画総インスタンス数のインクリメント
            instanceCountAll += instanceCount;
        }
    }
}

/*---------------- フレームの終わりに積み上げられた情報をまとめてコマンドに積んで描画する関数 -------------------*/

void PolygonManager::DrawToOffscreen(const std::string& cameraName){

    if(!isWrited_){

        // オフスクリーンの描画依頼をここで出しておく
        if((int)PostEffect::GetInstance()->postProcesses_.size() != 0){
            AddOffscreenResult(ViewManager::GetTextureHandle("postEffectResult"), BlendMode::NONE);

        } else{
            AddOffscreenResult(
                ViewManager::GetTextureHandle(pDxManager_->offScreenNames[pDxManager_->mainCameraName_]),
                BlendMode::NONE
            );
        }

        // skyBoxの描画フラグが立っていれば描画
        if(skyBoxAdded_){
            AddSkyBox();
        }

        // Resourceに情報を書き込む
        WriteRenderData();
        isWrited_ = true;
    }

    // 背景描画
    SetRenderData(cameraName, DrawOrder::BackSprite);

    // 3D
    SetRenderData(cameraName, DrawOrder::SkyBox);
    SetRenderData(cameraName, DrawOrder::Line);
    SetRenderData(cameraName, DrawOrder::Model);
    SetRenderData(cameraName, DrawOrder::AnimationModel);
    SetRenderData(cameraName, DrawOrder::Triangle);
    SetRenderData(cameraName, DrawOrder::Quad);
    SetRenderData(cameraName, DrawOrder::Text);
    SetRenderData(cameraName, DrawOrder::Particle);

    // 2D
    SetRenderData(cameraName, DrawOrder::Line2D);
    SetRenderData(cameraName, DrawOrder::Triangle2D);
    SetRenderData(cameraName, DrawOrder::Quad2D);
    SetRenderData(cameraName, DrawOrder::Sprite);
    SetRenderData(cameraName, DrawOrder::Text2D);
}

void PolygonManager::DrawToBackBuffer(){

    // オフスクリーンの描画結果を貼り付ける
    std::string cameraName = pDxManager_->mainCameraName_;
    SetRenderData(cameraName, DrawOrder::Offscreen);

    // 解像度の変更に影響を受けない2D描画
    SetRenderData(cameraName, DrawOrder::StaticLine2D);
    SetRenderData(cameraName, DrawOrder::StaticTriangle2D);
    SetRenderData(cameraName, DrawOrder::StaticQuad2D);
    SetRenderData(cameraName, DrawOrder::StaticText2D);
    SetRenderData(cameraName, DrawOrder::StaticSprite);
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ライトの追加
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void PolygonManager::AddLight(BaseLight* light){
    switch(light->GetLightType()){
    case BASE_LIGHT:
        break;

    case DIRECTIONAL_LIGHT:
        directionalLights_.emplace_back(*static_cast<DirectionalLight*>(light));
        break;

    case POINT_LIGHT:
        pointLights_.emplace_back(*static_cast<PointLight*>(light));
        break;

    case SPOT_LIGHT:
        spotLights_.emplace_back(*static_cast<SpotLight*>(light));
        break;

    default:
        break;
    }
}
