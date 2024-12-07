#pragma once
#include "Range1D.h"
#include "Range3D.h"
#include "BlendMode.h"
#include <vector>


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
    Emitter() = default;
    Emitter(const Emitter& other) = default;
    Emitter(Emitter&& other) = default;
    Emitter& operator=(const Emitter& other) = default;
    Emitter& operator=(Emitter&& other) = default;

    // 更新処理
    void Update();

    //-------------------- 発生パラメータ ------------------//
public:
    ParticleType particleType;// パーティクルの種類
    Range3D positionRange;// 発生範囲
    Range1D radiusRange;// 大きさの幅
    Range1D speedRange;// 速度の幅
    Range1D lifeTimeRange;// 寿命時間の幅
    std::vector<Vector4> colors;// 発生させる色の一覧
    float interval;// 発生間隔
    int32_t numEmitEvery;// 一度に発生させる数
    BlendMode blendMode = BlendMode::ADD;// ブレンドモード


    //-------------------- 管理用パラメータ ------------------//
public:// アクティブ・非アクティブ管理のための変数
    EmitType emitType = EmitType::kOnce;// 発生方法
    int32_t kMaxEmitCount = 1;// 最大発生回数(EmitType::kCustomの時用)
    bool isActive = true;// アクティブかどうか
    bool emitOrder = false;// 発生命令が出されたかどうか

private:
    int32_t emitCount = 0;// 発生させた回数
};