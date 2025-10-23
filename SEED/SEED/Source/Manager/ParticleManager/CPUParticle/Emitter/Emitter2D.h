#pragma once
#include <SEED/Source/Manager/ParticleManager/CPUParticle/Emitter/EmitterBase.h>
#include <string>

/// <summary>
// 2Dパーティクルを発生させるエミッター
/// </summary>
class Emitter2D : public EmitterBase{
public:
    Emitter2D();
    ~Emitter2D() = default;

public:// アクセッサ ------------------------------------------
    Vector2 GetCenter() const;

#ifdef _DEBUG// デバッグ用編集関数 -----------------------------
public:
    void Edit();
private:
    void EditRangeParameters();
    void EditMaterial();
#endif

public:// JSON入出力 ----------------------------------------
    nlohmann::json ExportToJson();
    void LoadFromJson(const nlohmann::json& j);

    // 発生パラメータ ----------------------------------------
public:
    Transform2D center;// 中心座標(guizmoに渡せるようTransform構造体)
    Vector2 emitRange = { 100.0f,100.0f };// 発生範囲
    Range1D radiusRange = { 2.0f,6.0f };// 大きさの幅
    Range2D scaleRange = { { 1.0f,1.0f },{ 1.0f,1.0f } };// スケールの幅
    Vector2 baseDirection = { 0.0f,1.0f };// パーティクルの向き
    Vector2 goalPosition = { 0.0f,0.0f };// ゴール位置(移動する場合の目標位置)
    float directionRange = 1.0f;// パーティクルの向きの範囲(ばらけ具合。1がmax)
    Range1D speedRange = { 0.1f,1.0f };// 速度の幅
    Range1D rotateSpeedRange = { 0.0f,1.0f };// 回転速度の幅
    Range1D lifeTimeRange = { 1.0f,3.0f };// 寿命時間の幅
};