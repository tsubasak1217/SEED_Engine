#pragma once
#include "Range1D.h"
#include "Range3D.h"
#include "Vector3.h"
#include "Vector4.h"
#include "BlendMode.h"
#include <vector>
#include <string>

// パーティクルの種類
enum class ParticleType : int32_t{
    kRadial = 0,
};

// パーティクルの発生方法
enum class EmitType : int32_t{
    kOnce = 0,// 一度だけ発生
    kInfinite,// 無限に発生
    kCustom,// カスタム
};


// パーティクルを発生させるための構造体
class Emitter{

public:
    Emitter();
    Emitter(const Emitter& other) = default;
    Emitter(Emitter&& other) = default;
    Emitter& operator=(const Emitter& other) = default;
    Emitter& operator=(Emitter&& other) = default;

    // 更新処理
    void Update();

    //---------------------- フラグ類 ----------------------//
public:
    bool isActive = true;// アクティブかどうか
    bool isAlive = true;// 生存しているかどうか
    bool emitOrder = false;// 発生命令が出されたかどうか
    bool isBillboard = true;// ビルボード処理を行うかどうか
    bool isUseGravity = false;// ライトを有効にするかどうか
    bool isUseRotate = false;// 回転処理を行うかどうか

    //-------------------- 発生パラメータ ------------------//
public:
    ParticleType particleType = ParticleType::kRadial;// パーティクルの種類
    Vector3 center;// 中心座標
    Vector3 emitRange = {10.0f,10.0f,10.0f};// 発生範囲
    Range1D radiusRange = {0.5f,3.0f};// 大きさの幅
    Vector3 baseDirection = { 0.0f,1.0f,0.0f };// パーティクルの向き
    float directionRange = 1.0f;// パーティクルの向きの範囲(ばらけ具合。1がmax)
    Range1D speedRange = {0.1f,1.0f};// 速度の幅
    Range1D rotateSpeedRange = { 0.0f,1.0f };// 回転速度の幅
    float gravity = -9.8f;// 重力
    Range1D lifeTimeRange = {1.0f,3.0f};// 寿命時間の幅
    std::vector<Vector4> colors;// 発生させる色の一覧
    std::vector<std::string> texturePaths;// テクスチャハンドルの一覧
    float interval = 0.1f;// 発生間隔
    int32_t numEmitEvery = 1;// 一度に発生させる数
    BlendMode blendMode = BlendMode::ADD;// ブレンドモード


    //-------------------- 管理用パラメータ ------------------//
public:// アクティブ・非アクティブ管理のための変数
    EmitType emitType = EmitType::kInfinite;// 発生方法
    int32_t kMaxEmitCount = 1;// 最大発生回数(EmitType::kCustomの時用)

private:
    float totalTime;// 経過時間
    int32_t emitCount = 0;// 発生させた回数
};