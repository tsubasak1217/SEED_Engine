// local
#include <PolygonManager.h>
#include <DxManager.h>
#include <SEED.h>
#include <MyMath.h>
#include <MyFunc.h>
#include <Environment.h>
#include "ModelManager.h"
#include "TextureManager.h"
#include "PSO/PSOManager.h"

// external
#include <assert.h>

/*------------------------ static, グローバル変数の初期化 ------------------------------*/

uint32_t PolygonManager::triangleIndexCount_ = 0;
uint32_t PolygonManager::quadIndexCount_ = 0;
uint32_t PolygonManager::modelIndexCount_ = 0;
uint32_t PolygonManager::spriteCount_ = 0;
uint32_t PolygonManager::lineCount_ = 0;

std::unordered_map<std::string, int32_t> ModelDrawData::modelSwitchIdx_Vertex;
std::unordered_map<std::string, int32_t> ModelDrawData::modelSwitchIdx_Index;

D3D12_VERTEX_BUFFER_VIEW ModelDrawData::vbv_vertex;
D3D12_VERTEX_BUFFER_VIEW ModelDrawData::vbv_instance;
D3D12_VERTEX_BUFFER_VIEW ModelDrawData::vbv_skinning;
D3D12_INDEX_BUFFER_VIEW ModelDrawData::ibv;


std::string blendName[(int)BlendMode::kBlendModeCount] = {
    "_Blend::NONE",
    "_Blend::MUL",
    "_Blend::SUB",
    "_Blend::NORMAL",
    "_Blend::ADD",
    "_Blend::SCREEN"
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
        CreateBufferResource(pDxManager_->device.Get(), sizeof(WellForGPU) * 1024);
    paletteResource_->SetName(L"paletteResource");

    // Camera
    cameraResource_ =
        CreateBufferResource(pDxManager_->device.Get(), sizeof(CameraForGPU));
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
    D3D12_SHADER_RESOURCE_VIEW_DESC instancingSrvDesc[6];
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
    instancingSrvDesc[2].Buffer.NumElements = 1024;

    instancingSrvDesc[3].Format = DXGI_FORMAT_UNKNOWN;
    instancingSrvDesc[3].Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
    instancingSrvDesc[3].ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
    instancingSrvDesc[3].Buffer.FirstElement = 0;
    instancingSrvDesc[3].Buffer.Flags = D3D12_BUFFER_SRV_FLAG_NONE;
    instancingSrvDesc[3].Buffer.NumElements = 0xff;

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

    /*--------- DirectionalLight用 ----------*/
    instancingSrvDesc[3].Buffer.StructureByteStride = sizeof(DirectionalLight);
    ViewManager::CreateView(
        VIEW_TYPE::SRV, directionalLightResource_.Get(),
        &instancingSrvDesc[3], "directionalLight"
    );

    /*------------ PointLight用 --------------*/
    instancingSrvDesc[4].Buffer.StructureByteStride = sizeof(PointLight);
    ViewManager::CreateView(
        VIEW_TYPE::SRV, pointLightResource_.Get(),
        &instancingSrvDesc[4], "pointLight"
    );

    /*------------ SpotLight用 --------------*/
    instancingSrvDesc[5].Buffer.StructureByteStride = sizeof(SpotLight);
    ViewManager::CreateView(
        VIEW_TYPE::SRV, spotLightResource_.Get(),
        &instancingSrvDesc[5], "spotLight"
    );

}

void PolygonManager::Finalize(){}

void PolygonManager::Reset(){

    // モデルの情報をリセット
    for(auto& modelDrawData : modelDrawData_){
        modelDrawData.second->indexCount = 0;
        modelDrawData.second->modelSwitchIdx_Index.clear();
        modelDrawData.second->modelSwitchIdx_Vertex.clear();
        std::memset(modelDrawData.second->totalDrawCount, 0, sizeof(modelDrawData.second->totalDrawCount));
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

}


// プリミティブ描画情報の初期化
void PolygonManager::InitializePrimitive(){


    for(int blendIdx = 0; blendIdx < (int)BlendMode::kBlendModeCount; blendIdx++){
        for(int cullIdx = 0; cullIdx < 3; ++cullIdx){
            // 通常の描画
            modelDrawData_["ENGINE_DRAW_TRIANGLE" + blendName[blendIdx] + cullName[cullIdx]] = std::make_unique<ModelDrawData>();
            modelDrawData_["ENGINE_DRAW_QUAD" + blendName[blendIdx] + cullName[cullIdx]] = std::make_unique<ModelDrawData>();
            modelDrawData_["ENGINE_DRAW_LINE" + blendName[blendIdx] + cullName[cullIdx]] = std::make_unique<ModelDrawData>();
            modelDrawData_["ENGINE_DRAW_TRIANGLE2D" + blendName[blendIdx] + cullName[cullIdx]] = std::make_unique<ModelDrawData>();
            modelDrawData_["ENGINE_DRAW_QUAD2D" + blendName[blendIdx] + cullName[cullIdx]] = std::make_unique<ModelDrawData>();
            modelDrawData_["ENGINE_DRAW_SPRITE" + blendName[blendIdx] + cullName[cullIdx]] = std::make_unique<ModelDrawData>();
            modelDrawData_["ENGINE_DRAW_LINE2D" + blendName[blendIdx] + cullName[cullIdx]] = std::make_unique<ModelDrawData>();

            // 解像度に影響しない描画
            modelDrawData_["ENGINE_DRAW_STATIC_TRIANGLE2D" + blendName[blendIdx] + cullName[cullIdx]] = std::make_unique<ModelDrawData>();
            modelDrawData_["ENGINE_DRAW_STATIC_QUAD2D" + blendName[blendIdx] + cullName[cullIdx]] = std::make_unique<ModelDrawData>();
            modelDrawData_["ENGINE_DRAW_STATIC_SPRITE" + blendName[blendIdx] + cullName[cullIdx]] = std::make_unique<ModelDrawData>();
            modelDrawData_["ENGINE_DRAW_STATIC_LINE2D" + blendName[blendIdx] + cullName[cullIdx]] = std::make_unique<ModelDrawData>();
            modelDrawData_["ENGINE_DRAW_OFFSCREEN" + blendName[blendIdx] + cullName[cullIdx]] = std::make_unique<ModelDrawData>();
        }
    }


    // プリミティブの頂点などの情報を初期化
    for(int blendIdx = 0; blendIdx < (int)BlendMode::kBlendModeCount; blendIdx++){
        for(int cullIdx = 0; cullIdx < 3; ++cullIdx){
            for(auto& primitiveData : primitiveData_){
                primitiveData[blendIdx][cullIdx].meshes.clear();
                primitiveData[blendIdx][cullIdx].materials.clear();
            }
        }
    }

    // プリミティブの描画情報を初期化
    for(int32_t blendIdx = 0; blendIdx < (int)BlendMode::kBlendModeCount; blendIdx++){
        for(int cullIdx = 0; cullIdx < 3; ++cullIdx){
            // 参照先を設定
            modelDrawData_["ENGINE_DRAW_TRIANGLE" + blendName[blendIdx] + cullName[cullIdx]]->modelData = &primitiveData_[PRIMITIVE_TRIANGLE][blendIdx][cullIdx];
            modelDrawData_["ENGINE_DRAW_TRIANGLE2D" + blendName[blendIdx] + cullName[cullIdx]]->modelData = &primitiveData_[PRIMITIVE_TRIANGLE2D][blendIdx][cullIdx];
            modelDrawData_["ENGINE_DRAW_STATIC_TRIANGLE2D" + blendName[blendIdx] + cullName[cullIdx]]->modelData = &primitiveData_[PRIMITIVE_STATIC_TRIANGLE2D][blendIdx][cullIdx];
            modelDrawData_["ENGINE_DRAW_QUAD" + blendName[blendIdx] + cullName[cullIdx]]->modelData = &primitiveData_[PRIMITIVE_QUAD][blendIdx][cullIdx];
            modelDrawData_["ENGINE_DRAW_QUAD2D" + blendName[blendIdx] + cullName[cullIdx]]->modelData = &primitiveData_[PRIMITIVE_QUAD2D][blendIdx][cullIdx];
            modelDrawData_["ENGINE_DRAW_STATIC_QUAD2D" + blendName[blendIdx] + cullName[cullIdx]]->modelData = &primitiveData_[PRIMITIVE_STATIC_QUAD2D][blendIdx][cullIdx];
            modelDrawData_["ENGINE_DRAW_LINE" + blendName[blendIdx] + cullName[cullIdx]]->modelData = &primitiveData_[PRIMITIVE_LINE][blendIdx][cullIdx];
            modelDrawData_["ENGINE_DRAW_LINE2D" + blendName[blendIdx] + cullName[cullIdx]]->modelData = &primitiveData_[PRIMITIVE_LINE2D][blendIdx][cullIdx];
            modelDrawData_["ENGINE_DRAW_STATIC_LINE2D" + blendName[blendIdx] + cullName[cullIdx]]->modelData = &primitiveData_[PRIMITIVE_STATIC_LINE2D][blendIdx][cullIdx];
            modelDrawData_["ENGINE_DRAW_SPRITE" + blendName[blendIdx] + cullName[cullIdx]]->modelData = &primitiveData_[PRIMITIVE_SPRITE][blendIdx][cullIdx];
            modelDrawData_["ENGINE_DRAW_STATIC_SPRITE" + blendName[blendIdx] + cullName[cullIdx]]->modelData = &primitiveData_[PRIMITIVE_STATIC_SPRITE][blendIdx][cullIdx];
            modelDrawData_["ENGINE_DRAW_OFFSCREEN" + blendName[blendIdx] + cullName[cullIdx]]->modelData = &primitiveData_[PRIMITIVE_OFFSCREEN][blendIdx][cullIdx];

            // 描画順を設定
            modelDrawData_["ENGINE_DRAW_TRIANGLE" + blendName[blendIdx] + cullName[cullIdx]]->drawOrder = (int8_t)DrawOrder::Triangle;
            modelDrawData_["ENGINE_DRAW_TRIANGLE2D" + blendName[blendIdx] + cullName[cullIdx]]->drawOrder = (int8_t)DrawOrder::Triangle2D;
            modelDrawData_["ENGINE_DRAW_STATIC_TRIANGLE2D" + blendName[blendIdx] + cullName[cullIdx]]->drawOrder = (int8_t)DrawOrder::StaticTriangle2D;
            modelDrawData_["ENGINE_DRAW_QUAD" + blendName[blendIdx] + cullName[cullIdx]]->drawOrder = (int8_t)DrawOrder::Quad;
            modelDrawData_["ENGINE_DRAW_QUAD2D" + blendName[blendIdx] + cullName[cullIdx]]->drawOrder = (int8_t)DrawOrder::Quad2D;
            modelDrawData_["ENGINE_DRAW_STATIC_QUAD2D" + blendName[blendIdx] + cullName[cullIdx]]->drawOrder = (int8_t)DrawOrder::StaticQuad2D;
            modelDrawData_["ENGINE_DRAW_LINE" + blendName[blendIdx] + cullName[cullIdx]]->drawOrder = (int8_t)DrawOrder::Line;
            modelDrawData_["ENGINE_DRAW_LINE2D" + blendName[blendIdx] + cullName[cullIdx]]->drawOrder = (int8_t)DrawOrder::Line2D;
            modelDrawData_["ENGINE_DRAW_STATIC_LINE2D" + blendName[blendIdx] + cullName[cullIdx]]->drawOrder = (int8_t)DrawOrder::StaticLine2D;
            modelDrawData_["ENGINE_DRAW_SPRITE" + blendName[blendIdx] + cullName[cullIdx]]->drawOrder = (int8_t)DrawOrder::Sprite;
            modelDrawData_["ENGINE_DRAW_STATIC_SPRITE" + blendName[blendIdx] + cullName[cullIdx]]->drawOrder = (int8_t)DrawOrder::StaticSprite;
            modelDrawData_["ENGINE_DRAW_OFFSCREEN" + blendName[blendIdx] + cullName[cullIdx]]->drawOrder = (int8_t)DrawOrder::Offscreen;
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
    DrawLocation drawLocation, uint32_t layer
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
            float zFar = pDxManager_->GetCamera()->GetZFar() - layerZ;
            transformed[0].z = zFar;
            transformed[1].z = zFar;
            transformed[2].z = zFar;
        } else{
            float zNear = pDxManager_->GetCamera()->GetZNear() - layerZ;
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

    auto* modelData = view3D ?
        &primitiveData_[PRIMITIVE_TRIANGLE][(int)blendMode][(int)cullMode - 1] :
        &primitiveData_[PRIMITIVE_TRIANGLE2D][(int)blendMode][(int)cullMode - 1];

    if(isStaticDraw){
        modelData = &primitiveData_[PRIMITIVE_STATIC_TRIANGLE2D][(int)blendMode][(int)cullMode - 1];
    }

    static std::string drawDataName[2];
    drawDataName[0].clear();
    drawDataName[0].reserve(128);
    drawDataName[0] += "ENGINE_DRAW_TRIANGLE";
    drawDataName[0] += blendName[(int)blendMode];
    drawDataName[0] += cullName[0];
    drawDataName[1].clear();
    drawDataName[1].reserve(128);
    drawDataName[1] += isStaticDraw ? "ENGINE_DRAW_STATIC_TRIANGLE2D" : "ENGINE_DRAW_TRIANGLE2D";
    drawDataName[1] += blendName[(int)blendMode];
    drawDataName[1] += cullName[(int)cullMode - 1];

    auto* drawData3D = modelDrawData_[drawDataName[0]].get();
    auto* drawData2D = modelDrawData_[drawDataName[1]].get();

    // indexCount
    int drawCount = (view3D ? drawData3D->totalDrawCount[(int)blendMode][(int)cullMode - 1] : drawData2D->totalDrawCount[(int)blendMode][(int)cullMode - 1]);
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
    // 合わせる
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
    if(view3D){
        drawData3D->materials[(int)blendMode][(int)cullMode - 1].resize(1);
        auto& material = drawData3D->materials[(int)blendMode][(int)cullMode - 1].back();
        if(material.size() <= drawCount){ material.resize(drawCount + 1); }
        material[drawCount].color_ = color;
        material[drawCount].lightingType_ = lightingType;
        material[drawCount].uvTransform_ = uvTransform;
        material[drawCount].GH_ = GH;

    } else{
        drawData2D->materials[(int)blendMode][(int)cullMode - 1].resize(1);
        auto& material = drawData2D->materials[(int)blendMode][(int)cullMode - 1].back();
        if(material.size() <= drawCount){ material.resize(drawCount + 1); }
        material[drawCount].color_ = color;
        material[drawCount].lightingType_ = lightingType;
        material[drawCount].uvTransform_ = uvTransform;
        material[drawCount].GH_ = GH;
    }


    // transform
    if(view3D){
        auto& transform = drawData3D->transforms[(int)blendMode][(int)cullMode - 1];
        if(transform.size() <= drawCount){ transform.resize(drawCount + 1); }
        transform[drawCount].world_ = worldMat;
        transform[drawCount].WVP_ = pDxManager_->GetCamera()->GetViewProjectionMat();
        transform[drawCount].worldInverseTranspose_ = Transpose(InverseMatrix(worldMat));
    } else{
        auto& transform = drawData2D->transforms[(int)blendMode][(int)cullMode - 1];
        if(transform.size() <= drawCount){ transform.resize(drawCount + 1); }
        transform[drawCount].world_ = worldMat;
        transform[drawCount].WVP_ = pDxManager_->GetCamera()->GetViewProjectionMat2D();
        transform[drawCount].worldInverseTranspose_ = Transpose(InverseMatrix(worldMat));
    }


    // offsetResourceの数を更新
    auto& offsetData = view3D ? drawData3D->offsetData[(int)blendMode][(int)cullMode - 1] : drawData2D->offsetData[(int)blendMode][(int)cullMode - 1];
    if(offsetData.size() == 0){
        offsetData.resize(1);
    }

    if(offsetData.back().size() <= drawCount){
        offsetData.back().resize(drawCount + 1);
    }


    // カウントを更新
    if(isStaticDraw){
        objCounts_[(int)DrawOrder::StaticTriangle2D]++;
    } else{
        if(view3D) {
            objCounts_[(int)DrawOrder::Triangle]++;
        } else{
            objCounts_[(int)DrawOrder::Triangle2D]++;
        }
    }

    objCountCull_[(int)cullMode - 1]++;
    objCountBlend_[(int)blendMode]++;
    triangleIndexCount_++;
    view3D ? drawData3D->indexCount += 3 : drawData2D->indexCount += 3;
    view3D ? drawData3D->totalDrawCount[(int)blendMode][(int)cullMode - 1]++ : drawData2D->totalDrawCount[(int)blendMode][(int)cullMode - 1]++;
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
    const Matrix4x4& worldMat, const Vector4& color,
    int32_t lightingType, const Matrix4x4& uvTransform, bool view3D, uint32_t GH,
    BlendMode blendMode, D3D12_CULL_MODE cullMode, bool isStaticDraw,
    DrawLocation drawLocation, uint32_t layer
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
            float zFar = pDxManager_->GetCamera()->GetZFar() - layerZ;
            transformed[0].z = zFar;
            transformed[1].z = zFar;
            transformed[2].z = zFar;
            transformed[3].z = zFar;
        } else{
            float zNear = pDxManager_->GetCamera()->GetZNear() - layerZ;
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

    auto* modelData = view3D ?
        &primitiveData_[PRIMITIVE_QUAD][(int)blendMode][(int)cullMode - 1] :
        &primitiveData_[PRIMITIVE_QUAD2D][(int)blendMode][(int)cullMode - 1];

    if(isStaticDraw){
        modelData = &primitiveData_[PRIMITIVE_STATIC_QUAD2D][(int)blendMode][(int)cullMode - 1];
    }

    static std::string drawDataName[2];
    drawDataName[0].clear();
    drawDataName[0].reserve(128);
    drawDataName[0] += "ENGINE_DRAW_QUAD";
    drawDataName[0] += blendName[(int)blendMode];
    drawDataName[0] += cullName[0];
    drawDataName[1].clear();
    drawDataName[1].reserve(128);
    drawDataName[1] += isStaticDraw ? "ENGINE_DRAW_STATIC_QUAD2D" : "ENGINE_DRAW_QUAD2D";
    drawDataName[1] += blendName[(int)blendMode];
    drawDataName[1] += cullName[(int)cullMode - 1];

    auto* drawData3D = modelDrawData_[drawDataName[0]].get();
    auto* drawData2D = modelDrawData_[drawDataName[1]].get();

    // Count
    int drawCount = (view3D ? drawData3D->totalDrawCount[(int)blendMode][(int)cullMode - 1] : drawData2D->totalDrawCount[(int)blendMode][(int)cullMode - 1]);
    int vertexCount = drawCount * 4;
    int indexCount = drawCount * 6;
    // vertexResource
    modelData->meshes.resize(1);
    auto& mesh = modelData->meshes[0];
    if(mesh.vertices.size() < vertexCount + 4){ mesh.vertices.resize(vertexCount + 4); }
    mesh.vertices[vertexCount] = VertexData(transformed[0].ToVec4(), Vector2(0.0f, 0.0f), normalVec);
    mesh.vertices[vertexCount + 1] = VertexData(transformed[1].ToVec4(), Vector2(1.0f, 0.0f), normalVec);
    mesh.vertices[vertexCount + 2] = VertexData(transformed[2].ToVec4(), Vector2(0.0f, 1.0f), normalVec);
    mesh.vertices[vertexCount + 3] = VertexData(transformed[3].ToVec4(), Vector2(1.0f, 1.0f), normalVec);

    //indexResource
    if(mesh.indices.size() <= indexCount){ mesh.indices.resize(indexCount + 6); }
    mesh.indices[indexCount] = indexCount;
    mesh.indices[indexCount + 1] = indexCount + 1;
    mesh.indices[indexCount + 2] = indexCount + 3;
    mesh.indices[indexCount + 3] = indexCount + 0;
    mesh.indices[indexCount + 4] = indexCount + 3;
    mesh.indices[indexCount + 5] = indexCount + 2;

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
    if(view3D){
        drawData3D->materials[(int)blendMode][(int)cullMode - 1].resize(1);
        auto& material = drawData3D->materials[(int)blendMode][(int)cullMode - 1].back();
        if(material.size() <= drawCount){ material.resize(drawCount + 1); }
        material[drawCount].color_ = color;
        material[drawCount].lightingType_ = lightingType;
        material[drawCount].uvTransform_ = uvTransform;
        material[drawCount].GH_ = GH;

    } else{
        drawData2D->materials[(int)blendMode][(int)cullMode - 1].resize(1);
        auto& material = drawData2D->materials[(int)blendMode][(int)cullMode - 1].back();
        if(material.size() <= drawCount){ material.resize(drawCount + 1); }
        material[drawCount].color_ = color;
        material[drawCount].lightingType_ = lightingType;
        material[drawCount].uvTransform_ = uvTransform;
        material[drawCount].GH_ = GH;
    }


    // transform
    if(view3D){
        auto& transform = drawData3D->transforms[(int)blendMode][(int)cullMode - 1];
        if(transform.size() <= drawCount){ transform.resize(drawCount + 1); }
        transform[drawCount].world_ = worldMat;
        transform[drawCount].WVP_ = pDxManager_->GetCamera()->GetViewProjectionMat();
        transform[drawCount].worldInverseTranspose_ = Transpose(InverseMatrix(worldMat));
    } else{
        auto& transform = drawData2D->transforms[(int)blendMode][(int)cullMode - 1];
        transform[drawCount].world_ = IdentityMat4();
        transform[drawCount].WVP_ = pDxManager_->GetCamera()->GetProjectionMat2D();
        transform[drawCount].worldInverseTranspose_ = IdentityMat4();
    }


    // offsetResourceの数を更新
    auto& offsetData = view3D ? drawData3D->offsetData[(int)blendMode][(int)cullMode - 1] : drawData2D->offsetData[(int)blendMode][(int)cullMode - 1];
    if(offsetData.size() == 0){
        offsetData.resize(1);
    }

    if(offsetData.back().size() <= drawCount){
        offsetData.back().resize(drawCount + 1);
    }

    // カウントを更新
    if(isStaticDraw){
        objCounts_[(int)DrawOrder::StaticQuad2D]++;
    } else{
        if(view3D) {
            objCounts_[(int)DrawOrder::Quad]++;
        } else{
            objCounts_[(int)DrawOrder::Quad2D]++;
        }
    }

    objCountCull_[(int)cullMode - 1]++;
    objCountBlend_[(int)blendMode]++;
    quadIndexCount_++;
    view3D ? drawData3D->indexCount += 6 : drawData2D->indexCount += 6;
    view3D ? drawData3D->totalDrawCount[(int)blendMode][(int)cullMode - 1]++ : drawData2D->totalDrawCount[(int)blendMode][(int)cullMode - 1]++;
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
    bool isStaticDraw, DrawLocation drawLocation, uint32_t layer, bool isSystemDraw
){
    assert(spriteCount_ < kMaxSpriteCount);
    blendMode;


    ///////////////////////////////////////////////////////////////////
    /*---------------------- 頂点・法線などを求める ---------------------*/
    ///////////////////////////////////////////////////////////////////

    // 遠近
    float layerZ = 0.001f * layer;
    float zNear = pDxManager_->GetCamera()->GetZNear() - layerZ;
    float zfar = pDxManager_->GetCamera()->GetZFar() - layerZ;

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

    static std::string drawDataName;
    drawDataName.clear();
    drawDataName.reserve(128);
    drawDataName += isStaticDraw ? "ENGINE_DRAW_STATIC_SPRITE" : "ENGINE_DRAW_SPRITE";
    drawDataName += blendName[(int)blendMode];
    drawDataName += cullName[(int)cullMode - 1];
    auto* drawData = modelDrawData_[drawDataName].get();

    // 背面描画の場合はzFarに設定
    if(drawLocation == DrawLocation::Back && !isStaticDraw){
        v[0].z = zfar;
        v[1].z = zfar;
        v[2].z = zfar;
        v[3].z = zfar;
    }

    //count
    int drawCount = drawData->totalDrawCount[(int)blendMode][(int)cullMode - 1];
    int vertexCount = drawCount * 4;
    int indexCount = drawCount * 6;
    // vertexResource
    modelData->meshes.resize(1);
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
    drawData->materials[(int)blendMode][(int)cullMode - 1].resize(1);
    auto& material = drawData->materials[(int)blendMode][(int)cullMode - 1].back();
    if(material.size() <= drawCount){ material.resize(drawCount + 1); }
    material[drawCount].color_ = color;
    material[drawCount].lightingType_ = LIGHTINGTYPE_NONE;
    material[drawCount].uvTransform_ = uvTransform;
    material[drawCount].GH_ = GH;

    // transform
    auto& transform = drawData->transforms[(int)blendMode][(int)cullMode - 1];
    if(transform.size() <= drawCount){ transform.resize(drawCount + 1); }
    transform[drawCount].world_ = IdentityMat4();
    transform[drawCount].WVP_ = pDxManager_->GetCamera()->GetProjectionMat2D();
    transform[drawCount].worldInverseTranspose_ = IdentityMat4();

    // offsetResourceの数を更新
    auto& offsetData = drawData->offsetData[(int)blendMode][(int)cullMode - 1];
    if(offsetData.size() == 0){
        offsetData.resize(1);
    }

    if(offsetData.back().size() <= drawCount){
        offsetData.back().resize(drawCount + 1);
    }


    // カウントを更新
    if(isStaticDraw){
        objCounts_[(int)DrawOrder::StaticSprite]++;
    } else{
        objCounts_[(int)DrawOrder::Sprite]++;
    }


    objCountCull_[(int)cullMode - 1]++;
    objCountBlend_[(int)blendMode]++;
    spriteCount_++;
    drawData->indexCount += 6;
    drawData->totalDrawCount[(int)blendMode][(int)cullMode - 1]++;

}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                                                        //
//                                                     モデルの追加                                                         //
//                                                                                                                        //
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void PolygonManager::AddModel(Model* model){

    //////////////////////////////////////////////////////////////////////////
    // モデルの名前の決定
    //////////////////////////////////////////////////////////////////////////
    static std::string modelName;
    modelName.clear();
    modelName.reserve(64);
    modelName = model->modelName_;

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
    if(modelDrawData_.find(modelName) == modelDrawData_.end()){
        modelDrawData_[modelName] = std::make_unique<ModelDrawData>();
        // vertexResource
        modelDrawData_[modelName]->modelData = ModelManager::GetModelData(model->modelName_);
        // drawOrder
        if(model->isAnimation_){
            modelDrawData_[modelName]->drawOrder = (int)DrawOrder::AnimationModel;
        } else if(model->isParticle_){
            modelDrawData_[modelName]->drawOrder = (int)DrawOrder::Particle;
        }
    }

    // サイズ確保
    auto& item = modelDrawData_[modelName];
    int meshSize = (int)modelDrawData_[modelName]->modelData->meshes.size();

    // count
    int drawCount = modelDrawData_[modelName]->totalDrawCount[(int)model->blendMode_][(int)model->cullMode - 1];

    /////////////////////////////////////////////////////////////////////////
    //                          materialResourceの設定
    /////////////////////////////////////////////////////////////////////////

    modelDrawData_[modelName]->materials[(int)model->blendMode_][(int)model->cullMode - 1].resize(meshSize);
    for(int meshIdx = 0; meshIdx < meshSize; meshIdx++){

        auto& material = item->materials[(int)model->blendMode_][(int)model->cullMode - 1][meshIdx];
        if(material.size() <= drawCount){ material.resize(drawCount + 1); }

        material[drawCount].color_ = model->color_;
        material[drawCount].shininess_ = model->shininess_;
        material[drawCount].lightingType_ = model->lightingType_;
        material[drawCount].uvTransform_ = model->GetUVTransform(meshIdx);
        material[drawCount].GH_ = model->textureGH_[meshIdx];
    }

    //////////////////////////////////////////////////////////////////////////
    //                          transformResourceの設定
    //////////////////////////////////////////////////////////////////////////

    Matrix4x4 wvp = Multiply(
        model->GetWorldMat(),
        pDxManager_->GetCamera()->GetViewProjectionMat()
    );

    auto& transform = item->transforms[(int)model->blendMode_][(int)model->cullMode - 1];
    if(transform.size() <= drawCount){ transform.resize(drawCount + 1); }
    transform[drawCount].world_ = model->GetWorldMat();
    transform[drawCount].WVP_ = wvp;
    transform[drawCount].worldInverseTranspose_ = Transpose(InverseMatrix(model->GetWorldMat()));

    //////////////////////////////////////////////////////////////////////////
    //                              offset情報の設定
    //////////////////////////////////////////////////////////////////////////

    modelDrawData_[modelName]->offsetData[(int)model->blendMode_][(int)model->cullMode - 1].resize(meshSize);
    for(int meshIdx = 0; meshIdx < meshSize; meshIdx++){
        auto& offsetData = item->offsetData[(int)model->blendMode_][(int)model->cullMode - 1][meshIdx];
        offsetData.resize(drawCount + 1);
    }

    //////////////////////////////////////////////////////////////////////////
    //                              palette情報の設定
    //////////////////////////////////////////////////////////////////////////
    if(model->isAnimation_){
        auto& palette = modelDrawData_[modelName]->paletteData[(int)model->blendMode_][(int)model->cullMode - 1];
        if(palette.size() <= drawCount){ palette.resize(drawCount + 1); }
        palette[drawCount] = model->palette_;
    }

    // 要素数を更新
    if(model->isAnimation_){
        objCounts_[(int)DrawOrder::AnimationModel]++;
    } else{
        model->isParticle_ ? objCounts_[(int)DrawOrder::Particle]++ : objCounts_[(int)DrawOrder::Model]++;
    }

    objCountCull_[(int)model->cullMode - 1]++;
    objCountBlend_[(int)model->blendMode_]++;
    modelDrawData_[modelName]->totalDrawCount[(int)model->blendMode_][(int)model->cullMode -1]++;
    modelIndexCount_++;

    // モデルのスケルトンを描画
    if(model->isSkeletonVisible_){

        const auto& modeldata = ModelManager::GetModelData(model->modelName_);
        const ModelSkeleton& skeleton = ModelManager::AnimatedSkeleton(
            modeldata->animations[model->animationName_],
            modeldata->defaultSkeleton,
            model->animationTime_
        );

        for(int i = 0; i < skeleton.joints.size(); i++){

            if(skeleton.joints[i].parent){
                Vector3 point[2];
                point[0] = Vector3(0.0f, 0.0f, 0.0f) * skeleton.joints[i].skeletonSpaceMatrix;
                point[1] = Vector3(0.0f, 0.0f, 0.0f) *
                    skeleton.joints[skeleton.joints[i].parent.value()].skeletonSpaceMatrix;

                SEED::DrawLine(point[0], point[1], { 0.0f,0.0f,1.0f,1.0f });
            }
        }
    }
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
    DrawLocation drawLocation, uint32_t layer
){

    assert(lineCount_ < kMaxLineCount_);
    Vector3 normalVec = { 0.0f,0.0f,-1.0f };
    Vector4 v[2]{ v1,v2 };
    float layerZ = 0.001f * layer;

    // レイヤー、描画場所に応じたZ値に設定
    if(drawLocation != DrawLocation::Not2D){
        if(drawLocation == DrawLocation::Back){
            float zFar = pDxManager_->GetCamera()->GetZFar() - layerZ;
            v[0].z = zFar;
            v[1].z = zFar;
        } else{
            float zNear = pDxManager_->GetCamera()->GetZNear() - layerZ;
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

    static std::string drawDataName[2];
    drawDataName[0].clear();
    drawDataName[0].reserve(128);
    drawDataName[0] += "ENGINE_DRAW_LINE";
    drawDataName[0] += blendName[(int)blendMode];
    drawDataName[0] += cullName[0];
    drawDataName[1].clear();
    drawDataName[1].reserve(128);
    drawDataName[1] += isStaticDraw ? "ENGINE_DRAW_STATIC_LINE2D" : "ENGINE_DRAW_LINE2D";
    drawDataName[1] += blendName[(int)blendMode];
    drawDataName[1] += cullName[0];

    auto* drawData3D = modelDrawData_[drawDataName[0]].get();
    auto* drawData2D = modelDrawData_[drawDataName[1]].get();

    // Count
    int drawCount = (view3D ? drawData3D->totalDrawCount[(int)blendMode][0] : drawData2D->totalDrawCount[(int)blendMode][0]);
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

    // material
    auto& material = view3D ? drawData3D->materials[(int)blendMode][0] : drawData2D->materials[(int)blendMode][0];
    static int GH = TextureManager::LoadTexture("Assets/white1x1.png");
    material.resize(1);
    if(material[0].size() <= drawCount){ material[0].resize(drawCount + 1); }
    material[0][drawCount].color_ = color;
    material[0][drawCount].lightingType_ = LIGHTINGTYPE_NONE;
    material[0][drawCount].uvTransform_ = IdentityMat4();
    material[0][drawCount].GH_ = GH;


    // transform
    if(view3D){
        auto& transform = drawData3D->transforms[(int)blendMode][0];
        if(transform.size() <= drawCount){ transform.resize(drawCount + 1); }
        transform[drawCount].world_ = worldMat;
        transform[drawCount].WVP_ = pDxManager_->GetCamera()->GetViewProjectionMat();
        transform[drawCount].worldInverseTranspose_ = Transpose(InverseMatrix(worldMat));
    } else{
        auto& transform = drawData2D->transforms[(int)blendMode][0];
        if(transform.size() <= drawCount){ transform.resize(drawCount + 1); }
        transform[drawCount].world_ = worldMat;
        transform[drawCount].WVP_ = pDxManager_->GetCamera()->GetProjectionMat2D();
        transform[drawCount].worldInverseTranspose_ = IdentityMat4();
    }

    // offsetResourceの数を更新
    auto& offsetData = view3D ? drawData3D->offsetData[(int)blendMode][0] : drawData2D->offsetData[(int)blendMode][0];
    if(offsetData.size() == 0){
        offsetData.resize(1);
    }

    if(offsetData.back().size() <= drawCount){
        offsetData.back().resize(drawCount + 1);
    }


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
    view3D ? drawData3D->indexCount += 2 : drawData2D->indexCount += 2;
    view3D ? drawData3D->totalDrawCount[(int)blendMode][0]++ : drawData2D->totalDrawCount[(int)blendMode][0]++;
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                                                        //
//                                                     リングの追加                                                         //
//                                                                                                                        //
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void PolygonManager::AddRing(const Ring& ring){

    //auto* modelData = 
    //    &primitiveData_[PRIMITIVE_RING][(int)blendMode][(int)cullMode - 1]
    //auto* drawData = isStaticDraw ?
    //    modelDrawData_["ENGINE_DRAW_STATIC_SPRITE" + blendName[(int)blendMode] + cullName[(int)cullMode - 1]].get() :
    //    modelDrawData_["ENGINE_DRAW_SPRITE" + blendName[(int)blendMode] + cullName[(int)cullMode - 1]].get();

    ring;

    // 分割数と刻み幅の設定
    //int diviceCount = 32;
    //float radianStep = 2.0f * 3.14159265358979323846f / diviceCount;

    // リングの頂点を求める

}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                                                        //
//                                                      円柱の追加                                                         //
//                                                                                                                        //
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void PolygonManager::AddCylinder(const Cylinder& cylinder){
    cylinder;
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
    float zfar = pDxManager_->GetCamera()->GetZFar();

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

    auto* modelData = &primitiveData_[PRIMITIVE_OFFSCREEN][(int)blendMode][0];

    static std::string drawDataName;
    drawDataName.clear();
    drawDataName.reserve(128);
    drawDataName += "ENGINE_DRAW_OFFSCREEN";
    drawDataName += blendName[(int)blendMode];
    drawDataName += cullName[0];
    auto* drawData = modelDrawData_[drawDataName].get();

    // Count
    int drawCount = drawData->totalDrawCount[(int)blendMode][0];
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
    drawData->materials[(int)blendMode][0].resize(1);
    auto& material = drawData->materials[(int)blendMode][0].back();
    if(material.size() <= drawCount){ material.resize(drawCount + 1); }
    material[drawCount].color_ = { 1.0f,1.0f,1.0f,1.0f };
    material[drawCount].lightingType_ = LIGHTINGTYPE_NONE;
    material[drawCount].uvTransform_ = uvTransform;
    material[drawCount].GH_ = GH;

    // transform
    auto& transform = drawData->transforms[(int)blendMode][0];
    if(transform.size() <= drawCount){ transform.resize(drawCount + 1); }
    transform[drawCount].world_ = IdentityMat4();
    transform[drawCount].WVP_ = pDxManager_->GetCamera()->GetProjectionMat2D();
    transform[drawCount].worldInverseTranspose_ = IdentityMat4();

    // offsetResourceの数を更新
    auto& offsetData = drawData->offsetData[(int)blendMode][0];
    if(offsetData.size() == 0){
        offsetData.resize(1);
    }

    if(offsetData.back().size() <= drawCount){
        offsetData.back().resize(drawCount + 1);
    }

    // カウントを更新
    objCounts_[(int)DrawOrder::Offscreen]++;
    objCountBlend_[(int)blendMode]++;
    objCountCull_[0]++;
    drawData->indexCount += 6;
    drawData->totalDrawCount[(int)blendMode][0]++;
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

    // カメラ情報
    BaseCamera* pCamera = pDxManager_->GetCamera();
    mapCameraData->position = pCamera->GetTranslation();

    ///////////////////////////////////////////////////////////////////////////////////////////////

    /*                                      頂点情報を書き込む                                      */

    ///////////////////////////////////////////////////////////////////////////////////////////////

    for(auto& modelData : modelDrawData_){

        int modelVertexCount = 0;
        int modelIndexCount = 0;
        auto& item = modelData.second;
        ModelDrawData::modelSwitchIdx_Vertex[modelData.first] = vertexCountAll;
        ModelDrawData::modelSwitchIdx_Index[modelData.first] = indexCountAll;
        if(modelData.second->totalDrawCount == 0){ continue; }

        /*--------------------------------------*/
        //      modelの頂点,index情報を書き込む
        /*--------------------------------------*/
        for(int meshIdx = 0; meshIdx < item->modelData->meshes.size(); meshIdx++){

            int meshVertexCount = (int)item->modelData->meshes[meshIdx].vertices.size();
            int meshIndexCount = (int)item->modelData->meshes[meshIdx].indices.size();

            item->meshSwitchIdx_Vertex.emplace_back(modelVertexCount);// メッシュが切り替わる頂点番号を記録
            item->meshSwitchIdx_Index.emplace_back(modelIndexCount);// メッシュが切り替わるインデックス番号を記録

            // 頂点情報を書き込む
            std::memcpy(
                mapVertexData + ModelDrawData::modelSwitchIdx_Vertex[modelData.first] + item->meshSwitchIdx_Vertex.back(),// に
                item->modelData->meshes[meshIdx].vertices.data(),// から
                sizeof(VertexData) * meshVertexCount// のサイズコピー
            );


            // インデックス情報を書き込む
            std::memcpy(
                mapIndexData + ModelDrawData::modelSwitchIdx_Index[modelData.first] + item->meshSwitchIdx_Index.back(),// に
                item->modelData->meshes[meshIdx].indices.data(),// から
                sizeof(uint32_t) * meshIndexCount// のサイズコピー
            );

            // 頂点の影響度(VertexInfluence)を書き込む
            std::memcpy(
                mapVertexInfluenceData + ModelDrawData::modelSwitchIdx_Vertex[modelData.first] + item->meshSwitchIdx_Vertex.back(),// に
                item->modelData->meshes[meshIdx].vertexInfluences.data(),// から
                sizeof(VertexInfluence) * meshVertexCount// のサイズコピー
            );


            // 数をインクリメント
            modelVertexCount += meshVertexCount;
            modelIndexCount += meshIndexCount;
            vertexCountAll += meshVertexCount;
            indexCountAll += meshIndexCount;
        }
    }


    ///////////////////////////////////////////////////////////////////////////////////////////////

    /*                          transform、material情報・を書き込む                                 */

    ///////////////////////////////////////////////////////////////////////////////////////////////

    for(int blendIdx = 0; blendIdx < (int)BlendMode::kBlendModeCount; blendIdx++){
        for(int cullModeIdx = 0; cullModeIdx < 3; cullModeIdx++){
            for(auto& modelData : modelDrawData_){

                // 各モデルの現在のブレンドモードのインスタンス数確認
                auto& item = modelData.second;
                int instanceCount = (int)item->transforms[blendIdx][cullModeIdx].size();
                int jointSize = (int)item->modelData->defaultSkeleton.joints.size();
                if(instanceCount == 0){ continue; }// インスタンスがない場合はスキップ
                if((int)item->materials[blendIdx][cullModeIdx].size() == 0){ continue; }// プリミティブ用の例外処理

                /*--------------------------------------*/
                //      トランスフォーム情報を書き込む
                /*--------------------------------------*/
                std::memcpy(
                    mapTransformData + instanceCountAll,
                    item->transforms[blendIdx][cullModeIdx].data(),
                    sizeof(TransformMatrix) * instanceCount
                );


                for(int meshIdx = 0; meshIdx < item->modelData->meshes.size(); meshIdx++){

                    /*--------------------------------------*/
                    /*        materialを一列に格納            */
                    /*--------------------------------------*/

                    // すべてのマテリアルを一列に並べる
                    materialArray.insert(
                        materialArray.end(),
                        item->materials[blendIdx][cullModeIdx][meshIdx].begin(),
                        item->materials[blendIdx][cullModeIdx][meshIdx].end()
                    );

                    /*--------------------------------------*/
                    /*      アニメーション用の情報を格納        */
                    /*--------------------------------------*/
                    if(item->drawOrder == (int)DrawOrder::AnimationModel){

                        for(int instanceIdx = 0; instanceIdx < item->paletteData[blendIdx][cullModeIdx].size(); instanceIdx++){
                            std::memcpy(
                                mapPaletteData + animationJointCount + (jointSize * instanceIdx),
                                item->paletteData[blendIdx][cullModeIdx][instanceIdx].data(),
                                sizeof(WellForGPU) * item->paletteData[blendIdx][cullModeIdx][instanceIdx].size()
                            );
                        }
                    }
                }

                // アニメーション用の情報を更新
                if(item->drawOrder == (int)DrawOrder::AnimationModel){
                    animationJointCount += jointSize * instanceCount;
                }

                // インスタンス数をインクリメント
                instanceCountAll += instanceCount;
            }
        }
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


void PolygonManager::SetRenderData(const DrawOrder& drawOrder){

    // モノがなければreturn
    if(modelDrawData_.size() == 0){ return; }
    if(objCounts_[(int)drawOrder] == 0){ return; }


    // インスタンスが切り替わる頂点間隔の決定(モデルは必要ないため0)
    int32_t instanceInterval = 0;
    if(drawOrder == DrawOrder::Line or drawOrder == DrawOrder::Line2D){
        instanceInterval = 2;

    } else if(drawOrder == DrawOrder::Triangle or drawOrder == DrawOrder::Triangle2D){
        instanceInterval = 3;

    } else if(
        drawOrder == DrawOrder::Sprite or drawOrder == DrawOrder::StaticSprite or
        drawOrder == DrawOrder::Quad or drawOrder == DrawOrder::Quad2D or drawOrder == DrawOrder::Offscreen)
    {
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

    for(int blendIdx = 0; blendIdx < (int)BlendMode::kBlendModeCount; blendIdx++){
        if(objCountBlend_[blendIdx] == 0){ continue; }// 描画対象がない場合はスキップ

        for(int cullModeIdx = 0; cullModeIdx < 3; cullModeIdx++){
            if(objCountCull_[cullModeIdx] == 0){ continue; }// 描画対象がない場合はスキップ

            ///////////////////////////////////////////////////////////////////////////////////////////////
            /*                                  パイプラインの切り替え                                      */
            ///////////////////////////////////////////////////////////////////////////////////////////////

            // RootSignature・PSOを設定
            if(drawOrder != DrawOrder::AnimationModel){

                if(drawOrder != DrawOrder::Line && drawOrder != DrawOrder::Line2D && drawOrder != DrawOrder::StaticLine2D){
                    pDxManager_->commandList->SetGraphicsRootSignature(
                        pDxManager_->rootSignatures[blendIdx][(int)PolygonTopology::TRIANGLE][cullModeIdx].rootSignature.Get()
                    );
                    pDxManager_->commandList->SetPipelineState(
                        pDxManager_->pipelines[blendIdx][(int)PolygonTopology::TRIANGLE][cullModeIdx].pipelineState_.Get()
                    );

                } else{
                    pDxManager_->commandList->SetGraphicsRootSignature(
                        pDxManager_->rootSignatures[blendIdx][(int)PolygonTopology::LINE][cullModeIdx].rootSignature.Get()
                    );
                    pDxManager_->commandList->SetPipelineState(
                        pDxManager_->pipelines[blendIdx][(int)PolygonTopology::LINE][cullModeIdx].pipelineState_.Get()
                    );
                }

            } else{
                // アニメーションを行うスキニングモデルの場合
                pDxManager_->commandList->SetGraphicsRootSignature(
                    pDxManager_->skinningRootSignatures[blendIdx][cullModeIdx].rootSignature.Get()
                );
                pDxManager_->commandList->SetPipelineState(
                    pDxManager_->skinningPipelines[blendIdx][cullModeIdx].pipelineState_.Get()
                );
            }

            // Resourceを設定
            pDxManager_->commandList->SetGraphicsRootConstantBufferView(0, cameraResource_->GetGPUVirtualAddress());

            // ライトの数を設定
            int32_t numDirectionalLight = (int32_t)directionalLights_.size();
            int32_t numPointLight = (int32_t)pointLights_.size();
            int32_t numSpotLight = (int32_t)spotLights_.size();
            pDxManager_->commandList->SetGraphicsRoot32BitConstants(6, 1, &numDirectionalLight, 0);
            pDxManager_->commandList->SetGraphicsRoot32BitConstants(7, 1, &numPointLight, 0);
            pDxManager_->commandList->SetGraphicsRoot32BitConstants(8, 1, &numSpotLight, 0);

            // SRVヒープの上のアドレスを格納するハンドル
            D3D12_GPU_DESCRIPTOR_HANDLE srvHandleGPU;
            // マテリアルのテーブルをセット
            srvHandleGPU = ViewManager::GetHandleGPU(DESCRIPTOR_HEAP_TYPE::SRV_CBV_UAV, "instancingResource_Material");
            pDxManager_->commandList->SetGraphicsRootDescriptorTable(1, srvHandleGPU);
            // トランスフォームのテーブルをセット
            srvHandleGPU = ViewManager::GetHandleGPU(DESCRIPTOR_HEAP_TYPE::SRV_CBV_UAV, "instancingResource_Transform");
            pDxManager_->commandList->SetGraphicsRootDescriptorTable(2, srvHandleGPU);
            // DirectionalLightのテーブルをセット
            srvHandleGPU = ViewManager::GetHandleGPU(DESCRIPTOR_HEAP_TYPE::SRV_CBV_UAV, "directionalLight");
            pDxManager_->commandList->SetGraphicsRootDescriptorTable(3, srvHandleGPU);
            // PointLightのテーブルをセット
            srvHandleGPU = ViewManager::GetHandleGPU(DESCRIPTOR_HEAP_TYPE::SRV_CBV_UAV, "pointLight");
            pDxManager_->commandList->SetGraphicsRootDescriptorTable(4, srvHandleGPU);
            // SpotLightのテーブルをセット
            srvHandleGPU = ViewManager::GetHandleGPU(DESCRIPTOR_HEAP_TYPE::SRV_CBV_UAV, "spotLight");
            pDxManager_->commandList->SetGraphicsRootDescriptorTable(5, srvHandleGPU);
            // テクスチャのテーブルをセット
            srvHandleGPU = ViewManager::GetHandleGPU(DESCRIPTOR_HEAP_TYPE::SRV_CBV_UAV, 0);
            pDxManager_->commandList->SetGraphicsRootDescriptorTable(9, srvHandleGPU);
            // パレットのテーブルをセット (アニメーション時のみ)
            if(drawOrder == DrawOrder::AnimationModel){
                srvHandleGPU = ViewManager::GetHandleGPU(DESCRIPTOR_HEAP_TYPE::SRV_CBV_UAV, "SkinningResource_Palette");
                pDxManager_->commandList->SetGraphicsRootDescriptorTable(10, srvHandleGPU);
            }


            ///////////////////////////////////////////////////////////////////////////////////////////////
            /*                                     Modelごとに見ていく                                      */
            ///////////////////////////////////////////////////////////////////////////////////////////////
            for(auto& modelData : modelDrawData_){


                // 各モデルの現在のブレンドモードのインスタンス数
                auto& item = modelData.second;
                int instanceCount = (int)item->transforms[blendIdx][cullModeIdx].size();
                int jointSize = (int)item->modelData->defaultSkeleton.joints.size();
                if(instanceCount == 0){ continue; }// インスタンスがない場合はスキップ
                if((int)item->materials[blendIdx][cullModeIdx].size() == 0){ continue; }// プリミティブ用の例外処理


                // 描画対象でない場合は書き込みを行わずインクリメントだけ行いスキップ
                if(item->drawOrder != (int8_t)drawOrder){
                    meshCountAll += instanceCount * (int)item->modelData->meshes.size();
                    instanceCountAll += instanceCount;
                    continue;
                }

                // オフセットなどを計算し書き込み・描画を行う
                for(int meshIdx = 0; meshIdx < item->modelData->meshes.size(); meshIdx++){


                    /*--------------------------------------*/
                    //      オフセット情報を書き込む
                    /*--------------------------------------*/

                    // 書き込み
                    for(auto& offset : item->offsetData[blendIdx][cullModeIdx][meshIdx]){
                        offset.instanceOffset = instanceCountAll;
                        offset.meshOffset = meshCountAll;
                        offset.jointIndexOffset = animationJointCount;
                        offset.jointinterval = jointSize;
                        offset.primitiveInterval = instanceInterval;
                    }


                    std::memmove(
                        mapOffsetData + meshCountAll,
                        item->offsetData[blendIdx][cullModeIdx][meshIdx].data(),
                        sizeof(OffsetData)* item->totalDrawCount[blendIdx][cullModeIdx]
                    );

                    /*///////////////////////////////////////////////////////////////////////////*/

                    /*                                VBVの設定                                   */

                    /*///////////////////////////////////////////////////////////////////////////*/


                    /*-------------------- 頂点ごとのデータ --------------------*/

                    D3D12_VERTEX_BUFFER_VIEW* vbv = &item->vbv_vertex;
                    int size = sizeof(VertexData);

                    // Resource上の開始位置設定
                    vbv->BufferLocation =
                        modelVertexResource_.Get()->GetGPUVirtualAddress() +
                        ((ModelDrawData::modelSwitchIdx_Vertex[modelData.first] + item->meshSwitchIdx_Vertex[meshIdx]) * size);

                    // 総サイズ、刻み幅の設定
                    vbv->SizeInBytes = size * (int)item->modelData->meshes[meshIdx].vertices.size();
                    vbv->StrideInBytes = size;

                    // VBVのセット
                    pDxManager_->commandList->IASetVertexBuffers(0, 1, vbv);

                    /*-------------------- インスタンスごとのデータ --------------------*/

                    D3D12_VERTEX_BUFFER_VIEW* vbv2 = &item->vbv_instance;
                    size = sizeof(OffsetData);

                    // Resource上の開始位置設定
                    vbv2->BufferLocation =
                        offsetResource_.Get()->GetGPUVirtualAddress() + (meshCountAll * size);

                    // 総サイズ、刻み幅の設定
                    if(drawOrder == DrawOrder::Model or drawOrder == DrawOrder::AnimationModel or drawOrder == DrawOrder::Particle){
                        vbv2->SizeInBytes = size * item->totalDrawCount[blendIdx][cullModeIdx];
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
                        D3D12_VERTEX_BUFFER_VIEW* vbv3 = &item->vbv_skinning;
                        size = sizeof(VertexInfluence);
                        int ofst = ((ModelDrawData::modelSwitchIdx_Vertex[modelData.first] + item->meshSwitchIdx_Vertex[meshIdx]) * size);
                        // Resource上の開始位置設定
                        vbv3->BufferLocation =
                            vertexInfluenceResource_.Get()->GetGPUVirtualAddress() +
                            ofst;

                        // 総サイズ、刻み幅の設定
                        vbv3->SizeInBytes = size * (UINT)item->modelData->meshes[meshIdx].vertices.size();
                        vbv3->StrideInBytes = size;

                        // VBVのセット
                        pDxManager_->commandList->IASetVertexBuffers(2, 1, vbv3);
                    }

                    /*///////////////////////////////////////////////////////////////////////////*/

                    /*                                IBVの設定                                   */

                    /*///////////////////////////////////////////////////////////////////////////*/


                    D3D12_INDEX_BUFFER_VIEW* ibv = &item->ibv;
                    size = sizeof(int32_t);

                    // Resource上の開始位置設定
                    ibv->BufferLocation =
                        modelIndexResource_.Get()->GetGPUVirtualAddress() +
                        ((ModelDrawData::modelSwitchIdx_Index[modelData.first] + item->meshSwitchIdx_Index[meshIdx]) * size);

                    // 総サイズ、刻み幅の設定
                    ibv->SizeInBytes = size * (int)item->modelData->meshes[meshIdx].indices.size();
                    ibv->Format = DXGI_FORMAT_R32_UINT;

                    // IBVのセット
                    pDxManager_->commandList->IASetIndexBuffer(ibv);


                    /*/////////////////////////////////////////////////////////////////*/

                    /*      　                 メッシュごとに描画                         */

                    /*/////////////////////////////////////////////////////////////////*/

                    if(drawOrder == DrawOrder::Model or drawOrder == DrawOrder::AnimationModel or drawOrder == DrawOrder::Particle){

                        if(drawOrder == DrawOrder::Particle){
                            int a = 0;
                            a = a;
                        }

                        pDxManager_->commandList->DrawIndexedInstanced(
                            (int)item->modelData->meshes[meshIdx].indices.size(),
                            item->totalDrawCount[blendIdx][cullModeIdx],
                            0,
                            0,
                            0
                        );

                    } else{

                        // プリミティブは一斉描画
                        pDxManager_->commandList->DrawIndexedInstanced(
                            (int)item->indexCount,
                            1,
                            0,
                            0,
                            0
                        );
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
}

/*---------------- フレームの終わりに積み上げられた情報をまとめてコマンドに積んで描画する関数 -------------------*/

void PolygonManager::DrawToOffscreen(){

#ifdef _DEBUG
    //ImGui::Begin("PostEffect");
    //ImGui::Checkbox("active", &isActivePostEffect_);
    //ImGui::End();
#endif // _DEBUG

    // オフスクリーンの描画依頼をここで出しておく
    if(isActivePostEffect_){
        AddOffscreenResult(ViewManager::GetTextureHandle("blur_0"), BlendMode::NORMAL);
        //AddOffscreenResult(ViewManager::GetTextureHandle("depth_1"), BlendMode::NORMAL);
    } else{
        AddOffscreenResult(ViewManager::GetTextureHandle("offScreen_0"), BlendMode::NORMAL);
    }

    // Resourceに情報を書き込む
    WriteRenderData();

    // 3D
    SetRenderData(DrawOrder::Line);
    SetRenderData(DrawOrder::Model);
    SetRenderData(DrawOrder::AnimationModel);
    SetRenderData(DrawOrder::Triangle);
    SetRenderData(DrawOrder::Quad);
    SetRenderData(DrawOrder::Particle);
    //SetRenderData(DrawOrder::Sprite);

    // 2D
    SetRenderData(DrawOrder::Line2D);
    SetRenderData(DrawOrder::Triangle2D);
    SetRenderData(DrawOrder::Quad2D);
    SetRenderData(DrawOrder::Sprite);
}

void PolygonManager::DrawToBackBuffer(){

    // オフスクリーンの描画結果を貼り付ける
    SetRenderData(DrawOrder::Offscreen);

    // 解像度の変更に影響を受けない2D描画
    SetRenderData(DrawOrder::StaticLine2D);
    SetRenderData(DrawOrder::StaticTriangle2D);
    SetRenderData(DrawOrder::StaticQuad2D);
    SetRenderData(DrawOrder::StaticSprite);
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ライトの追加
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void PolygonManager::AddLight(BaseLight* light){
    switch(light->GetLightType())
    {
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
