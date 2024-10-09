// local
#include <PolygonManager.h>
#include <DxManager.h>
#include <MyMath.h>
#include <Environment.h>
#include "ModelManager.h"
#include "TextureManager.h"

// external
#include <assert.h>

/*------------------------ static, グローバル変数の初期化 ------------------------------*/

uint32_t PolygonManager::triangleIndexCount_ = 0;
uint32_t PolygonManager::quadIndexCount_ = 0;
uint32_t PolygonManager::modelIndexCount_ = 0;
uint32_t PolygonManager::spriteCount_ = 0;
uint32_t PolygonManager::lineCount_ = 0;

/*---------------------------------------------------------------------------------------------------------------*/
/*                                                                                                               */
/*                                             初期化処理・終了処理                                                 */
/*                                                                                                               */
/*---------------------------------------------------------------------------------------------------------------*/

PolygonManager::PolygonManager(DxManager* pDxManager){
    pDxManager_ = pDxManager;
}

PolygonManager::~PolygonManager(){
    //Finalize();
}

void PolygonManager::InitResources(){
    for(int i = 0; i < (int)BlendMode::kBlendModeCount; i++){

        // triangle
        vertexResource_[MESHTYPE_TRIANGLE][i] =
            CreateBufferResource(pDxManager_->device.Get(), (sizeof(VertexData) * 3) * kMaxTriangleCount_);
        materialResource_[MESHTYPE_TRIANGLE][i] =
            CreateBufferResource(pDxManager_->device.Get(), sizeof(Material) * kMaxTriangleCount_);
        wvpResource_[MESHTYPE_TRIANGLE][i] =
            CreateBufferResource(pDxManager_->device.Get(), sizeof(TransformMatrix) * kMaxTriangleCount_);
        numElementResource_[MESHTYPE_TRIANGLE][i] =
            CreateBufferResource(pDxManager_->device.Get(), sizeof(int));
        keyIndexResource_[MESHTYPE_TRIANGLE][i] =
            CreateBufferResource(pDxManager_->device.Get(), sizeof(int) * kMaxTriangleCount_);

        // model
        vertexResource_[MESHTYPE_MODEL][i] =
            CreateBufferResource(pDxManager_->device.Get(), sizeof(VertexData) * kMaxModelVertexCount * kMaxModelCount_);
        materialResource_[MESHTYPE_MODEL][i] =
            CreateBufferResource(pDxManager_->device.Get(), sizeof(Material) * kMaxModelCount_);
        wvpResource_[MESHTYPE_MODEL][i] =
            CreateBufferResource(pDxManager_->device.Get(), sizeof(TransformMatrix) * kMaxModelCount_);
        numElementResource_[MESHTYPE_MODEL][i] =
            CreateBufferResource(pDxManager_->device.Get(), sizeof(int));
        keyIndexResource_[MESHTYPE_MODEL][i] =
            CreateBufferResource(pDxManager_->device.Get(), sizeof(int) * kMaxModelCount_);

        // quad
        vertexResource_[MESHTYPE_QUAD][i] =
            CreateBufferResource(pDxManager_->device.Get(), (sizeof(VertexData) * 6) * kMaxQuadCount_);
        materialResource_[MESHTYPE_QUAD][i] =
            CreateBufferResource(pDxManager_->device.Get(), sizeof(Material) * kMaxQuadCount_);
        wvpResource_[MESHTYPE_QUAD][i] =
            CreateBufferResource(pDxManager_->device.Get(), sizeof(TransformMatrix) * kMaxQuadCount_);
        numElementResource_[MESHTYPE_QUAD][i] =
            CreateBufferResource(pDxManager_->device.Get(), sizeof(int));
        keyIndexResource_[MESHTYPE_QUAD][i] =
            CreateBufferResource(pDxManager_->device.Get(), sizeof(int) * kMaxQuadCount_);

        // sprite
        vertexResource_[MESHTYPE_SPRITE][i] =
            CreateBufferResource(pDxManager_->device.Get(), (sizeof(VertexData) * 4) * kMaxSpriteCount);
        indexResource_[MESHTYPE_SPRITE][i] =
            CreateBufferResource(pDxManager_->device.Get(), (sizeof(uint32_t) * 6) * kMaxSpriteCount);
        materialResource_[MESHTYPE_SPRITE][i] =
            CreateBufferResource(pDxManager_->device.Get(), sizeof(Material) * kMaxSpriteCount);
        wvpResource_[MESHTYPE_SPRITE][i] =
            CreateBufferResource(pDxManager_->device.Get(), sizeof(TransformMatrix) * kMaxSpriteCount);
        numElementResource_[MESHTYPE_SPRITE][i] =
            CreateBufferResource(pDxManager_->device.Get(), sizeof(int));
        keyIndexResource_[MESHTYPE_SPRITE][i] =
            CreateBufferResource(pDxManager_->device.Get(), sizeof(int) * kMaxSpriteCount);

        // offscreen
        vertexResource_[MESHTYPE_OFFSCREEN][i] =
            CreateBufferResource(pDxManager_->device.Get(), (sizeof(VertexData) * 4) * kMaxSpriteCount);
        indexResource_[MESHTYPE_OFFSCREEN][i] =
            CreateBufferResource(pDxManager_->device.Get(), (sizeof(uint32_t) * 6) * kMaxSpriteCount);
        materialResource_[MESHTYPE_OFFSCREEN][i] =
            CreateBufferResource(pDxManager_->device.Get(), sizeof(Material) * kMaxSpriteCount);
        wvpResource_[MESHTYPE_OFFSCREEN][i] =
            CreateBufferResource(pDxManager_->device.Get(), sizeof(TransformMatrix) * kMaxSpriteCount);
        numElementResource_[MESHTYPE_OFFSCREEN][i] =
            CreateBufferResource(pDxManager_->device.Get(), sizeof(int));
        keyIndexResource_[MESHTYPE_OFFSCREEN][i] =
            CreateBufferResource(pDxManager_->device.Get(), sizeof(int) * kMaxSpriteCount);

        // line
        vertexResource_[MESHTYPE_LINE][i] =
            CreateBufferResource(pDxManager_->device.Get(), (sizeof(VertexData) * 2) * kMaxLineCount_);
        materialResource_[MESHTYPE_LINE][i] =
            CreateBufferResource(pDxManager_->device.Get(), sizeof(Material) * kMaxLineCount_);
        wvpResource_[MESHTYPE_LINE][i] =
            CreateBufferResource(pDxManager_->device.Get(), sizeof(TransformMatrix) * kMaxLineCount_);
        numElementResource_[MESHTYPE_LINE][i] =
            CreateBufferResource(pDxManager_->device.Get(), sizeof(int));
        keyIndexResource_[MESHTYPE_LINE][i] =
            CreateBufferResource(pDxManager_->device.Get(), sizeof(int) * kMaxLineCount_);
    }


    // resourceの設定
    for(int i = 0; i < kNumMeshVariation; i++){
        for(int j = 0; j < (int)BlendMode::kBlendModeCount; j++){
            inputData_[i][j].vertexResource = vertexResource_[i][j].Get();
            inputData_[i][j].materialResource = materialResource_[i][j].Get();
            inputData_[i][j].wvpResource = wvpResource_[i][j].Get();
            inputData_[i][j].numElementResource = numElementResource_[i][j].Get();
            inputData_[i][j].keyIndexResource = keyIndexResource_[i][j].Get();
        }
    }
}

void PolygonManager::Finalize(){}

void PolygonManager::Reset(){
    for(int i = 0; i < kNumMeshVariation; i++){
        for(int j = 0; j < (int)BlendMode::kBlendModeCount; j++){

            if(i != MESHTYPE_MODEL){
                for(auto& item : inputData_[i][j].items){
                    delete item.modelData;
                    item.modelData = nullptr;
                }
            }

            inputData_[i][j].items.clear();
            inputData_[i][j].keyIndices.clear();
        }
    }

    triangleIndexCount_ = 0;
    quadIndexCount_ = 0;
    modelIndexCount_ = 0;
    spriteCount_ = 0;
    lineCount_ = 0;
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


/*----------------------------------- 三角形の情報を追加する関数 ---------------------------------------*/

// DrawTriangleが呼び出されるごとに 三角形の情報を積み上げていく
void PolygonManager::AddTriangle(
    const Vector4& v1, const Vector4& v2, const Vector4& v3,
    const Matrix4x4& worldMat, const Vector4& color,
    int32_t lightingType, const Matrix4x4& uvTransform, bool view3D, uint32_t GH, 
    BlendMode blendMode, bool isStaticDraw
){
    assert(triangleIndexCount_ < kMaxTriangleCount_);

    Vector3 transformed[3];

    transformed[0] = Multiply(TransformToVec3(v1), worldMat);
    transformed[1] = Multiply(TransformToVec3(v2), worldMat);
    transformed[2] = Multiply(TransformToVec3(v3), worldMat);

    Vector3 normalVec =
        MyMath::Normalize(MyMath::Cross(
            Vector3(transformed[1].x, transformed[1].y, transformed[1].z) - Vector3(transformed[0].x, transformed[0].y, transformed[0].z),
            Vector3(transformed[2].x, transformed[2].y, transformed[2].z) - Vector3(transformed[1].x, transformed[1].y, transformed[1].z),
            view3D ? kWorld : kScreen
        ));

    Matrix4x4 wvp = Multiply(
        worldMat,
        view3D ?
        pDxManager_->GetCamera()->viewProjectionMat_ :
        pDxManager_->GetCamera()->projectionMat2D_
    );

    /*-------------------- 情報をまとめる --------------------*/

    InputItems item;
    item.modelData = new ModelData();
    // vertexResource
    item.modelData->vertices.push_back(VertexData(v1, Vector2(0.5f, 0.0f), normalVec));
    item.modelData->vertices.push_back(VertexData(v2, Vector2(1.0f, 1.0f), normalVec));
    item.modelData->vertices.push_back(VertexData(v3, Vector2(0.0f, 1.0f), normalVec));
    // materialResource
    item.material.color_ = color;
    item.material.lightingType_ = lightingType;
    item.material.uvTransform_ = uvTransform;
    item.material.GH_ = GH;
    // wvpResource
    item.transform.world_ = worldMat;
    item.transform.WVP_ = wvp;


    /*-------------------- まとめたのを後ろに追加 --------------------*/
    if(isStaticDraw == false){
        inputData_[MESHTYPE_TRIANGLE][(int)blendMode].items.push_back(item);
    } else{
        inputData_[MESHTYPE_TRIANGLE][(int)blendMode].items.push_back(item);
    }

    triangleIndexCount_++;
}


/*----------------------------------- 矩形の情報を追加する関数 ---------------------------------------*/


void PolygonManager::AddQuad(
    const Vector3& v1, const Vector3& v2, const Vector3& v3, const Vector3& v4,
    const Matrix4x4& worldMat, const Vector4& color,
    int32_t lightingType, const Matrix4x4& uvTransform, bool view3D, uint32_t GH,
    BlendMode blendMode, bool isStaticDraw
){
    assert(quadIndexCount_ < kMaxQuadCount_);

    Vector3 transformed[4];

    transformed[0] = Multiply(v1, worldMat);
    transformed[1] = Multiply(v2, worldMat);
    transformed[2] = Multiply(v3, worldMat);
    transformed[3] = Multiply(v4, worldMat);

    Vector3 normalVec =
        MyMath::Normalize(MyMath::Cross(
            Vector3(transformed[1].x, transformed[1].y, transformed[1].z) - Vector3(transformed[0].x, transformed[0].y, transformed[0].z),
            Vector3(transformed[2].x, transformed[2].y, transformed[2].z) - Vector3(transformed[1].x, transformed[1].y, transformed[1].z),
            view3D ? kWorld : kScreen
        ));

    Matrix4x4 wvp = Multiply(
        worldMat,
        view3D ?
        pDxManager_->GetCamera()->viewProjectionMat_ :
        pDxManager_->GetCamera()->projectionMat2D_
    );

    /*-------------------- 情報をまとめる --------------------*/

    InputItems item;
    item.modelData = new ModelData();
    // vertexResource
    item.modelData->vertices.push_back(VertexData(TransformToVec4(v1), Vector2(0.0f, 0.0f), normalVec));
    item.modelData->vertices.push_back(VertexData(TransformToVec4(v2), Vector2(1.0f, 0.0f), normalVec));
    item.modelData->vertices.push_back(VertexData(TransformToVec4(v4), Vector2(1.0f, 1.0f), normalVec));
    item.modelData->vertices.push_back(VertexData(TransformToVec4(v1), Vector2(0.0f, 0.0f), normalVec));
    item.modelData->vertices.push_back(VertexData(TransformToVec4(v4), Vector2(1.0f, 1.0f), normalVec));
    item.modelData->vertices.push_back(VertexData(TransformToVec4(v3), Vector2(0.0f, 1.0f), normalVec));

    // materialResource
    item.material.color_ = color;
    item.material.lightingType_ = lightingType;
    item.material.uvTransform_ = uvTransform;
    item.material.GH_ = GH;
    // wvpResource
    item.transform.world_ = worldMat;
    item.transform.WVP_ = wvp;

    /*-------------------- まとめたのを後ろに追加 --------------------*/

    if(isStaticDraw == false){
        inputData_[MESHTYPE_QUAD][(int)blendMode].items.push_back(item);
    } else{
        inputData_[MESHTYPE_QUAD][(int)blendMode].items.push_back(item);
    }

    quadIndexCount_++;
}


void PolygonManager::AddSprite(
    const Vector2& size, const Matrix4x4& worldMat,
    uint32_t GH, const Vector4& color, const Matrix4x4& uvTransform, const Vector2& anchorPoint,
    const Vector2& clipLT, const Vector2& clipSize, BlendMode blendMode,
    bool isStaticDraw, bool isSystemDraw
){
    assert(spriteCount_ < kMaxSpriteCount);

    // 遠近
    float zNear = pDxManager_->GetCamera()->znear_;
    float zfar = pDxManager_->GetCamera()->zfar_;

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

    } else{
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


    Vector3 normalVec =
        MyMath::Normalize(MyMath::Cross(
            Vector3(v[1].x, v[1].y, v[1].z) - Vector3(v[0].x, v[0].y, v[0].z),
            Vector3(v[2].x, v[2].y, v[2].z) - Vector3(v[1].x, v[1].y, v[1].z),
            kScreen
        ));


    /*-------------------- 情報をまとめる --------------------*/

    InputItems item;
    item.modelData = new ModelData();
    // vertexResource
    if(MyMath::Length(clipSize) == 0.0f){
        item.modelData->vertices.push_back(VertexData(v[0], Vector2(0.0f, 0.0f), normalVec));
        item.modelData->vertices.push_back(VertexData(v[1], Vector2(1.0f, 0.0f), normalVec));
        item.modelData->vertices.push_back(VertexData(v[3], Vector2(1.0f, 1.0f), normalVec));
        item.modelData->vertices.push_back(VertexData(v[0], Vector2(0.0f, 0.0f), normalVec));
        item.modelData->vertices.push_back(VertexData(v[3], Vector2(1.0f, 1.0f), normalVec));
        item.modelData->vertices.push_back(VertexData(v[2], Vector2(0.0f, 1.0f), normalVec));

    } else{// 描画範囲指定がある場合

        item.modelData->vertices.push_back(VertexData(v[0], Vector2(clipLT.x/size.x, clipLT.y / size.y), normalVec));
        item.modelData->vertices.push_back(VertexData(v[1], Vector2((clipLT.x + clipSize.x) / size.x, clipLT.y / size.y), normalVec));
        item.modelData->vertices.push_back(VertexData(v[3], Vector2((clipLT.x + clipSize.x) / size.x, (clipLT.y + clipSize.y) / size.y), normalVec));
        item.modelData->vertices.push_back(VertexData(v[0], Vector2(clipLT.x / size.x, clipLT.y / size.y), normalVec));
        item.modelData->vertices.push_back(VertexData(v[3], Vector2((clipLT.x + clipSize.x) / size.x, (clipLT.y + clipSize.y) / size.y), normalVec));
        item.modelData->vertices.push_back(VertexData(v[2], Vector2(clipLT.x / size.x, (clipLT.y + clipSize.y) / size.y), normalVec));
    }
    // materialResource
    item.material.color_ = color;
    item.material.lightingType_ = false;
    item.material.uvTransform_ = uvTransform;
    item.material.GH_ = GH;
    // wvpResource
    item.transform.world_ = adjustedWorldMat;
    Matrix4x4 wvp = Multiply(adjustedWorldMat, pDxManager_->GetCamera()->projectionMat2D_);
    item.transform.WVP_ = wvp;

    /*-------------------- まとめたのを後ろに追加 --------------------*/

    if(isSystemDraw == false){
        if(isStaticDraw == false){
            inputData_[MESHTYPE_SPRITE][(int)blendMode].items.push_back(item);
        } else{
            inputData_[MESHTYPE_SPRITE][(int)blendMode].items.push_back(item);
        }
    } else{
        inputData_[MESHTYPE_OFFSCREEN][(int)blendMode].items.push_back(item);
    }

    spriteCount_++;
}

void PolygonManager::AddModel(Model* model, bool isStaticDraw){

    Matrix4x4 wvp = Multiply(
        model->GetWorldMat(),
        pDxManager_->GetCamera()->viewProjectionMat_
    );


    /*-------------------- 情報をまとめる --------------------*/


    InputItems item;
    // vertexResource
    item.modelData = ModelManager::GetModelData(model->modelName_);

    // materialResource
    item.material.color_ = model->color_;
    item.material.lightingType_ = model->lightingType_;
    item.material.uvTransform_ = model->GetUVTransform();
    item.material.GH_ = model->GetTextureGH();
    // wvpResource
    item.transform.world_ = model->GetWorldMat();
    item.transform.WVP_ = wvp;


    /*-------------------- まとめたのを後ろに追加 --------------------*/

    if(isStaticDraw == false){
        inputData_[MESHTYPE_MODEL][(int)model->blendMode_].items.emplace_back(item);
    } else{
        inputData_[MESHTYPE_MODEL][(int)model->blendMode_].items.emplace_back(item);
    }

    modelIndexCount_++;
}


void PolygonManager::AddLine(
    const Vector4& v1, const Vector4& v2, const Matrix4x4& worldMat,
    const Vector4& color, bool view3D, BlendMode blendMode, bool isStaticDraw
){
    assert(triangleIndexCount_ < kMaxTriangleCount_);

    Vector3 transformed[3];

    transformed[0] = Multiply(TransformToVec3(v1), worldMat);
    transformed[1] = Multiply(TransformToVec3(v2), worldMat);

    Vector3 normalVec = { 0.0f,0.0f,1.0f };

    Matrix4x4 wvp = Multiply(
        worldMat,
        view3D ?
        pDxManager_->GetCamera()->viewProjectionMat_ :
        pDxManager_->GetCamera()->projectionMat2D_
    );

    /*-------------------- 情報をまとめる --------------------*/

    InputItems item;
    item.modelData = new ModelData();
    // vertexResource
    item.modelData->vertices.push_back(VertexData(v1, Vector2(0.5f, 0.0f), normalVec));
    item.modelData->vertices.push_back(VertexData(v2, Vector2(1.0f, 1.0f), normalVec));
    // materialResource
    item.material.color_ = color;
    item.material.lightingType_ = LIGHTINGTYPE_NONE;
    item.material.uvTransform_ = IdentityMat4();
    item.material.GH_ = TextureManager::LoadTexture("white1x1.png");
    // wvpResource
    item.transform.world_ = worldMat;
    item.transform.WVP_ = wvp;


    /*-------------------- まとめたのを後ろに追加 --------------------*/
    if(isStaticDraw == false){
        inputData_[MESHTYPE_LINE][(int)blendMode].items.push_back(item);
    } else{
        inputData_[MESHTYPE_LINE][(int)blendMode].items.push_back(item);
    }

    lineCount_++;
}


void PolygonManager::SetRenderData(InputData* input, BlendMode blendMode, bool isStaticDraw, bool isLine){
    // モノがなければreturn
    if(input->items.size() == 0){ return; }
    D3D12_VERTEX_BUFFER_VIEW* vbv = &input->vbv;

    /*===========================================================================================*/
    /*                               すべての三角形に共通の設定                                      */
    /*===========================================================================================*/

    // シザー矩形とviewport
    if(isStaticDraw == false){
        pDxManager_->commandList->RSSetViewports(1, &pDxManager_->viewport); // Viewport
        pDxManager_->commandList->RSSetScissorRects(1, &pDxManager_->scissorRect); // Scissor
    } else{
        pDxManager_->commandList->RSSetViewports(1, &pDxManager_->viewport_default); // Viewport
        pDxManager_->commandList->RSSetScissorRects(1, &pDxManager_->scissorRect_default); // Scissor
    }

    // RootSignatureを設定。 PSOに設定しているけど別途設定が必要
    if(!isLine){
        pDxManager_->commandList->SetGraphicsRootSignature(pDxManager_->commonRootSignature[(int)blendMode][(int)PolygonTopology::TRIANGLE].Get());
        pDxManager_->commandList->SetPipelineState(pDxManager_->commonPipelineState[(int)blendMode][(int)PolygonTopology::TRIANGLE].Get());
    } else{
        pDxManager_->commandList->SetGraphicsRootSignature(pDxManager_->commonRootSignature[(int)blendMode][(int)PolygonTopology::LINE].Get());
        pDxManager_->commandList->SetPipelineState(pDxManager_->commonPipelineState[(int)blendMode][(int)PolygonTopology::LINE].Get());
    }

    // 形状を設定。 PSOに設定しているものとはまた別。 同じものを設定すると考えておけば良い
    if(!isLine){
        pDxManager_->commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    } else{
        pDxManager_->commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_LINELIST);
    }

    // Resourceを設定
    pDxManager_->commandList->SetGraphicsRootShaderResourceView(0, input->materialResource->GetGPUVirtualAddress());
    pDxManager_->commandList->SetGraphicsRootShaderResourceView(1, input->wvpResource->GetGPUVirtualAddress());
    pDxManager_->commandList->SetGraphicsRootConstantBufferView(3, pDxManager_->lightingResource->GetGPUVirtualAddress());
    pDxManager_->commandList->SetGraphicsRootShaderResourceView(4, input->keyIndexResource->GetGPUVirtualAddress());
    pDxManager_->commandList->SetGraphicsRootConstantBufferView(5, input->numElementResource->GetGPUVirtualAddress());
    vbv->BufferLocation = input->vertexResource->GetGPUVirtualAddress();

    // グラフハンドルに応じたテクスチャハンドルを得る
    D3D12_GPU_DESCRIPTOR_HANDLE textureSrvHandleGPU;

    // グラフハンドルに応じたテクスチャハンドルを得る
    textureSrvHandleGPU = GetGPUDescriptorHandle(
        pDxManager_->SRV_UAV_DescriptorHeap.Get(),
        pDxManager_->descriptorSizeSRV_UAV,
        1
    );

    // テクスチャのディスクリプタをセット
    pDxManager_->commandList->SetGraphicsRootDescriptorTable(2, textureSrvHandleGPU);


    /*===========================================================================================*/
    /*                                         情報の書き込み                                      */
    /*===========================================================================================*/

    VertexData* vertexData;
    Material* materialData;
    TransformMatrix* transformData;
    int* numElementData;
    int* keyIndexData;

    vertexCountAll = 0;
    input->instanceCount = 0;

    input->vertexResource->Map(0, nullptr, reinterpret_cast<void**>(&vertexData));
    input->materialResource->Map(0, nullptr, reinterpret_cast<void**>(&materialData));
    input->wvpResource->Map(0, nullptr, reinterpret_cast<void**>(&transformData));
    input->numElementResource->Map(0, nullptr, reinterpret_cast<void**>(&numElementData));
    input->keyIndexResource->Map(0, nullptr, reinterpret_cast<void**>(&keyIndexData));


    /*///////////////////////////////////////////////*/

    /*      　     HLSLに送る情報の書き込み              */

    /*///////////////////////////////////////////////*/

    for(auto& item : input->items){

        // 頂点情報
        std::memcpy(
            vertexData + vertexCountAll,
            item.modelData->vertices.data(),
            sizeof(VertexData) * (int)item.modelData->vertices.size()
        );

        // マテリアル情報
        std::memcpy(
            materialData + input->instanceCount,
            &item.material, sizeof(Material)
        );

        // トランスフォーム情報
        std::memcpy(
            transformData + input->instanceCount,
            &item.transform, sizeof(TransformMatrix)
        );

        // 総頂点、インスタンス数をインクリメント
        vertexCountAll += (int)item.modelData->vertices.size();
        input->instanceCount++;

        // インスタンスが切り替わる頂点
        input->keyIndices.push_back(vertexCountAll);
    }

    for(int32_t i = 0; i < input->keyIndices.size(); i++){
        // インスタンスが切り替わる頂点
        std::memcpy(
            keyIndexData + i,
            &input->keyIndices[i], sizeof(int32_t)
        );
    }

    // インスタンス数の設定
    *numElementData = input->instanceCount;


    /*///////////////////////////////////////////////*/

    /*      　            Unmap                      */

    /*///////////////////////////////////////////////*/


    D3D12_RANGE writeRange[5] = {
        {0,sizeof(VertexData) * vertexCountAll},
        {0,sizeof(Material) * input->instanceCount},
        {0,sizeof(TransformMatrix) * input->instanceCount},
        {0,sizeof(int32_t)},
        {0,sizeof(int32_t) * input->instanceCount},
    };

    input->vertexResource->Unmap(0, &writeRange[0]);
    input->materialResource->Unmap(0, &writeRange[1]);
    input->wvpResource->Unmap(0, &writeRange[2]);
    input->numElementResource->Unmap(0, &writeRange[3]);
    input->keyIndexResource->Unmap(0, &writeRange[4]);


    /*///////////////////////////////////////////////*/
    /*                   VBVの設定                    */
    /*///////////////////////////////////////////////*/


    vbv->SizeInBytes = sizeof(VertexData) * vertexCountAll;
    vbv->StrideInBytes = sizeof(VertexData);
    pDxManager_->commandList->IASetVertexBuffers(0, 1, vbv); // VBVのセット


    /*///////////////////////////////////////////////*/

    /*      　              描画                      */

    /*///////////////////////////////////////////////*/

    pDxManager_->commandList->DrawInstanced(
        vertexCountAll,
        1,
        0,
        0
    );


}

/*---------------- フレームの終わりに積み上げられた情報をまとめてコマンドに積んで描画する関数 -------------------*/

void PolygonManager::DrawPolygonAll(){

    for(int i = 0; i < (int)BlendMode::kBlendModeCount; i++){
        // 線
        SetRenderData(&inputData_[MESHTYPE_LINE][i], BlendMode(i), false, true);
        // モデル
        SetRenderData(&inputData_[MESHTYPE_MODEL][i], BlendMode(i));
        // 三角形
        SetRenderData(&inputData_[MESHTYPE_TRIANGLE][i], BlendMode(i));
        // 矩形
        SetRenderData(&inputData_[MESHTYPE_QUAD][i], BlendMode(i));
        // スプライト
        SetRenderData(&inputData_[MESHTYPE_SPRITE][i], BlendMode(i));
    }
}

void PolygonManager::DrawResult(){
    Vector2 windowSize = { float(kWindowSizeX),float(kWindowSizeY) };
    float scaleRate = float(pDxManager_->GetPreResolutionRate());
    Matrix4x4 uvTransform = AffineMatrix({ scaleRate,scaleRate,0.0f }, { 0.0f,0.0f,0.0f }, { 0.0f,0.0f,0.0f });

    //ImGui::Begin("PostEffect");
    //ImGui::Checkbox("active", &isActivePostEffect_);
    //ImGui::End();

    if(isActivePostEffect_){

        AddSprite(windowSize, IdentityMat4(),
            pDxManager_->systemTextures_["blurredTexture_SRV"],
            { 1.0f,1.0f,1.0f,1.0f }, uvTransform, { 0.0f,0.0f },
            { 0.0f,0.0f }, {0.0f,0.0f},BlendMode::NORMAL,true, true
        );
    } else{
        AddSprite(windowSize, IdentityMat4(),
            pDxManager_->systemTextures_["offScreenTexture"],
            { 1.0f,1.0f,1.0f,1.0f }, uvTransform, { 0.0f,0.0f },
            { 0.0f,0.0f }, { 0.0f,0.0f }, BlendMode::NORMAL, true, true
        );
    }

    pDxManager_->TransitionResourceState(
        pDxManager_->depthStencilResource.Get(),
        D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
        D3D12_RESOURCE_STATE_DEPTH_WRITE
    );


    // OffScreenToTextre
    SetRenderData(&inputData_[MESHTYPE_OFFSCREEN][0], BlendMode::NORMAL, true);

}
