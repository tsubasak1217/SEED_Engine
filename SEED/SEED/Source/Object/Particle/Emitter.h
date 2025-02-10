#pragma once
#include <SEED/Lib/Structs/Range1D.h>
#include <SEED/Lib/Structs/Range3D.h>
#include <SEED/Lib/Tensor/Vector3.h>
#include <SEED/Lib/Tensor/Vector4.h>
#include <SEED/Lib/Tensor/Matrix4x4.h>
#include <SEED/Lib/Functions/MyFunc/MatrixFunc.h>
#include <SEED/Lib/Structs/blendMode.h>
#include <vector>
#include <list>
#include <memory>
#include <string>
#include <d3d12.h>

class EmitterGroup;

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

public:
    Vector3 GetCenter() const;

    //---------------------- フラグ類 ----------------------//
public:
    bool isActive = true;// アクティブかどうか
    bool isAlive = true;// 生存しているかどうか
    bool emitOrder = false;// 発生命令が出されたかどうか
    bool isBillboard = true;// ビルボード処理を行うかどうか
    bool isUseGravity = false;// ライトを有効にするかどうか
    bool isUseRotate = false;// 回転処理を行うかどうか
    bool isEdittting = false;// 編集中かどうか

    //-------------------- 発生パラメータ ------------------//
public:
    EmitterGroup* parentGroup = nullptr;// 親グループ
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
    uint32_t cullingMode = (uint32_t)D3D12_CULL_MODE::D3D12_CULL_MODE_BACK - 1;// カリングモード


    //-------------------- 管理用パラメータ ------------------//
public:// アクティブ・非アクティブ管理のための変数
    EmitType emitType = EmitType::kInfinite;// 発生方法
    int32_t kMaxEmitCount = 1;// 最大発生回数(EmitType::kCustomの時用)

private:
    float totalTime;// 経過時間
    int32_t emitCount = 0;// 発生させた回数
};



// パーティクルをプリセット化するための構造体
class EmitterGroup{
public:
    EmitterGroup() = default;
    EmitterGroup(const Matrix4x4* parentMat) : parentMat(parentMat){}

public:
    bool GetIsAlive() const{
        if(emitters.size() == 0){ return true; }
        for(const auto& emitter : emitters){
            if(emitter.isAlive){
                return true;
            }
        }
        return false;
    }

    Vector3 GetCenter() const{
        if(parentMat){
            return ExtractTranslation(*parentMat) + offset;
        } else{
            return offset;
        }
    }

    void TeachParent(){
        for(auto& emitter : emitters){
            emitter.parentGroup = this;
        }
    }

public:
    std::string name = "";
    const Matrix4x4* parentMat = nullptr;
    std::list<Emitter> emitters;
    Vector3 offset;
};