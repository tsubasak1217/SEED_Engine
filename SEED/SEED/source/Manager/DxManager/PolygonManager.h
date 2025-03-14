#pragma once
// external
#include <stdint.h>
#include <unordered_map>
#include <vector>
#include <queue>
#include <wrl/client.h>
// local
#include <SEED/Lib/Tensor/Vector4.h>
#include <SEED/Lib/Structs/VertexData.h>
#include <SEED/Lib/Structs/OffsetData.h>
#include <SEED/Lib/Structs/Model.h>
#include <SEED/Lib/Structs/Material.h>
#include <SEED/Lib/Structs/Transform.h>
#include <SEED/Lib/Structs/CameraForGPU.h>
#include <SEED/Lib/Structs/DirectionalLight.h>
#include <SEED/Lib/Structs/PointLight.h>
#include <SEED/Lib/Structs/SpotLight.h>
#include <SEED/Lib/Structs/blendMode.h>
#include <SEED/Lib/Structs/DrawLocation.h>
#include <SEED/Lib/Shapes/Ring.h>
#include <SEED/Lib/Shapes/Cylinder.h>

//
using Microsoft::WRL::ComPtr;

class DxManager;
struct D3D12_VERTEX_BUFFER_VIEW;
struct ID3D12Resource;


struct ModelDrawData{

    // 各種データ
    ModelData* modelData;
    std::vector<std::vector<MaterialForGPU>> materials[(int32_t)BlendMode::kBlendModeCount][3];
    std::vector<TransformMatrix>transforms[(int32_t)BlendMode::kBlendModeCount][3];
    std::vector<std::vector<OffsetData>> offsetData[(int32_t)BlendMode::kBlendModeCount][3];
    std::vector<std::vector<WellForGPU>> paletteData[(int32_t)BlendMode::kBlendModeCount][3];

    // VBV
    static D3D12_VERTEX_BUFFER_VIEW vbv_vertex;
    static D3D12_VERTEX_BUFFER_VIEW vbv_instance;
    static D3D12_VERTEX_BUFFER_VIEW vbv_skinning;
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

    // 描画総数
    uint32_t totalDrawCount[(int32_t)BlendMode::kBlendModeCount][3];
};


class PolygonManager{

private:// 内部で使用する定数や列挙型

    static const int kPrimitiveVariation = 13;
    enum PRIMITIVE_TYPE : BYTE{
        PRIMITIVE_TRIANGLE = 0,
        PRIMITIVE_TRIANGLE2D,
        PRIMITIVE_QUAD,
        PRIMITIVE_QUAD2D,
        PRIMITIVE_LINE,
        PRIMITIVE_LINE2D,
        PRIMITIVE_SPRITE,
        PRIMITIVE_BACKSPRITE,
        //PRIMITIVE_RING,
        //PRIMITIVE_CYLINDER,
        // 以下は解像度の変更の影響を受けない描画用
        PRIMITIVE_OFFSCREEN,
        PRIMITIVE_STATIC_TRIANGLE2D,
        PRIMITIVE_STATIC_QUAD2D,
        PRIMITIVE_STATIC_SPRITE,
        PRIMITIVE_STATIC_LINE2D,
    };


    enum class DrawOrder : BYTE{
        Model = 0,
        AnimationModel,
        Triangle,
        Quad,
        Line,
        Particle,
        Triangle2D,
        Quad2D,
        Line2D,
        Sprite,
        BackSprite,
        //Ring,
        //Cylinder,
        Offscreen,
        StaticTriangle2D,
        StaticQuad2D,
        StaticSprite,
        StaticLine2D,
        // カウント用。これより後ろには追加しないこと
        DrawOrderCount
    };



public:// 根幹をなす関数

    PolygonManager(DxManager* pDxManager);
    ~PolygonManager();
    void InitResources();
    void Finalize();
    void Reset();

public:
    void DrawToOffscreen();
    void DrawToBackBuffer();

public:
    void AddLight(BaseLight* light);

private:

    void InitializePrimitive();
    void MapOnce();

public:// 頂点情報の追加に関わる関数

    void AddTriangle(
        const Vector4& v1, const Vector4& v2, const Vector4& v3,
        const Matrix4x4& worldMat, const Vector4& color,
        int32_t lightingType, const Matrix4x4& uvTransform, bool view3D,
        uint32_t GH, BlendMode blendMode,
        D3D12_CULL_MODE cullMode = D3D12_CULL_MODE::D3D12_CULL_MODE_BACK, bool isStaticDraw = false,
        DrawLocation drawLocation = DrawLocation::Not2D, uint32_t layer = 0
    );

    void AddQuad(
        const Vector3& v1, const Vector3& v2, const Vector3& v3, const Vector3& v4,
        const Matrix4x4& worldMat, const Vector4& color,
        int32_t lightingType, const Matrix4x4& uvTransform, bool view3D,
        uint32_t GH, BlendMode blendMode,
        D3D12_CULL_MODE cullMode = D3D12_CULL_MODE::D3D12_CULL_MODE_BACK, bool isStaticDraw = false,
        DrawLocation drawLocation = DrawLocation::Not2D, uint32_t layer = 0
    );

    void AddSprite(
        const Vector2& size, const Matrix4x4& worldMat,
        uint32_t GH, const Vector4& color, const Matrix4x4& uvTransform, bool flipX, bool flipY,
        const Vector2& anchorPoint, const Vector2& clipLT, const Vector2& clipSize, BlendMode blendMode,
        D3D12_CULL_MODE cullMode = D3D12_CULL_MODE::D3D12_CULL_MODE_BACK,
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

    void AddRing(const Ring& ring);
    void AddCylinder(const Cylinder& cylinder);

private:
    void AddOffscreenResult(uint32_t GH, BlendMode blendMode);

private:

    void WriteRenderData();
    void SetRenderData(const DrawOrder& drawOrder);

private:// 外部参照のためのポインタ変数

    DxManager* pDxManager_;

private:// 描画上限や頂点数などの定数

    static const int32_t kMaxTriangleCount_ = 0xfff;
    static const int32_t kMaxQuadCount_ = kMaxTriangleCount_ / 2;
    static const int32_t kMaxMeshCount_ = 0xffff;
    static const int32_t kMaxVerticesCountInResource_ = 10240000;
    static const int32_t kMaxModelVertexCount = 5000000;
    static const int32_t kMaxSpriteCount = 256;
    static const int32_t kMaxLineCount_ = 512000;

private:// 現在の描画数や頂点数などを格納する変数

    static uint32_t triangleIndexCount_;
    static uint32_t quadIndexCount_;
    static uint32_t modelIndexCount_;
    static uint32_t spriteCount_;
    static uint32_t lineCount_;
    int vertexCountAll = 0;

    // カリングモードごとの描画数
    std::array<int32_t, 3>objCountCull_;
    // ブレンドモードごとの描画数
    std::array<int32_t, (int)BlendMode::kBlendModeCount>objCountBlend_;
    // 描画種類ごとの描画数
    std::array<int32_t, (int)DrawOrder::DrawOrderCount> objCounts_;

private:// 実際に頂点情報や色などの情報が入っている変数

    // モデル用
    std::unordered_map<std::string, std::unique_ptr<ModelDrawData>> modelDrawData_;
    // プリミティブな描画に使用するデータ
    ModelData primitiveData_[kPrimitiveVariation][(int)BlendMode::kBlendModeCount][3];
    uint32_t primitiveDrawCount_[kPrimitiveVariation][(int)BlendMode::kBlendModeCount][3];

private:// ライティング用のデータ-----------------------------------------------------------------

    // データを格納するための変数
    std::vector<DirectionalLight> directionalLights_;
    std::vector<PointLight> pointLights_;
    std::vector<SpotLight> spotLights_;

    // LightingのResource
    ComPtr<ID3D12Resource> directionalLightResource_;
    ComPtr<ID3D12Resource> pointLightResource_;
    ComPtr<ID3D12Resource> spotLightResource_;

    // Map用
    DirectionalLight* mapDirectionalLightData;
    PointLight* mapPointLightData;
    SpotLight* mapSpotLightData;

private:// カメラ用のデータ-----------------------------------------------------------------

    // カメラ用
    ComPtr<ID3D12Resource> cameraResource_;
    // Map用
    CameraForGPU* mapCameraData;

private:// Resource (すべての描画で1つにまとめている)

    // モデル共通リソース
    ComPtr<ID3D12Resource> modelVertexResource_;
    ComPtr<ID3D12Resource> modelIndexResource_;
    ComPtr<ID3D12Resource> modelMaterialResource_;
    ComPtr<ID3D12Resource> modelWvpResource_;
    ComPtr<ID3D12Resource> offsetResource_;
    // スキニング用のリソース
    ComPtr<ID3D12Resource> vertexInfluenceResource_;
    ComPtr<ID3D12Resource> paletteResource_;

    // Map用
    VertexData* mapVertexData;
    uint32_t* mapIndexData;
    MaterialForGPU* mapMaterialData;
    TransformMatrix* mapTransformData;
    OffsetData* mapOffsetData;
    VertexInfluence* mapVertexInfluenceData;
    WellForGPU* mapPaletteData;

private:

    bool isActivePostEffect_ = false;
};