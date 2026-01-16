#pragma once
#include <SEED/Source/Manager/ParticleManager/CPUParticle/Emitter/EmitterBase.h>
#include <string>
#include <nlohmann/json.hpp>

namespace SEED{
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
        void EditGeneral();
        void EditTransformSettings();
        void EditTextureAndDrawSettings();
    #endif

    public:// JSON入出力 ----------------------------------------
        nlohmann::json ExportToJson();
        void LoadFromJson(const nlohmann::json& j);

        // 発生パラメータ ----------------------------------------
    public:
        Transform2D center_;// 中心座標(guizmoに渡せるようTransform構造体)
        Vector2 emitRange_ = { 100.0f,100.0f };// 発生範囲
        Range1D radiusRange_ = { 10.0f,30.0f };// 大きさの幅
        Range2D scaleRange_ = { { 1.0f,1.0f },{ 1.0f,1.0f } };// スケールの幅
        Vector2 baseDirection_ = { 0.0f,1.0f };// パーティクルの向き
        Vector2 goalPosition_ = { 0.0f,0.0f };// ゴール位置(移動する場合の目標位置)
        float directionRange_ = 1.0f;// パーティクルの向きの範囲(ばらけ具合。1がmax)
        Range1D speedRange_ = { 100.0f,300.0f };// 速度の幅
        Range1D rotateSpeedRange_ = { 0.0f,1.0f };// 回転速度の幅

        // 描画設定
        int32_t layer_ = 0;// 描画レイヤー
        DrawLocation drawLocation_ = DrawLocation::Front;// 描画位置
        bool isApplyViewMatrix_ = false;// ビュー行列を適用するかどうか
    };
}