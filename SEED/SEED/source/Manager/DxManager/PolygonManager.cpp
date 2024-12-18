// local
#include <PolygonManager.h>
#include <DxManager.h>
#include <MyMath.h>
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
D3D12_INDEX_BUFFER_VIEW ModelDrawData::ibv;


std::string blendName[(int)BlendMode::kBlendModeCount] = {
    "_Blend::NONE",
    "_Blend::MUL",
    "_Blend::SUB",
    "_Blend::NORMAL",
    "_Blend::ADD",
    "_Blend::SCREEN"
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
    modelIndexResource_ =
        CreateBufferResource(pDxManager_->device.Get(), sizeof(uint32_t) * kMaxModelVertexCount);
    modelMaterialResource_ =
        CreateBufferResource(pDxManager_->device.Get(), sizeof(Material) * kMaxMeshCount_ * 50);
    modelWvpResource_ =
        CreateBufferResource(pDxManager_->device.Get(), sizeof(TransformMatrix) * 0xffff);
    offsetResource_ =
        CreateBufferResource(pDxManager_->device.Get(), sizeof(OffsetData) * 0xffff);


    // primitiveの初期化
    InitializePrimitive();


    ////////////////////////////////////////////////
    // viewの作成
    ////////////////////////////////////////////////

    // SRVのDescの設定
    D3D12_SHADER_RESOURCE_VIEW_DESC instancingSrvDesc[2];
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

    /*------------- Transform用 --------------*/
    instancingSrvDesc[0].Buffer.StructureByteStride = sizeof(TransformMatrix);
    ViewManager::CreateView(
        VIEW_TYPE::SRV, modelWvpResource_.Get(),
        &instancingSrvDesc[0], "instancingResource_Transform"
    );

    /*------------- Material用 --------------*/
    instancingSrvDesc[1].Buffer.StructureByteStride = sizeof(Material);// マテリアルのサイズに変更
    ViewManager::CreateView(
        VIEW_TYPE::SRV, modelMaterialResource_.Get(),
        &instancingSrvDesc[1], "instancingResource_Material"
    );

}

void PolygonManager::Finalize(){}

void PolygonManager::Reset(){

    // モデルの情報をリセット
    modelDrawData_.clear();
    ModelDrawData::modelSwitchIdx_Vertex.clear();
    ModelDrawData::modelSwitchIdx_Index.clear();

    // カウントのリセット
    triangleIndexCount_ = 0;
    quadIndexCount_ = 0;
    modelIndexCount_ = 0;
    spriteCount_ = 0;
    lineCount_ = 0;

    objCount3D_ = 0;
    objCount2D_back_ = 0;
    objCount2D_front_ = 0;
    objCountStaticDraw_ = 0;

    // プリミティブ描画情報の初期化
    InitializePrimitive();
}


// プリミティブ描画情報の初期化
void PolygonManager::InitializePrimitive(){


    for(int blendIdx = 0; blendIdx < (int)BlendMode::kBlendModeCount; blendIdx++){
        // 通常の描画
        modelDrawData_["ENGINE_DRAW_TRIANGLE" + blendName[blendIdx]] = std::make_unique<ModelDrawData>();
        modelDrawData_["ENGINE_DRAW_QUAD" + blendName[blendIdx]] = std::make_unique<ModelDrawData>();
        modelDrawData_["ENGINE_DRAW_LINE" + blendName[blendIdx]] = std::make_unique<ModelDrawData>();
        modelDrawData_["ENGINE_DRAW_TRIANGLE2D" + blendName[blendIdx]] = std::make_unique<ModelDrawData>();
        modelDrawData_["ENGINE_DRAW_QUAD2D" + blendName[blendIdx]] = std::make_unique<ModelDrawData>();
        modelDrawData_["ENGINE_DRAW_SPRITE" + blendName[blendIdx]] = std::make_unique<ModelDrawData>();
        modelDrawData_["ENGINE_DRAW_LINE2D" + blendName[blendIdx]] = std::make_unique<ModelDrawData>();

        // 解像度に影響しない描画
        modelDrawData_["ENGINE_DRAW_STATIC_TRIANGLE2D" + blendName[blendIdx]] = std::make_unique<ModelDrawData>();
        modelDrawData_["ENGINE_DRAW_STATIC_QUAD2D" + blendName[blendIdx]] = std::make_unique<ModelDrawData>();
        modelDrawData_["ENGINE_DRAW_STATIC_SPRITE" + blendName[blendIdx]] = std::make_unique<ModelDrawData>();
        modelDrawData_["ENGINE_DRAW_STATIC_LINE2D" + blendName[blendIdx]] = std::make_unique<ModelDrawData>();
        modelDrawData_["ENGINE_DRAW_OFFSCREEN" + blendName[blendIdx]] = std::make_unique<ModelDrawData>();

    }


    // プリミティブの頂点などの情報を初期化
    for(int blendIdx = 0; blendIdx < (int)BlendMode::kBlendModeCount; blendIdx++){
        for(auto& primitiveData : primitiveData_){
            primitiveData[blendIdx].meshes.clear();
            primitiveData[blendIdx].materials.clear();
        }
    }

    // プリミティブの描画情報を初期化
    for(int32_t blendIdx = 0; blendIdx < (int)BlendMode::kBlendModeCount; blendIdx++){

        // 参照先を設定
        modelDrawData_["ENGINE_DRAW_TRIANGLE" + blendName[blendIdx]]->modelData = &primitiveData_[PRIMITIVE_TRIANGLE][blendIdx];
        modelDrawData_["ENGINE_DRAW_TRIANGLE2D" + blendName[blendIdx]]->modelData = &primitiveData_[PRIMITIVE_TRIANGLE2D][blendIdx];
        modelDrawData_["ENGINE_DRAW_STATIC_TRIANGLE2D" + blendName[blendIdx]]->modelData = &primitiveData_[PRIMITIVE_STATIC_TRIANGLE2D][blendIdx];
        modelDrawData_["ENGINE_DRAW_QUAD" + blendName[blendIdx]]->modelData = &primitiveData_[PRIMITIVE_QUAD][blendIdx];
        modelDrawData_["ENGINE_DRAW_QUAD2D" + blendName[blendIdx]]->modelData = &primitiveData_[PRIMITIVE_QUAD2D][blendIdx];
        modelDrawData_["ENGINE_DRAW_STATIC_QUAD2D" + blendName[blendIdx]]->modelData = &primitiveData_[PRIMITIVE_STATIC_QUAD2D][blendIdx];
        modelDrawData_["ENGINE_DRAW_LINE" + blendName[blendIdx]]->modelData = &primitiveData_[PRIMITIVE_LINE][blendIdx];
        modelDrawData_["ENGINE_DRAW_LINE2D" + blendName[blendIdx]]->modelData = &primitiveData_[PRIMITIVE_LINE2D][blendIdx];
        modelDrawData_["ENGINE_DRAW_STATIC_LINE2D" + blendName[blendIdx]]->modelData = &primitiveData_[PRIMITIVE_STATIC_LINE2D][blendIdx];
        modelDrawData_["ENGINE_DRAW_SPRITE" + blendName[blendIdx]]->modelData = &primitiveData_[PRIMITIVE_SPRITE][blendIdx];
        modelDrawData_["ENGINE_DRAW_STATIC_SPRITE" + blendName[blendIdx]]->modelData = &primitiveData_[PRIMITIVE_STATIC_SPRITE][blendIdx];
        modelDrawData_["ENGINE_DRAW_OFFSCREEN" + blendName[blendIdx]]->modelData = &primitiveData_[PRIMITIVE_OFFSCREEN][blendIdx];

        // 描画順を設定
        modelDrawData_["ENGINE_DRAW_TRIANGLE" + blendName[blendIdx]]->drawOrder = (int8_t)DrawOrder::Triangle;
        modelDrawData_["ENGINE_DRAW_TRIANGLE2D" + blendName[blendIdx]]->drawOrder = (int8_t)DrawOrder::Triangle2D;
        modelDrawData_["ENGINE_DRAW_STATIC_TRIANGLE2D" + blendName[blendIdx]]->drawOrder = (int8_t)DrawOrder::StaticTriangle2D;
        modelDrawData_["ENGINE_DRAW_QUAD" + blendName[blendIdx]]->drawOrder = (int8_t)DrawOrder::Quad;
        modelDrawData_["ENGINE_DRAW_QUAD2D" + blendName[blendIdx]]->drawOrder = (int8_t)DrawOrder::Quad2D;
        modelDrawData_["ENGINE_DRAW_STATIC_QUAD2D" + blendName[blendIdx]]->drawOrder = (int8_t)DrawOrder::StaticQuad2D;
        modelDrawData_["ENGINE_DRAW_LINE" + blendName[blendIdx]]->drawOrder = (int8_t)DrawOrder::Line;
        modelDrawData_["ENGINE_DRAW_LINE2D" + blendName[blendIdx]]->drawOrder = (int8_t)DrawOrder::Line2D;
        modelDrawData_["ENGINE_DRAW_STATIC_LINE2D" + blendName[blendIdx]]->drawOrder = (int8_t)DrawOrder::StaticLine2D;
        modelDrawData_["ENGINE_DRAW_SPRITE" + blendName[blendIdx]]->drawOrder = (int8_t)DrawOrder::Sprite;
        modelDrawData_["ENGINE_DRAW_STATIC_SPRITE" + blendName[blendIdx]]->drawOrder = (int8_t)DrawOrder::StaticSprite;
        modelDrawData_["ENGINE_DRAW_OFFSCREEN" + blendName[blendIdx]]->drawOrder = (int8_t)DrawOrder::Offscreen;
    }
}

/*---------------------------------------------------------------------------------------------------------------*/
/*                                                                                                               */
/*                                               描画に関わる関数                                                   */
/*                                                                                                               */
/*---------------------------------------------------------------------------------------------------------------*/

/*---------------------------- 整数型のカラーコードをVector4に変換する関数 ------------------------------*/

Vector4 FloatColor(uint32_t color){
    float delta = 1.0f / 255.0f;

    Vector4 colorf = {
        float((color >> 24) & 0xff) * delta,
        float((color >> 16) & 0xff) * delta,
        float((color >> 8) & 0xff) * delta,
        float(color & 0xff) * delta
    };

    return colorf;
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
    BlendMode blendMode, bool isStaticDraw,
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
            float zFar = pDxManager_->GetCamera()->zfar_ - layerZ;
            transformed[0].z = zFar;
            transformed[1].z = zFar;
            transformed[2].z = zFar;
        } else{
            float zNear = pDxManager_->GetCamera()->znear_ - layerZ;
            transformed[0].z = zNear;
            transformed[1].z = zNear;
            transformed[2].z = zNear;
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

    auto* modelData = view3D ? &primitiveData_[PRIMITIVE_TRIANGLE][(int)blendMode] : &primitiveData_[PRIMITIVE_TRIANGLE2D][(int)blendMode];
    auto* drawData3D = modelDrawData_["ENGINE_DRAW_TRIANGLE" + blendName[(int)blendMode]].get();
    auto* drawData2D = modelDrawData_["ENGINE_DRAW_TRIANGLE2D" + blendName[(int)blendMode]].get();
    if(isStaticDraw){
        modelData = &primitiveData_[PRIMITIVE_STATIC_TRIANGLE2D][(int)blendMode];
        drawData2D = modelDrawData_["ENGINE_DRAW_STATIC_TRIANGLE2D" + blendName[(int)blendMode]].get();
    }

    // vertexResource
    modelData->meshes.resize(1);
    auto& mesh = modelData->meshes[0];
    mesh.vertices.push_back(VertexData(transformed[0].ToVec4(), Vector2(0.5f, 0.0f), normalVec));
    mesh.vertices.push_back(VertexData(transformed[1].ToVec4(), Vector2(1.0f, 1.0f), normalVec));
    mesh.vertices.push_back(VertexData(transformed[2].ToVec4(), Vector2(0.0f, 1.0f), normalVec));
    //indexResource
    mesh.indices.push_back((view3D ? drawData3D->indexCount : drawData2D->indexCount) + 0);
    mesh.indices.push_back((view3D ? drawData3D->indexCount : drawData2D->indexCount) + 1);
    mesh.indices.push_back((view3D ? drawData3D->indexCount : drawData2D->indexCount) + 2);
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
        drawData3D->materials[(int)blendMode].resize(1);
        auto& material = drawData3D->materials[(int)blendMode].back().emplace_back(Material());
        material.color_ = color;
        material.lightingType_ = lightingType;
        material.uvTransform_ = uvTransform;
        material.GH_ = GH;

    } else{
        drawData2D->materials[(int)blendMode].resize(1);
        auto& material = drawData2D->materials[(int)blendMode].back().emplace_back(Material());
        material.color_ = color;
        material.lightingType_ = lightingType;
        material.uvTransform_ = uvTransform;
        material.GH_ = GH;
    }


    // transform
    if(view3D){
        auto& transform = drawData3D->transforms[(int)blendMode].emplace_back(TransformMatrix());
        transform.world_ = IdentityMat4();
        transform.WVP_ = pDxManager_->GetCamera()->viewProjectionMat_;
    } else{
        auto& transform = drawData2D->transforms[(int)blendMode].emplace_back(TransformMatrix());
        transform.world_ = worldMat;
        transform.WVP_ = pDxManager_->GetCamera()->projectionMat2D_;
    }


    // offsetResourceの数を更新
    if(view3D){
        auto& offsetData = drawData3D->offsetData[(int)blendMode];
        offsetData.resize(1);
        offsetData.back().push_back(OffsetData());
        drawData3D->indexCount += 3;
    } else{
        auto& offsetData = drawData2D->offsetData[(int)blendMode];
        offsetData.resize(1);
        offsetData.back().push_back(OffsetData());
        drawData2D->indexCount += 3;
    }

    triangleIndexCount_++;

    if(view3D){
        objCount3D_++;
    } else{
        if(isStaticDraw){
            objCountStaticDraw_++;
        } else{

        }
    }

    // カウントを更新
    if(view3D){
        objCount3D_++;
    } else{
        if(isStaticDraw){
            objCountStaticDraw_++;
        } else{
            if(drawLocation == DrawLocation::Front){
                objCount2D_front_++;
            } else{
                objCount2D_back_++;
            }
        }
    }
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
    BlendMode blendMode, bool isStaticDraw,
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
            float zFar = pDxManager_->GetCamera()->zfar_ - layerZ;
            transformed[0].z = zFar;
            transformed[1].z = zFar;
            transformed[2].z = zFar;
            transformed[3].z = zFar;
        } else{
            float zNear = pDxManager_->GetCamera()->znear_ - layerZ;
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

    auto* modelData = view3D ? &primitiveData_[PRIMITIVE_QUAD][(int)blendMode] : &primitiveData_[PRIMITIVE_QUAD2D][(int)blendMode];
    auto* drawData3D = modelDrawData_["ENGINE_DRAW_QUAD" + blendName[(int)blendMode]].get();
    auto* drawData2D = modelDrawData_["ENGINE_DRAW_QUAD2D" + blendName[(int)blendMode]].get();
    if(isStaticDraw){
        modelData = &primitiveData_[PRIMITIVE_STATIC_QUAD2D][(int)blendMode];
        drawData2D = modelDrawData_["ENGINE_DRAW_STATIC_QUAD2D" + blendName[(int)blendMode]].get();
    }

    // vertexResource
    modelData->meshes.resize(1);
    auto& mesh = modelData->meshes[0];
    mesh.vertices.push_back(VertexData(transformed[0].ToVec4(), Vector2(0.0f, 0.0f), normalVec));
    mesh.vertices.push_back(VertexData(transformed[1].ToVec4(), Vector2(1.0f, 0.0f), normalVec));
    mesh.vertices.push_back(VertexData(transformed[2].ToVec4(), Vector2(0.0f, 1.0f), normalVec));
    mesh.vertices.push_back(VertexData(transformed[3].ToVec4(), Vector2(1.0f, 1.0f), normalVec));

    //indexResource
    mesh.indices.push_back((view3D ? drawData3D->indexCount : drawData2D->indexCount) + 0);
    mesh.indices.push_back((view3D ? drawData3D->indexCount : drawData2D->indexCount) + 1);
    mesh.indices.push_back((view3D ? drawData3D->indexCount : drawData2D->indexCount) + 3);
    mesh.indices.push_back((view3D ? drawData3D->indexCount : drawData2D->indexCount) + 0);
    mesh.indices.push_back((view3D ? drawData3D->indexCount : drawData2D->indexCount) + 3);
    mesh.indices.push_back((view3D ? drawData3D->indexCount : drawData2D->indexCount) + 2);

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
        drawData3D->materials[(int)blendMode].resize(1);
        auto& material = drawData3D->materials[(int)blendMode].back().emplace_back(Material());
        material.color_ = color;
        material.lightingType_ = lightingType;
        material.uvTransform_ = uvTransform;
        material.GH_ = GH;

    } else{
        drawData2D->materials[(int)blendMode].resize(1);
        auto& material = drawData2D->materials[(int)blendMode].back().emplace_back(Material());
        material.color_ = color;
        material.lightingType_ = lightingType;
        material.uvTransform_ = uvTransform;
        material.GH_ = GH;
    }


    // transform
    if(view3D){
        auto& transform = drawData3D->transforms[(int)blendMode].emplace_back(TransformMatrix());
        transform.world_ = IdentityMat4();
        transform.WVP_ = pDxManager_->GetCamera()->viewProjectionMat_;
    } else{
        auto& transform = drawData2D->transforms[(int)blendMode].emplace_back(TransformMatrix());
        transform.world_ = IdentityMat4();
        transform.WVP_ = pDxManager_->GetCamera()->projectionMat2D_;
    }


    // offsetResourceの数を更新
    if(view3D){
        auto& offsetData = drawData3D->offsetData[(int)blendMode];
        offsetData.resize(1);
        offsetData.back().push_back(OffsetData());
        drawData3D->indexCount += 4;
    } else{
        auto& offsetData = drawData2D->offsetData[(int)blendMode];
        offsetData.resize(1);
        offsetData.back().push_back(OffsetData());
        drawData2D->indexCount += 4;
    }


    // カウントを更新
    if(view3D){
        objCount3D_++;
    } else{
        if(isStaticDraw){
            objCountStaticDraw_++;
        } else{
            if(drawLocation == DrawLocation::Front){
                objCount2D_front_++;
            } else{
                objCount2D_back_++;
            }
        }
    }

    quadIndexCount_++;
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
    uint32_t GH, const Vector4& color, const Matrix4x4& uvTransform, const Vector2& anchorPoint,
    const Vector2& clipLT, const Vector2& clipSize, BlendMode blendMode,
    bool isStaticDraw, DrawLocation drawLocation, uint32_t layer, bool isSystemDraw
){
    assert(spriteCount_ < kMaxSpriteCount);
    blendMode;


    ///////////////////////////////////////////////////////////////////
    /*---------------------- 頂点・法線などを求める ---------------------*/
    ///////////////////////////////////////////////////////////////////

    // 遠近
    float layerZ = 0.001f * layer;
    float zNear = pDxManager_->GetCamera()->znear_ - layerZ;
    float zfar = pDxManager_->GetCamera()->zfar_ - layerZ;

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
        &primitiveData_[PRIMITIVE_STATIC_SPRITE][(int)blendMode] :
        &primitiveData_[PRIMITIVE_SPRITE][(int)blendMode];
    auto* drawData = isStaticDraw ?
        modelDrawData_["ENGINE_DRAW_STATIC_SPRITE" + blendName[(int)blendMode]].get() :
        modelDrawData_["ENGINE_DRAW_SPRITE" + blendName[(int)blendMode]].get();

    // 背面描画の場合はzFarに設定
    if(drawLocation == DrawLocation::Back && !isStaticDraw){
        v[0].z = zfar;
        v[1].z = zfar;
        v[2].z = zfar;
        v[3].z = zfar;
    }

    // vertexResource
    modelData->meshes.resize(1);
    auto& mesh = modelData->meshes[0];

    if(MyMath::Length(clipSize) == 0.0f){// 描画範囲指定がない場合
        mesh.vertices.push_back(VertexData(v[0], Vector2(0.0f, 0.0f), normalVec));
        mesh.vertices.push_back(VertexData(v[1], Vector2(1.0f, 0.0f), normalVec));
        mesh.vertices.push_back(VertexData(v[2], Vector2(0.0f, 1.0f), normalVec));
        mesh.vertices.push_back(VertexData(v[3], Vector2(1.0f, 1.0f), normalVec));

    } else{// 描画範囲指定がある場合
        mesh.vertices.push_back(VertexData(v[0], Vector2(clipLT.x / size.x, clipLT.y / size.y), normalVec));
        mesh.vertices.push_back(VertexData(v[1], Vector2((clipLT.x + clipSize.x) / size.x, clipLT.y / size.y), normalVec));
        mesh.vertices.push_back(VertexData(v[2], Vector2(clipLT.x / size.x, (clipLT.y + clipSize.y) / size.y), normalVec));
        mesh.vertices.push_back(VertexData(v[3], Vector2((clipLT.x + clipSize.x) / size.x, (clipLT.y + clipSize.y) / size.y), normalVec));
    }

    //indexResource
    mesh.indices.push_back(drawData->indexCount + 0);
    mesh.indices.push_back(drawData->indexCount + 1);
    mesh.indices.push_back(drawData->indexCount + 3);
    mesh.indices.push_back(drawData->indexCount + 0);
    mesh.indices.push_back(drawData->indexCount + 3);
    mesh.indices.push_back(drawData->indexCount + 2);

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
    drawData->materials[(int)blendMode].resize(1);
    auto& material = drawData->materials[(int)blendMode].back().emplace_back(Material());
    material.color_ = color;
    material.lightingType_ = LIGHTINGTYPE_NONE;
    material.uvTransform_ = uvTransform;
    material.GH_ = GH;

    // transform
    auto& transform = drawData->transforms[(int)blendMode].emplace_back(TransformMatrix());
    transform.world_ = IdentityMat4();
    transform.WVP_ = pDxManager_->GetCamera()->projectionMat2D_;

    // offsetResourceの数を更新
    auto& offsetData = drawData->offsetData[(int)blendMode];
    offsetData.resize(1);
    offsetData.back().push_back(OffsetData());
    drawData->indexCount += 4;

    // カウントを更新
    if(isStaticDraw){
        objCountStaticDraw_++;
    } else{
        if(drawLocation == DrawLocation::Front){
            objCount2D_front_++;
        } else{
            objCount2D_back_++;
        }
    }

    spriteCount_++;

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
    //                     モデル描画データがが存在しないときのみ
    //////////////////////////////////////////////////////////////////////////
    if(modelDrawData_.find(model->modelName_) == modelDrawData_.end()){
        modelDrawData_[model->modelName_] = std::make_unique<ModelDrawData>();
        // vertexResource
        modelDrawData_[model->modelName_]->modelData = ModelManager::GetModelData(model->modelName_);

    }

    // サイズ確保
    auto& item = modelDrawData_[model->modelName_];
    int meshSize = (int)modelDrawData_[model->modelName_]->modelData->meshes.size();


    /////////////////////////////////////////////////////////////////////////
    //                          materialResourceの設定
    /////////////////////////////////////////////////////////////////////////

    modelDrawData_[model->modelName_]->materials[(int)model->blendMode_].resize(meshSize);
    for(int meshIdx = 0; meshIdx < meshSize; meshIdx++){

        auto& material = item->materials[(int)model->blendMode_][meshIdx];
        item->materials[(int)model->blendMode_][meshIdx].resize(material.size() + 1);
        material.back().color_ = model->color_;
        material.back().lightingType_ = model->lightingType_;
        material.back().uvTransform_ = model->uvTransform_[meshIdx];
        material.back().GH_ = model->textureGH_[meshIdx];
    }

    //////////////////////////////////////////////////////////////////////////
    //                          transformResourceの設定
    //////////////////////////////////////////////////////////////////////////

    Matrix4x4 wvp = Multiply(
        model->GetWorldMat(),
        pDxManager_->GetCamera()->viewProjectionMat_
    );

    auto& transform = item->transforms[(int)model->blendMode_].emplace_back(TransformMatrix());
    transform.world_ = model->GetWorldMat();
    transform.WVP_ = wvp;

    //////////////////////////////////////////////////////////////////////////
    //                              offset情報の設定
    //////////////////////////////////////////////////////////////////////////

    modelDrawData_[model->modelName_]->offsetData[(int)model->blendMode_].resize(meshSize);
    for(int meshIdx = 0; meshIdx < meshSize; meshIdx++){
        auto& offsetData = item->offsetData[(int)model->blendMode_][meshIdx];
        offsetData.resize(offsetData.size() + 1);
    }



    // 要素数を更新
    objCount3D_++;
    modelIndexCount_++;
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
            float zFar = pDxManager_->GetCamera()->zfar_ - layerZ;
            v[0].z = zFar;
            v[1].z = zFar;
        } else{
            float zNear = pDxManager_->GetCamera()->znear_ - layerZ;
            v[0].z = zNear;
            v[1].z = zNear;
        }
    }

    ///////////////////////////////////////////////////////////////////
    /*-------------------- ModelDataに情報を追加する--------------------*/
    ///////////////////////////////////////////////////////////////////

    auto* modelData = view3D ? &primitiveData_[PRIMITIVE_LINE][(int)blendMode] : &primitiveData_[PRIMITIVE_LINE2D][(int)blendMode];
    auto* drawData3D = modelDrawData_["ENGINE_DRAW_LINE" + blendName[(int)blendMode]].get();
    auto* drawData2D = modelDrawData_["ENGINE_DRAW_LINE2D" + blendName[(int)blendMode]].get();
    if(isStaticDraw){
        modelData = &primitiveData_[PRIMITIVE_STATIC_LINE2D][(int)blendMode];
        drawData2D = modelDrawData_["ENGINE_DRAW_STATIC_LINE2D" + blendName[(int)blendMode]].get();
    }


    // vertexResource
    modelData->meshes.resize(1);
    auto& mesh = modelData->meshes[0];
    mesh.vertices.push_back(VertexData(v[0], Vector2(0.0f, 0.0f), normalVec));
    mesh.vertices.push_back(VertexData(v[1], Vector2(1.0f, 1.0f), normalVec));
    //indexResource
    mesh.indices.push_back((view3D ? drawData3D->indexCount : drawData2D->indexCount) + 0);
    mesh.indices.push_back((view3D ? drawData3D->indexCount : drawData2D->indexCount) + 1);
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
        drawData3D->materials[(int)blendMode].resize(1);
        auto& material = drawData3D->materials[(int)blendMode].back().emplace_back(Material());
        material.color_ = color;
        material.lightingType_ = LIGHTINGTYPE_NONE;
        material.uvTransform_ = IdentityMat4();
        material.GH_ = TextureManager::LoadTexture("white1x1.png");

    } else{
        drawData2D->materials[(int)blendMode].resize(1);
        auto& material = drawData2D->materials[(int)blendMode].back().emplace_back(Material());
        material.color_ = color;
        material.lightingType_ = LIGHTINGTYPE_NONE;
        material.uvTransform_ = IdentityMat4();
        material.GH_ = TextureManager::LoadTexture("white1x1.png");
    }

    // transform
    if(view3D){
        auto& transform = drawData3D->transforms[(int)blendMode].emplace_back(TransformMatrix());
        transform.world_ = worldMat;
        transform.WVP_ = pDxManager_->GetCamera()->viewProjectionMat_;
    } else{
        auto& transform = drawData2D->transforms[(int)blendMode].emplace_back(TransformMatrix());
        transform.world_ = worldMat;
        transform.WVP_ = pDxManager_->GetCamera()->projectionMat2D_;
    }

    // offsetResourceの数を更新
    if(view3D){
        auto& offsetData = drawData3D->offsetData[(int)blendMode];
        offsetData.resize(1);
        offsetData.back().push_back(OffsetData());
        drawData3D->indexCount += 2;
    } else{
        auto& offsetData = drawData2D->offsetData[(int)blendMode];
        offsetData.resize(1);
        offsetData.back().push_back(OffsetData());
        drawData2D->indexCount += 2;
    }

    // カウントを更新
    if(view3D){
        objCount3D_++;
    } else{
        if(isStaticDraw){
            objCountStaticDraw_++;
        } else{
            if(drawLocation == DrawLocation::Front){
                objCount2D_front_++;
            } else{
                objCount2D_back_++;
            }
        }
    }

    lineCount_++;
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
    float zfar = pDxManager_->GetCamera()->zfar_;

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

    auto* modelData = &primitiveData_[PRIMITIVE_OFFSCREEN][(int)blendMode];
    auto* drawData = modelDrawData_["ENGINE_DRAW_OFFSCREEN" + blendName[(int)blendMode]].get();

    // vertexResource
    modelData->meshes.resize(1);
    auto& mesh = modelData->meshes[0];
    mesh.vertices.push_back(VertexData(v[0], Vector2(0.0f, 0.0f), { 0.0f,0.0f,-1.0f }));
    mesh.vertices.push_back(VertexData(v[1], Vector2(1.0f, 0.0f), { 0.0f,0.0f,-1.0f }));
    mesh.vertices.push_back(VertexData(v[2], Vector2(0.0f, 1.0f), { 0.0f,0.0f,-1.0f }));
    mesh.vertices.push_back(VertexData(v[3], Vector2(1.0f, 1.0f), { 0.0f,0.0f,-1.0f }));

    //indexResource
    mesh.indices.push_back(drawData->indexCount + 0);
    mesh.indices.push_back(drawData->indexCount + 1);
    mesh.indices.push_back(drawData->indexCount + 3);
    mesh.indices.push_back(drawData->indexCount + 0);
    mesh.indices.push_back(drawData->indexCount + 3);
    mesh.indices.push_back(drawData->indexCount + 2);

    // materialResource
    if(modelData->materials.size() == 0){
        modelData->materials.resize(1);
        auto& baseMaterial = modelData->materials[0];
        baseMaterial.textureFilePath_ = "";
        baseMaterial.UV_scale_ = { 1.0f,1.0f,1.0f };
        baseMaterial.UV_offset_ = { 0.0f,0.0f,0.0f };
        baseMaterial.UV_translate_ = { 0.0f,0.0f,0.0f };
    }

    // material
    drawData->materials[(int)blendMode].resize(1);
    auto& material = drawData->materials[(int)blendMode].back().emplace_back(Material());
    material.color_ = { 1.0f,1.0f,1.0f,1.0f };
    material.lightingType_ = LIGHTINGTYPE_NONE;
    material.uvTransform_ = uvTransform;
    material.GH_ = GH;

    // transform
    auto& transform = drawData->transforms[(int)blendMode].emplace_back(TransformMatrix());
    transform.world_ = IdentityMat4();
    transform.WVP_ = pDxManager_->GetCamera()->projectionMat2D_;

    // offsetResourceの数を更新
    auto& offsetData = drawData->offsetData[(int)blendMode];
    offsetData.resize(1);
    offsetData.back().push_back(OffsetData());
    drawData->indexCount += 4;

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
    vertexCountAll = 0;

    // 一列に格納する用の配列
    std::vector<Material> materialArray;

    ///////////////////////////////////////////////////////////////////////////////////////////////
    /*                                          共通の処理                                        */
    ///////////////////////////////////////////////////////////////////////////////////////////////

    // 各リソースのアドレスをMapしてポインタに格納
    VertexData* vertexData;
    uint32_t* indexData;
    Material* materialData;
    TransformMatrix* transformData;


    modelVertexResource_.Get()->Map(0, nullptr, reinterpret_cast<void**>(&vertexData));
    modelIndexResource_.Get()->Map(0, nullptr, reinterpret_cast<void**>(&indexData));
    modelMaterialResource_.Get()->Map(0, nullptr, reinterpret_cast<void**>(&materialData));
    modelWvpResource_.Get()->Map(0, nullptr, reinterpret_cast<void**>(&transformData));

    ///////////////////////////////////////////////////////////////////////////////////////////////

    /*                                      頂点情報を書き込む                                      */

    ///////////////////////////////////////////////////////////////////////////////////////////////

    for(auto& modelData : modelDrawData_){

        int modelVertexCount = 0;
        int modelIndexCount = 0;
        auto& item = modelData.second;
        ModelDrawData::modelSwitchIdx_Vertex[modelData.first] = vertexCountAll;
        ModelDrawData::modelSwitchIdx_Index[modelData.first] = indexCountAll;

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
                vertexData + ModelDrawData::modelSwitchIdx_Vertex[modelData.first] + item->meshSwitchIdx_Vertex.back(),// に
                item->modelData->meshes[meshIdx].vertices.data(),// から
                sizeof(VertexData) * meshVertexCount// のサイズコピー
            );


            // インデックス情報を書き込む
            std::memcpy(
                indexData + ModelDrawData::modelSwitchIdx_Index[modelData.first] + item->meshSwitchIdx_Index.back(),// に
                item->modelData->meshes[meshIdx].indices.data(),// から
                sizeof(uint32_t) * meshIndexCount// のサイズコピー
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

        for(auto& modelData : modelDrawData_){

            // 各モデルの現在のブレンドモードのインスタンス数確認
            auto& item = modelData.second;
            int instanceCount = (int)item->transforms[blendIdx].size();
            if(instanceCount == 0){ continue; }// インスタンスがない場合はスキップ
            if((int)item->materials[blendIdx].size() == 0){ continue; }// プリミティブ用の例外処理

            /*--------------------------------------*/
            //      トランスフォーム情報を書き込む
            /*--------------------------------------*/
            std::memcpy(
                transformData + instanceCountAll,
                item->transforms[blendIdx].data(),
                sizeof(TransformMatrix) * instanceCount
            );


            /*--------------------------------------*/
            /*        materialを一列に格納            */
            /*--------------------------------------*/

            if(item->drawOrder != (int)DrawOrder::Line){
                for(int meshIdx = 0; meshIdx < item->modelData->meshes.size(); meshIdx++){


                    if(item->drawOrder == (int)DrawOrder::Offscreen){
                        // すべてのマテリアルを一列に並べる
                        materialArray.insert(
                            materialArray.end(),
                            item->materials[blendIdx][meshIdx].begin(),
                            item->materials[blendIdx][meshIdx].end()
                        );
                    } else{
                        // すべてのマテリアルを一列に並べる
                        materialArray.insert(
                            materialArray.end(),
                            item->materials[blendIdx][meshIdx].begin(),
                            item->materials[blendIdx][meshIdx].end()
                        );
                    }
                }
            } else{
                for(int meshIdx = 0; meshIdx < item->modelData->meshes.size(); meshIdx++){

                    // すべてのマテリアルを一列に並べる
                    materialArray.insert(
                        materialArray.end(),
                        item->materials[blendIdx][meshIdx].begin(),
                        item->materials[blendIdx][meshIdx].end()
                    );
                }
            }
            // インスタンス数をインクリメント
            instanceCountAll += instanceCount;
        }
    }


    /*--------------------------------------*/
    //       マテリアル情報を書き込む
    /*--------------------------------------*/
    std::memcpy(
        materialData,
        materialArray.data(),
        sizeof(Material) * (int)materialArray.size()
    );


    /*/////////////////////////////////////////////////////////////////*/

    /*      　                     UnMap                               */
    
    /*/////////////////////////////////////////////////////////////////*/
    modelIndexResource_.Get()->Unmap(0, nullptr);
    modelVertexResource_.Get()->Unmap(0, nullptr);
    modelMaterialResource_.Get()->Unmap(0, nullptr);
    modelWvpResource_.Get()->Unmap(0, nullptr);
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

    for(int blendIdx = 0; blendIdx < (int)BlendMode::kBlendModeCount; blendIdx++){


        ///////////////////////////////////////////////////////////////////////////////////////////////
        /*                          BlendModeごとのパイプラインの切り替え                                 */
        ///////////////////////////////////////////////////////////////////////////////////////////////

        // RootSignature・PSOを設定
        if(drawOrder != DrawOrder::Line && drawOrder != DrawOrder::Line2D && drawOrder != DrawOrder::StaticLine2D){
            pDxManager_->commandList->SetGraphicsRootSignature(pDxManager_->commonRootSignature[blendIdx][(int)PolygonTopology::TRIANGLE].Get());
            pDxManager_->commandList->SetPipelineState(pDxManager_->commonPipelineState[blendIdx][(int)PolygonTopology::TRIANGLE].Get());
        } else{
            pDxManager_->commandList->SetGraphicsRootSignature(pDxManager_->commonRootSignature[blendIdx][(int)PolygonTopology::LINE].Get());
            pDxManager_->commandList->SetPipelineState(pDxManager_->commonPipelineState[blendIdx][(int)PolygonTopology::LINE].Get());
        }

        // Resourceを設定
        pDxManager_->commandList->SetGraphicsRootConstantBufferView(3, pDxManager_->lightingResource->GetGPUVirtualAddress());

        // SRVヒープの上のアドレスを格納するハンドル
        D3D12_GPU_DESCRIPTOR_HANDLE srvHandleGPU;
        // マテリアルのテーブルをセット
        srvHandleGPU = ViewManager::GetHandleGPU(DESCRIPTOR_HEAP_TYPE::SRV_CBV_UAV, "instancingResource_Material");
        pDxManager_->commandList->SetGraphicsRootDescriptorTable(0, srvHandleGPU);
        // トランスフォームのテーブルをセット
        srvHandleGPU = ViewManager::GetHandleGPU(DESCRIPTOR_HEAP_TYPE::SRV_CBV_UAV, "instancingResource_Transform");
        pDxManager_->commandList->SetGraphicsRootDescriptorTable(1, srvHandleGPU);
        // テクスチャのテーブルをセット
        srvHandleGPU = ViewManager::GetHandleGPU(DESCRIPTOR_HEAP_TYPE::SRV_CBV_UAV, 0);
        pDxManager_->commandList->SetGraphicsRootDescriptorTable(2, srvHandleGPU);



        ///////////////////////////////////////////////////////////////////////////////////////////////
        /*                                     Modelごとに見ていく                                      */
        ///////////////////////////////////////////////////////////////////////////////////////////////
        for(auto& modelData : modelDrawData_){


            // 各モデルの現在のブレンドモードのインスタンス数
            auto& item = modelData.second;
            int instanceCount = (int)item->transforms[blendIdx].size();
            if(instanceCount == 0){ continue; }// インスタンスがない場合はスキップ
            if((int)item->materials[blendIdx].size() == 0){ continue; }// プリミティブ用の例外処理


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
                
                // map
                OffsetData* offsetData;
                offsetResource_.Get()->Map(0, nullptr, reinterpret_cast<void**>(&offsetData));

                // 書き込み
                for(auto& offset : item->offsetData[blendIdx][meshIdx]){
                    offset.instanceOffset = instanceCountAll;
                    offset.meshOffset = meshCountAll;
                    offset.interval = instanceInterval;
                }

                std::memcpy(
                    offsetData + meshCountAll,
                    item->offsetData[blendIdx][meshIdx].data(),
                    sizeof(OffsetData) * instanceCount
                );

                // Unmap
                offsetResource_.Get()->Unmap(0, nullptr);


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
                if(drawOrder == DrawOrder::Model){
                    vbv2->SizeInBytes = size * instanceCount;
                } else{
                    vbv2->SizeInBytes = size;
                }
                vbv2->StrideInBytes = size;

                // VBVのセット
                assert(meshCountAll < kMaxMeshCount_);// メッシュ数が上限を超えていないか確認
                pDxManager_->commandList->IASetVertexBuffers(1, 1, vbv2);


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

                if(drawOrder == DrawOrder::Model){

                    pDxManager_->commandList->DrawIndexedInstanced(
                        (int)item->modelData->meshes[meshIdx].indices.size(),
                        instanceCount,
                        0,
                        0,
                        0
                    );

                } else{

                    // プリミティブは一斉描画
                    pDxManager_->commandList->DrawIndexedInstanced(
                        (int)item->modelData->meshes[meshIdx].indices.size(),
                        1,
                        0,
                        0,
                        0
                    );
                }

                // 描画総メッシュ数のインクリメント
                meshCountAll += instanceCount;
            }

            // 描画総インスタンス数のインクリメント
            instanceCountAll += instanceCount;
        }
    }
}

/*---------------- フレームの終わりに積み上げられた情報をまとめてコマンドに積んで描画する関数 -------------------*/

void PolygonManager::DrawToOffscreen(){

    // オフスクリーンの描画依頼をここで出しておく
    if(isActivePostEffect_){
        AddOffscreenResult(ViewManager::GetTextureHandle("blur_0"), BlendMode::NORMAL);
    } else{
        //AddOffscreenResult(ViewManager::GetTextureHandle("blur_0"), BlendMode::NORMAL);
        AddOffscreenResult(ViewManager::GetTextureHandle("offScreen_0"), BlendMode::NORMAL);
        //AddOffscreenResult(ViewManager::GetTextureHandle("depth_1"), BlendMode::NORMAL);
    }

    // Resourceに情報を書き込む
    WriteRenderData();

    // 3D
    if(objCount3D_ > 0){
        SetRenderData(DrawOrder::Line);
        SetRenderData(DrawOrder::Model);
        SetRenderData(DrawOrder::Triangle);
        SetRenderData(DrawOrder::Quad);
    }
}

void PolygonManager::DrawResult(){

#ifdef _DEBUG
    //ImGui::Begin("PostEffect");
    //ImGui::Checkbox("active", &isActivePostEffect_);
    //ImGui::End();
#endif // _DEBUG

    // オフスクリーンの描画結果を貼り付ける
    SetRenderData(DrawOrder::Offscreen);

    // 2D
    if(objCount2D_front_ > 0){
        SetRenderData(DrawOrder::Line2D);
        SetRenderData(DrawOrder::Triangle2D);
        SetRenderData(DrawOrder::Quad2D);
        SetRenderData(DrawOrder::Sprite);
    }
}
