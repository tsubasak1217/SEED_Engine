#pragma once
// external
#include <stdint.h>
#include <vector>
#include <wrl/client.h>
// local
#include <Vector4.h>
#include <VertexData.h>
#include <OffsetData.h>
#include <Model.h>
#include <Material.h>
#include <Transform.h>
#include "blendMode.h"
#include "DrawLocation.h"

//
using Microsoft::WRL::ComPtr;

class DxManager;
struct D3D12_VERTEX_BUFFER_VIEW;
struct ID3D12Resource;


struct ModelDrawData{

    // 各種データ
    ModelData* modelData;
    std::vector<std::vector<Material>> materials[(int32_t)BlendMode::kBlendModeCount];
    std::vector<TransformMatrix>transforms[(int32_t)BlendMode::kBlendModeCount];
    std::vector<std::vector<OffsetData>> offsetData[(int32_t)BlendMode::kBlendModeCount];

    // VBV
    static D3D12_VERTEX_BUFFER_VIEW vbv_vertex;
    static D3D12_VERTEX_BUFFER_VIEW vbv_instance;
    // IBV
    static D3D12_INDEX_BUFFER_VIEW ibv;

    // モデルの種類が切り替わる番号(頂点)
    static std::unordered_map<std::string, int32_t>modelSwitchIdx_Vertex;
    std::vector<int32_t>meshSwitchIdx_Vertex;// メッシュの切り替わる番号

    // モデルの種類が切り替わる番号(インデックス)
    static std::unordered_map<std::string, int32_t>modelSwitchIdx_Index;
    std::vector<int32_t>meshSwitchIdx_Index;// メッシュの切り替わる番号

    // 描画する順番
    int8_t drawOrder = 0;
    int32_t layer = 0;

    // インデックス数(プリミティブ用)
    int32_t indexCount = 0;
};


class PolygonManager{

private:// 内部で使用する定数や列挙型

    static const int kPrimitiveVariation = 12;
    enum PRIMITIVE_TYPE : BYTE{
        PRIMITIVE_TRIANGLE = 0,
        PRIMITIVE_TRIANGLE2D,
        PRIMITIVE_QUAD,
        PRIMITIVE_QUAD2D,
        PRIMITIVE_LINE,
        PRIMITIVE_LINE2D,
        PRIMITIVE_SPRITE,
        // 以下は解像度の変更の影響を受けない描画用
        PRIMITIVE_OFFSCREEN,
        PRIMITIVE_STATIC_TRIANGLE2D,
        PRIMITIVE_STATIC_QUAD2D,
        PRIMITIVE_STATIC_SPRITE,
        PRIMITIVE_STATIC_LINE2D,
    };


    enum class DrawOrder : BYTE{
        Model = 0,
        Triangle,
        Quad,
        Line,
        Triangle2D,
        Quad2D,
        Line2D,
        Sprite,
        Offscreen,
        StaticTriangle2D,
        StaticQuad2D,
        StaticSprite,
        StaticLine2D,
    };



public:// 根幹をなす関数

    PolygonManager(DxManager* pDxManager);
    ~PolygonManager();
    void InitResources();
    void Finalize();
    void Reset();

public:
    void DrawToOffscreen();
    void DrawResult();

private:

    void InitializePrimitive();

public:// 頂点情報の追加に関わる関数

    void AddTriangle(
        const Vector4& v1, const Vector4& v2, const Vector4& v3,
        const Matrix4x4& worldMat, const Vector4& color,
        int32_t lightingType, const Matrix4x4& uvTransform, bool view3D,
        uint32_t GH, BlendMode blendMode, bool isStaticDraw = false,
        DrawLocation drawLocation = DrawLocation::Not2D, uint32_t layer = 0
    );

    void AddQuad(
        const Vector3& v1, const Vector3& v2, const Vector3& v3, const Vector3& v4,
        const Matrix4x4& worldMat, const Vector4& color,
        int32_t lightingType, const Matrix4x4& uvTransform, bool view3D,
        uint32_t GH, BlendMode blendMode, bool isStaticDraw = false,
        DrawLocation drawLocation = DrawLocation::Not2D, uint32_t layer = 0
    );

    void AddSprite(
        const Vector2& size, const Matrix4x4& worldMat,
        uint32_t GH, const Vector4& color, const Matrix4x4& uvTransform, const Vector2& anchorPoint,
        const Vector2& clipLT, const Vector2& clipSize, BlendMode blendMode,
        bool isStaticDraw = true, DrawLocation drawLocation = DrawLocation::Not2D, uint32_t layer = 0,
        bool isSystemDraw = false
    );

    void AddModel(Model* model);

    void AddLine(
        const Vector4& v1, const Vector4& v2,
        const Matrix4x4& worldMat, const Vector4& color,
        bool view3D, BlendMode blendMode, bool isStaticDraw = false,
        DrawLocation drawLocation = DrawLocation::Not2D, uint32_t layer = 0
    );

private:
    void AddOffscreenResult(uint32_t GH,BlendMode blendMode);

private:

    void WriteRenderData();
    void SetRenderData(const DrawOrder& drawOrder);

private:// 外部参照のためのポインタ変数

    DxManager* pDxManager_;

private:// 描画上限や頂点数などの定数

    static const int32_t kMaxTriangleCount_ = 0xfff;
    static const int32_t kMaxQuadCount_ = kMaxTriangleCount_ / 2;
    static const int32_t kMaxModelCount_ = 1024;
    static const int32_t kMaxMeshCount_ = 0xffff;
    static const int32_t kMaxVerticesCountInResource_ = 10240000;
    static const int32_t kMaxModelVertexCount = 500000;
    static const int32_t kMaxSpriteCount = 1024;
    static const int32_t kMaxLineCount_ = 51200;

private:// 現在の描画数や頂点数などを格納する変数

    static uint32_t triangleIndexCount_;
    static uint32_t quadIndexCount_;
    static uint32_t modelIndexCount_;
    static uint32_t spriteCount_;
    static uint32_t lineCount_;
    int vertexCountAll = 0;

    // 総描画数
    int32_t objCount2D_back_ = 0;
    int32_t objCount2D_front_ = 0;
    int32_t objCount3D_ = 0;
    int32_t objCountStaticDraw_ = 0;

private:// 実際に頂点情報や色などの情報が入っている変数

    // モデル用
    std::unordered_map<std::string, std::unique_ptr<ModelDrawData>> modelDrawData_;

private:// Resource

    // モデル用
    ComPtr<ID3D12Resource> modelVertexResource_;
    ComPtr<ID3D12Resource> modelIndexResource_;
    ComPtr<ID3D12Resource> modelMaterialResource_;
    ComPtr<ID3D12Resource> modelWvpResource_;
    ComPtr<ID3D12Resource> offsetResource_;

private:

    // プリミティブな描画に使用するデータ
    ModelData primitiveData_[kPrimitiveVariation][(int)BlendMode::kBlendModeCount];

private:

    bool isActivePostEffect_ = false;
};