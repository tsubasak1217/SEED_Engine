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
#include <SEED/Lib/Structs/Curve.h>
#include <SEED/Lib/enums/ColorMode.h>
#include <SEED/Lib/Structs/Color.h>

// lib
#include <vector>
#include <list>
#include <unordered_set>
#include <memory>
#include <string>
#include <d3d12.h>
#include <json.hpp>


/// <summary>
// パーティクルを発生させるエミッターの基底クラス
/// </summary>
class EmitterBase{
    friend class EmitterGroupBase;
public:// コンストラクタ・デストラクタ・コピーコンストラクタ・代入演算子
    EmitterBase();
    virtual ~EmitterBase() = default;
    EmitterBase(const EmitterBase& other) = default;
    EmitterBase(EmitterBase&& other) = default;
    EmitterBase& operator=(const EmitterBase& other) = default;
    EmitterBase& operator=(EmitterBase&& other) = default;

public:// 基本関数---------------------------------------------
    void Update();

protected:// 内部関数------------------------------------------
    void CreateTag();
    
#ifdef _DEBUG// 編集処理---------------------------------------
public:
    virtual void Edit() = 0;
protected:
    void EditColors();
    void EditGeneral();
    void EditFrequency();
#endif // _DEBUG

public:// json入出力 -----------------------------------------
    virtual nlohmann::json ExportToJson() = 0;
    virtual void LoadFromJson(const nlohmann::json& j) = 0;

    // フラグ類 ----------------------------------------------
public:
    bool isActive_ = true;// アクティブかどうか
    bool isAlive_ = true;// 生存しているかどうか
    bool emitOrder_ = false;// 発生命令が出されたかどうか
    bool isUseGravity_ = false;// 重力を有効にするかどうか
    bool isUseRotate_ = false;// 回転処理を行うかどうか
    bool isRoteteRandomInit_ = false;// 回転初期価値をランダムにするか
    bool isSetGoalPosition_ = false;// ゴール位置を設定するかどうか
#ifdef _DEBUG
    bool useGuizmo_ = true;
#endif // _DEBUG

    // 発生パラメータ -----------------------------------------
public:
    // parameter
    EmitterGroupBase* parentGroup_ = nullptr;// 親グループ
    float interval_ = 0.1f;// 発生間隔
    int32_t numEmitEvery_ = 1;// 一度に発生させる数
    Range1D lifeTimeRange_ = { 1.0f,3.0f };// 寿命時間の幅
    float gravity_ = 0.0f;// 重力
    float initUpdateTime_ = 0.0f;// 初期化時にあらかじめ更新しておく時間

    // texture
    static inline std::unordered_map<std::string, ImTextureID> textureDict_;// テクスチャの辞書
    std::vector<std::string> texturePaths_; // 使用するテクスチャのパス一覧

    // material
    std::vector<Color> colors_;// 発生させる色の一覧
    BlendMode blendMode_ = BlendMode::ADD;// ブレンドモード

    // カーブ
    Curve scaleCurve_;
    Curve velocityCurve_;// 速度カーブ
    Curve rotateCurve_;// 回転カーブ
    Curve colorCurve_;// 色カーブ
    Curve positionInterpolationCurve_;// 位置補間カーブ

    // 色操作のモード
    ColorMode colorMode_ = ColorMode::RGBA;// カラーモード

    // 管理用パラメータ -----------------------------------------
public:// アクティブ・非アクティブ管理のための変数
    EmitType emitType_ = EmitType::kInfinite;// 発生方法
    int32_t kMaxEmitCount_ = 5;// 最大発生回数(EmitType::kCustomの時用)

protected:
    float totalTime_ = 0.0f;// 経過時間
    int32_t emitCount_ = 0;// 発生させた回数
    std::string idTag_;// IDタグ
};