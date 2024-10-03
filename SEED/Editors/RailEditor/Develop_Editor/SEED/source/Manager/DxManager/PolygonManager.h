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
    uint16_t index;
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


class PolygonManager{

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
        int32_t lightingType, const Matrix4x4& uvTransform, bool view3D, uint32_t GH, bool isStaticDraw = false
    );

    void AddQuad(
        const Vector3& v1, const Vector3& v2, const Vector3& v3, const Vector3& v4,
        const Matrix4x4& worldMat, const Vector4& color,
        int32_t lightingType, const Matrix4x4& uvTransform, bool view3D, uint32_t GH, bool isStaticDraw = false
    );

    void AddSprite(
        const Vector2& size, const Matrix4x4& worldMat,
        uint32_t GH, const Vector4& color, const Matrix4x4& uvTransform,const Vector2& anchorPoint,
        bool isStaticDraw = true,bool isSystemDraw = false
    );

    void AddModel(Model* model, bool isStaticDraw = false);


    void AddLine(
        const Vector4& v1, const Vector4& v2,
        const Matrix4x4& worldMat, const Vector4& color,
        bool view3D, bool isStaticDraw = false
    );

private:

    void SetRenderData(InputData* input, bool isStaticDraw = false,bool isLine = false);

private:// 外部参照のためのポインタ変数

    DxManager* pDxManager_;

private:// 描画上限や頂点数などの定数

    static const int32_t kMaxTriangleCount_ = 16384;
    static const int32_t kMaxQuadCount_ = kMaxTriangleCount_ / 2;
    static const int32_t kMaxModelCount_ = 1048;
    static const int32_t kMaxModelVertexCount = 40000;
    static const int32_t kMaxSpriteCount = 1024;
    static const int32_t kMaxLineCount_ = 40000;

private:// 現在の描画数や頂点数などを格納する変数

    static uint32_t triangleIndexCount_;
    static uint32_t quadIndexCount_;
    static uint32_t modelIndexCount_;
    static uint32_t spriteCount_;
    static uint32_t lineCount_;

    int vertexCountAll = 0;
    

private:

    static const int kNumMeshVariation = 7;
    enum MESH_TYPE : BYTE{
        MESHTYPE_TRIANGLE = 0,
        MESHTYPE_QUAD,
        MESHTYPE_MODEL,
        MESHTYPE_SPRITE,
        MESHTYPE_OFFSCREEN,
        MESHTYPE_LINE,
    };

private:// 実際に頂点情報や色などの情報が入っている変数

    // 奥の配列[2]はオフスクリーン前後の描画を分けるもの
    InputData inputData_[kNumMeshVariation][2];


private:// Resourceを格納する変数

    // 奥の配列[2]はオフスクリーン前後の描画を分けるもの
    ComPtr<ID3D12Resource> vertexResource_[kNumMeshVariation][2];
    ComPtr<ID3D12Resource> indexResource_[kNumMeshVariation][2];
    ComPtr<ID3D12Resource> materialResource_[kNumMeshVariation][2];
    ComPtr<ID3D12Resource> wvpResource_[kNumMeshVariation][2];
    ComPtr<ID3D12Resource> numElementResource_[kNumMeshVariation][2];
    ComPtr<ID3D12Resource> keyIndexResource_[kNumMeshVariation][2];

private:

    bool isActivePostEffect_ = false;
};