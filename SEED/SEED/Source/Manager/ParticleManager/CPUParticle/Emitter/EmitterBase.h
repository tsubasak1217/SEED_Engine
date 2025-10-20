#pragma once

// local
#include <SEED/Source/Manager/ParticleManager/CPUParticle/Emitter/EmitterContext.h>
#include <SEED/Source/Manager/ImGuiManager/ImGuiManager.h>
#include <SEED/Lib/Structs/Range1D.h>
#include <SEED/Lib/Structs/Range2D.h>
#include <SEED/Lib/Structs/Range3D.h>
#include <SEED/Lib/Functions/MatrixFunc.h>
#include <SEED/Lib/Functions/MyMath.h>
#include <SEED/Lib/Functions/DxFunc.h>
#include <SEED/Lib/Functions/Easing.h>
#include <SEED/Lib/Structs/Model.h>


// lib
#include <vector>
#include <list>
#include <unordered_set>
#include <memory>
#include <string>
#include <d3d12.h>
#include <json.hpp>


// パーティクルを発生させるための構造体
class EmitterBase{
    friend class EmitterGroupBase;
public:
    EmitterBase();
    virtual ~EmitterBase() = default;
    EmitterBase(const EmitterBase& other) = default;
    EmitterBase(EmitterBase&& other) = default;
    EmitterBase& operator=(const EmitterBase& other) = default;
    EmitterBase& operator=(EmitterBase&& other) = default;

    // 更新処理
    void Update();

    // 編集処理
    virtual void Edit() = 0;
    virtual nlohmann::json ExportToJson() = 0;
    virtual void LoadFromJson(const nlohmann::json& j) = 0;

protected:
    virtual void DrawEditData() = 0;
    void CreateTag();

    //---------------------- フラグ類 ----------------------//
public:
    bool isActive = true;// アクティブかどうか
    bool isAlive = true;// 生存しているかどうか
    bool emitOrder = false;// 発生命令が出されたかどうか
#ifdef _DEBUG
    bool useGuizmo_ = true;
#endif // _DEBUG

    //-------------------- 発生パラメータ ------------------//
public:
    // parameter
    EmitterGroupBase* parentGroup = nullptr;// 親グループ
    float interval = 0.1f;// 発生間隔
    int32_t numEmitEvery = 1;// 一度に発生させる数
    float initUpdateTime_ = 0.0f;// 初期化時にあらかじめ更新しておく時間

    // texture
    static inline std::unordered_map<std::string, ImTextureID> textureDict;// テクスチャの辞書
    std::vector<std::string> texturePaths;// 使用するテクスチャのパス一覧

    // material
    std::vector<Color> colors;// 発生させる色の一覧
    BlendMode blendMode = BlendMode::ADD;// ブレンドモード

    // 発生・消滅の動きに関わるパラメータ
    float maxTimePoint = 0.5f;// 中心(scale,Alphaともに最大になる場所)の時間(0.0f~1.0f/lifeTime)
    float maxTimeRate = 0.2f;// 最大になっている時間の割合
    Vector3 kInScale = { 1.0f,1.0f,1.0f };// 出現時のスケール
    Vector3 kOutScale = { 0.0f,0.0f,0.0f };// 消失時のスケール
    float kInAlpha = 0.0f;// 出現時のアルファ値
    float kOutAlpha = 0.0f;// 消失時のアルファ値


    // ease関数
    Easing::Type velocityEaseType_ = Easing::Type::None;
    Easing::Type rotateEaseType_ = Easing::Type::None;
    Easing::Type enterEaseType_ = Easing::Type::None;
    Easing::Type exitEaseType_ = Easing::Type::None;

    //-------------------- 管理用パラメータ ------------------//
public:// アクティブ・非アクティブ管理のための変数
    EmitType emitType = EmitType::kInfinite;// 発生方法
    int32_t kMaxEmitCount = 5;// 最大発生回数(EmitType::kCustomの時用)

protected:
    float totalTime;// 経過時間
    int32_t emitCount = 0;// 発生させた回数
    std::string idTag_;// IDタグ（エディター用）
};