#pragma once
#include <SEED/Source/Object/Particle/Emitter/Emitter.h>

// パーティクルを発生させるための構造体
class Emitter_RingQuad : public Emitter_Base{

public:
    Emitter_RingQuad();

public:
    void Edit()override;
    nlohmann::json ExportToJson()override;
    void LoadFromJson(const nlohmann::json& j)override;

private:
    void EditGeneral();
    void EditRangeParameters();
    void EditMaterial();
    void EditFrequency();

    //---------------------- フラグ類 ----------------------//
public:

    bool isBillboard = true;// ビルボード処理を行うかどうか
    bool isUseGravity = false;// ライトを有効にするかどうか
    bool isUseRotate = false;// 回転処理を行うかどうか

    //-------------------- 発生パラメータ ------------------//
public:
    Range1D radiusRange = { 0.5f,3.0f };// 大きさの幅
    Vector3 baseDirection = { 0.0f,1.0f,0.0f };// パーティクルの向き
    float directionRange = 1.0f;// パーティクルの向きの範囲(ばらけ具合。1がmax)
    Range1D speedRange = { 0.1f,1.0f };// 速度の幅
    Range1D rotateSpeedRange = { 0.0f,1.0f };// 回転速度の幅
    float gravity = -9.8f;// 重力
    Range1D lifeTimeRange = { 1.0f,3.0f };// 寿命時間の幅
    D3D12_CULL_MODE cullingMode = D3D12_CULL_MODE_BACK;// カリングモード

};