#pragma once
// external
#include <stdint.h>
#include <vector>
#include <wrl/client.h>
// local
#include <Vector4.h>
#include <VertexData.h>
#include <Model.h>
#include <Material.h>
#include <Transform.h>
#include "blendMode.h"

//
using Microsoft::WRL::ComPtr;

class DxManager;
struct D3D12_VERTEX_BUFFER_VIEW;
struct ID3D12Resource;

//struct TriangleData{
//    std::vector<VertexData>vertices;
//    std::vector<Material>colorf;
//    std::vector<TransformMatrix>transform;
//    std::vector<uint32_t>GH;
//};
//
//struct Models{
//    std::vector<ModelData>modelData;
//    std::vector<Material>material;
//    std::vector<TransformMatrix>transform;
//    std::vector<uint32_t>GH;
//};

struct InputItems{
    ModelData* modelData;
    Material material;
    TransformMatrix transform;
};

struct InputData{
    std::list<InputItems>items;
    std::vector<int32_t>keyIndices;
    int instanceCount;
    ID3D12Resource* vertexResource;
    ID3D12Resource* materialResource;
    ID3D12Resource* wvpResource;
    ID3D12Resource* numElementResource;
    ID3D12Resource* keyIndexResource;
    D3D12_VERTEX_BUFFER_VIEW vbv;
};

struct ModelDrawData{

    // 各種データ
    ModelData* modelData;
    std::list<Material>materials[(int32_t)BlendMode::kBlendModeCount];
    std::list<TransformMatrix>transforms[(int32_t)BlendMode::kBlendModeCount];

    // 各BlendModeごとのVBV
    D3D12_VERTEX_BUFFER_VIEW vbv[(int32_t)BlendMode::kBlendModeCount];

    // モデルの種類が切り替わるインデックス
    static std::vector<int32_t>modelSwitchIndices;
    int instanceCount;

    // モデル用リソースの参照用ポインタ
    static ID3D12Resource* vertexResource;
    static ID3D12Resource* materialResource;
    static ID3D12Resource* wvpResource;
};


class PolygonManager {

public:// 根幹をなす関数

    PolygonManager(DxManager* pDxManager);
    ~PolygonManager();
    void InitResources();
    void Finalize();
    void Reset();

public:
    void DrawPolygonAll();
    void DrawResult();

public:// 頂点情報の追加に関わる関数

    void AddTriangle(
        const Vector4& v1, const Vector4& v2, const Vector4& v3,
        const Matrix4x4& worldMat, const Vector4& color,
        int32_t lightingType, const Matrix4x4& uvTransform, bool view3D, 
        uint32_t GH,BlendMode blendMode, bool isStaticDraw = false
    );

    void AddQuad(
        const Vector3& v1, const Vector3& v2, const Vector3& v3, const Vector3& v4,
        const Matrix4x4& worldMat, const Vector4& color,
        int32_t lightingType, const Matrix4x4& uvTransform, bool view3D, 
        uint32_t GH,BlendMode blendMode, bool isStaticDraw = false
    );

    void AddSprite(
        const Vector2& size, const Matrix4x4& worldMat,
        uint32_t GH, const Vector4& color, const Matrix4x4& uvTransform,const Vector2& anchorPoint,
        const Vector2& clipLT, const Vector2& clipSize, BlendMode blendMode,
        bool isStaticDraw = true,bool isSystemDraw = false
    );

    void AddModel(Model* model);

    void AddLine(
        const Vector4& v1, const Vector4& v2,
        const Matrix4x4& worldMat, const Vector4& color,
        bool view3D, BlendMode blendMode, bool isStaticDraw = false
    );

private:

    void SetRenderData(InputData* input, BlendMode blendMode, bool isStaticDraw = false, bool isLine = false);
    void SetModelData();

private:// 外部参照のためのポインタ変数

    DxManager* pDxManager_;

private:// 描画上限や頂点数などの定数

    static const int32_t kMaxTriangleCount_ = 256;
    static const int32_t kMaxQuadCount_ = kMaxTriangleCount_ / 2;
    static const int32_t kMaxModelCount_ = 1024;
    static const int32_t kMaxModelVertexCount = 10000;
    static const int32_t kMaxSpriteCount = 128;
    static const int32_t kMaxLineCount_ = 51200;

private:// 現在の描画数や頂点数などを格納する変数

    static uint32_t triangleIndexCount_;
    static uint32_t quadIndexCount_;
    static uint32_t modelIndexCount_;
    static uint32_t spriteCount_;
    static uint32_t lineCount_;

    int vertexCountAll = 0;

    

private:

    static const int kNumMeshVariation = 6;
    enum MESH_TYPE : BYTE{
        MESHTYPE_TRIANGLE = 0,
        MESHTYPE_QUAD,
        MESHTYPE_MODEL,
        MESHTYPE_SPRITE,
        MESHTYPE_OFFSCREEN,
        MESHTYPE_LINE,
    };

private:// 実際に頂点情報や色などの情報が入っている変数

    InputData inputData_[kNumMeshVariation][(int)BlendMode::kBlendModeCount];

    // モデル用
    std::unordered_map < std::string , std::unique_ptr<ModelDrawData >> modelDrawData_;

private:// Resourceを格納する変数

    ComPtr<ID3D12Resource> vertexResource_[kNumMeshVariation][(int)BlendMode::kBlendModeCount];
    ComPtr<ID3D12Resource> indexResource_[kNumMeshVariation][(int)BlendMode::kBlendModeCount];
    ComPtr<ID3D12Resource> materialResource_[kNumMeshVariation][(int)BlendMode::kBlendModeCount];
    ComPtr<ID3D12Resource> wvpResource_[kNumMeshVariation][(int)BlendMode::kBlendModeCount];
    ComPtr<ID3D12Resource> numElementResource_[kNumMeshVariation][(int)BlendMode::kBlendModeCount];
    ComPtr<ID3D12Resource> keyIndexResource_[kNumMeshVariation][(int)BlendMode::kBlendModeCount];

    // モデル用
    ComPtr<ID3D12Resource> modelVertexResource_;
    ComPtr<ID3D12Resource> modelIndexResource_;
    ComPtr<ID3D12Resource> modelMaterialResource_;
    ComPtr<ID3D12Resource> modelWvpResource_;


private:

    bool isActivePostEffect_ = false;
};