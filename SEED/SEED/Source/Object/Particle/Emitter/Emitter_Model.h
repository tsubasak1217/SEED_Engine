#pragma once
#include <SEED/Source/Object/Particle/Emitter/Emitter.h>

// パーティクルを発生させるための構造体
class Emitter_Model : public Emitter_Base{

public:
    Emitter_Model();

public:
    void Edit()override;
    nlohmann::json ExportToJson()override;
    void LoadFromJson(const nlohmann::json& j)override;

private:
    void EditGeneral();
    void EditRangeParameters();
    void EditEaseType();
    void EditMaterial();
    void EditFrequency();

    //---------------------- フラグ類 ----------------------//
public:
    bool isBillboard = true;// ビルボード処理を行うかどうか
    bool isUseGravity = false;// ライトを有効にするかどうか
    bool isUseRotate = false;// 回転処理を行うかどうか
    bool useRotateDirection = false;// 回転方向を自分で決めるかどうか
    bool isRoteteRandomInit_ = false;
    bool useDefaultTexture = true;// モデルのデフォルトのテクスチャを使用するかどうか

    //-------------------- 発生パラメータ ------------------//
public:
    Range1D radiusRange = { 0.5f,3.0f };// 大きさの幅
    Range3D scaleRange = { { 1.0f,1.0f,1.0f },{ 1.0f,1.0f,1.0f }};// スケールの幅
    Vector3 baseDirection = { 0.0f,1.0f,0.0f };// パーティクルの向き
    float directionRange = 1.0f;// パーティクルの向きの範囲(ばらけ具合。1がmax)
    Range1D speedRange = { 0.1f,1.0f };// 速度の幅
    Range1D rotateSpeedRange = { 0.0f,1.0f };// 回転速度の幅
    Vector3 rotateDirection = { 0.0f,1.0f,0.0f };// 回転方向
    float gravity = -9.8f;// 重力
    Range1D lifeTimeRange = { 1.0f,3.0f };// 寿命時間の幅
    D3D12_CULL_MODE cullingMode = D3D12_CULL_MODE_BACK;// カリングモード
    LIGHTING_TYPE lightingType_ = LIGHTINGTYPE_NONE;// ライティングの種類
    std::string emitModelFilePath_ = "Assets/Plane.obj";// 発生モデルのファイルパス
};