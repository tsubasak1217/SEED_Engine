#pragma once
#include <SEED/Source/Manager/ParticleManager/CPUParticle/Emitter/EmitterBase.h>
#include <string>

/// <summary>
// 3Dパーティクルを発生させるエミッター
/// </summary>
class Emitter3D : public EmitterBase{
public:
    Emitter3D();
    ~Emitter3D() = default;

public:// アクセッサ ------------------------------------------
    Vector3 GetCenter() const;

#ifdef _DEBUG// デバッグ用編集関数 -----------------------------
public:
    void Edit();
private:
    void EditGeneral();
    void EditTransformSettings();
    void EditTextureAndDrawSettings();
    void EditModel();
#endif // _DEBUG

public:// JSON入出力 ----------------------------------------
    nlohmann::json ExportToJson();
    void LoadFromJson(const nlohmann::json& j);

    // フラグ類 ---------------------------------------------
public:
    bool isBillboard_ = true;// ビルボード処理を行うかどうか
    bool useRotateDirection_ = false;// 回転方向を自分で決めるかどうか
    bool useDefaultTexture_ = true;// モデルのデフォルトのテクスチャを使用するかどうか

    // 発生パラメータ ----------------------------------------
public:
    Transform center_;// 中心座標(guizmoに渡せるようTransform構造体)
    Vector3 emitRange_ = { 10.0f,10.0f,10.0f };// 発生範囲
    Range1D radiusRange_ = { 0.5f,3.0f };// 大きさの幅
    Range3D scaleRange_ = { { 1.0f,1.0f,1.0f },{ 1.0f,1.0f,1.0f } };// スケールの幅
    Vector3 baseDirection_ = { 0.0f,1.0f,0.0f };// パーティクルの向き
    Vector3 goalPosition_ = { 0.0f,0.0f,0.0f };// ゴール位置(移動する場合の目標位置)
    float directionRange_ = 1.0f;// パーティクルの向きの範囲(ばらけ具合。1がmax)
    Range1D speedRange_ = { 0.1f,1.0f };// 速度の幅
    Range1D rotateSpeedRange_ = { 0.0f,1.0f };// 回転速度の幅
    Vector3 rotateDirection_ = { 0.0f,1.0f,0.0f };// 回転方向
    D3D12_CULL_MODE cullingMode_ = D3D12_CULL_MODE_BACK;// カリングモード
    LIGHTING_TYPE lightingType_ = LIGHTINGTYPE_NONE;// ライティングの種類
    std::string emitModelFilePath_ = "DefaultAssets/Plane/Plane.obj";// 発生モデルのファイルパス
};