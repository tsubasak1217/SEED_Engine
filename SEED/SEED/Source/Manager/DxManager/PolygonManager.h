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
#include <SEED/Lib/Structs/Sprite.h>
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
#include <SEED/Lib/Structs/SkyBox.h>
#include <SEED/Source/Manager/DxManager/PSO/PSOManager.h>

//
using Microsoft::WRL::ComPtr;

class DxManager;
struct D3D12_VERTEX_BUFFER_VIEW;
struct ID3D12Resource;

// 全部の描画が種類ごとに最終的にこの形に情報をまとめられる
struct ModelDrawData{

    // 識別情報
    std::string name;
    uint64_t hash;

    // 各種データ
    ModelData* modelData;
    std::vector<std::vector<MaterialForGPU>> materials;// instance数 * mesh数分ある
    std::unordered_map<std::string, std::vector<TransformMatrix>>transforms;// instance数分 * カメラ数分ある
    std::vector<std::vector<OffsetData>> offsetData;// instance数 * mesh数分ある
    std::vector<std::vector<WellForGPU>> paletteData;// instance数 * palette数分ある

    // VBV
    static D3D12_VERTEX_BUFFER_VIEW vbv_vertex;
    static D3D12_VERTEX_BUFFER_VIEW vbv_instance;
    static D3D12_VERTEX_BUFFER_VIEW vbv_skinning;

    // IBV
    static D3D12_INDEX_BUFFER_VIEW ibv;

    // モデルの種類が切り替わる番号(頂点)
    static std::unordered_map<uint64_t, int32_t>modelSwitchIdx_Vertex;
    std::array<int32_t,128>meshSwitchIdx_Vertex;// メッシュの切り替わる番号(最大16meshまで)

    // モデルの種類が切り替わる番号(インデックス)
    static std::unordered_map<uint64_t, int32_t>modelSwitchIdx_Index;
    std::array<int32_t,128>meshSwitchIdx_Index;// メッシュの切り替わる番号(最大16meshまで)

    // 描画する順番
    int8_t drawOrder = 0;
    int32_t layer = 0;

    // パイプラインの種類
    PipelineType pipelineType = PipelineType::VSPipeline;

    // 対応するPSOのポインタ
    PSO* pso = nullptr;

    // インデックス数(プリミティブ用)
    int32_t indexCount = 0;

    // 描画総数
    uint32_t totalDrawCount;
};

///////////////////////////////////////////////////////////////////////////////
// 本体
///////////////////////////////////////////////////////////////////////////////
class PolygonManager{

private:// 内部で使用する定数や列挙型

    enum PRIMITIVE_TYPE : BYTE{
        PRIMITIVE_TRIANGLE = 0,
        PRIMITIVE_TRIANGLE2D,
        PRIMITIVE_QUAD,
        PRIMITIVE_QUAD2D,
        PRIMITIVE_LINE,
        PRIMITIVE_LINE2D,
        PRIMITIVE_SPRITE,
        PRIMITIVE_BACKSPRITE,
        PRIMITIVE_TEXT,
        PRIMITIVE_TEXT2D,
        // 以下は解像度の変更の影響を受けない描画用
        PRIMITIVE_OFFSCREEN,
        PRIMITIVE_STATIC_TRIANGLE2D,
        PRIMITIVE_STATIC_QUAD2D,
        PRIMITIVE_STATIC_SPRITE,
        PRIMITIVE_STATIC_TEXT2D,
        PRIMITIVE_STATIC_LINE2D,
        kPrimitiveCount
    };

    static const int kPrimitiveVariation = kPrimitiveCount;

    enum class DrawOrder : BYTE{
        SkyBox = 0,
        Model,
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
        Text,
        Text2D,
        Offscreen,
        StaticTriangle2D,
        StaticQuad2D,
        StaticSprite,
        StaticText2D,
        StaticLine2D,
        // カウント用。これより後ろには追加しないこと
        DrawOrderCount
    };

    // GPUハンドルの種類
    enum class HANDLE_TYPE : BYTE{
        TextureTable = 0,
        InstancingResource_Transform,
        InstancingResource_Material,
        SkinningResource_Palette,
        CameraResource,
        DirectionalLight,
        PointLight,
        SpotLight,
    };

public:// 根幹をなす関数

    PolygonManager(DxManager* pDxManager);
    ~PolygonManager();
    void InitResources();
    void BindFrameDatas();
    void BindCameraDatas(const std::string& cameraName);
    void Finalize();
    void Reset();

public:
    void DrawToOffscreen(const std::string& cameraName);
    void DrawToBackBuffer();

public:
    void AddLight(BaseLight* light);
    void AddSkyBoxDrawCommand(){ skyBoxAdded_ = true; }

private:

    void InitializePrimitive();
    void MapOnce();

public:// 頂点情報の追加に関わる関数

    void AddTriangle(
        const Vector4& v1, const Vector4& v2, const Vector4& v3,
        const Matrix4x4& worldMat, const Vector4& color,
        int32_t lightingType, const Matrix4x4& uvTransform, bool view3D, bool isApplyViewMat,
        uint32_t GH, BlendMode blendMode,
        D3D12_CULL_MODE cullMode = D3D12_CULL_MODE::D3D12_CULL_MODE_BACK, bool isStaticDraw = false,
        DrawLocation drawLocation = DrawLocation::Not2D, int32_t layer = 0
    );

    void AddTrianglePrimitive(
        const Vector4& v1, const Vector4& v2, const Vector4& v3,
        const Vector2& texCoordV1, const Vector2& texCoordV2, const Vector2& texCoordV3, const Vector4& color,
        uint32_t GH, BlendMode blendMode, int32_t lightingType, const Matrix4x4& uvTransform,
        D3D12_CULL_MODE cullMode = D3D12_CULL_MODE::D3D12_CULL_MODE_BACK
    );

    void AddQuad(
        const Vector3& v1, const Vector3& v2, const Vector3& v3, const Vector3& v4,
        const Vector2& texCoordV1, const Vector2& texCoordV2, const Vector2& texCoordV3, const Vector2& texCoordV4,
        const Matrix4x4& worldMat, const Vector4& color,
        int32_t lightingType, const Matrix4x4& uvTransform, bool view3D, bool isApplyViewMat,
        uint32_t GH, BlendMode blendMode,bool isText = false,
        D3D12_CULL_MODE cullMode = D3D12_CULL_MODE::D3D12_CULL_MODE_BACK, bool isStaticDraw = false,
        DrawLocation drawLocation = DrawLocation::Not2D, int32_t layer = 0
    );

    void AddQuadPrimitive(
        const Vector4& v1, const Vector4& v2, const Vector4& v3, const Vector4& v4,
        const Vector2& texCoordV1, const Vector2& texCoordV2, const Vector2& texCoordV3, const Vector2& texCoordV4,
        const Vector4& color, uint32_t GH, BlendMode blendMode, int32_t lightingType, const Matrix4x4& uvTransform,
        D3D12_CULL_MODE cullMode = D3D12_CULL_MODE::D3D12_CULL_MODE_BACK
    );

    void AddSprite(
        const struct Sprite& sprite, bool isSystemDraw = false
    );

    void AddModel(Model* model);

    void AddLine(
        const Vector4& v1, const Vector4& v2,
        const Matrix4x4& worldMat, const Vector4& color,
        bool view3D,bool isApplyViewMat, BlendMode blendMode, bool isStaticDraw = false,
        DrawLocation drawLocation = DrawLocation::Not2D, int32_t layer = 0, bool alwaysWrite = false
    );

private:
    void AddOffscreenResult(uint32_t GH, BlendMode blendMode);
    void AddSkyBox();

private:

    void WriteRenderData();
    void SetRenderData(const std::string& cameraName, const DrawOrder& drawOrder);

private:// 外部参照のためのポインタ変数

    DxManager* pDxManager_;

private:// 描画上限や頂点数などの定数

    static const int32_t kMaxTriangleCount_ = 0xffff;
    static const int32_t kMaxQuadCount_ = kMaxTriangleCount_ / 2;
    static const int32_t kMaxMeshCount_ = 0xffff;
    static const int32_t kMaxVerticesCountInResource_ = 10240000;
    static const int32_t kMaxModelVertexCount = 5000000;
    static const int32_t kMaxSpriteCount = 1024;
    static const int32_t kMaxLineCount_ = 512000;
    static const int32_t kMeshletIndexCount = 64;

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
    // カメラが切り替わるインスタンス数
    std::unordered_map<std::string, int32_t> cameraSwitchInstanceCount_;
    std::unordered_map<std::string, int32_t> cameraOrder_;

private:// 実際に頂点情報や色などの情報が入っている変数

    // モデル用
    std::unordered_map<uint64_t, std::unique_ptr<ModelDrawData>> modelDrawData_;
    // プリミティブな描画に使用するデータ
    ModelData primitiveData_[kPrimitiveVariation][(int)BlendMode::kBlendModeCount][3];
    uint32_t primitiveDrawCount_[kPrimitiveVariation][(int)BlendMode::kBlendModeCount][3];

    // 上記をパイプラインごとにまとめたもの
    std::unordered_map<PSO*, std::list<ModelDrawData*>> drawLists_;
    // 上記の描画リストを描画順に並べ替えたもの
    std::list<std::list<ModelDrawData*>*> orderedDrawLists_;

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
    int32_t directionalLightCount_ = 0;
    int32_t pointLightCount_ = 0;
    int32_t spotLightCount_ = 0;

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

private:// GPUハンドルまとめ
    std::vector<D3D12_GPU_DESCRIPTOR_HANDLE> gpuHandles_;

private:
    bool isWrited_ = false; // 描画データを書き込んだかどうか
    bool skyBoxAdded_ = false; // SkyBoxが追加されたかどうか
};