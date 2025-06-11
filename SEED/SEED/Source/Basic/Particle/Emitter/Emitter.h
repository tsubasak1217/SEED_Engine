#pragma once
#include <SEED/Source/SEED.h>
#include <SEED/Lib/Structs/Range1D.h>
#include <SEED/Lib/Structs/Range2D.h>
#include <SEED/Lib/Structs/Range3D.h>
#include <SEED/Lib/Functions/MyFunc/MatrixFunc.h>
#include <SEED/Lib/Functions/MyFunc/MyMath.h>
#include <SEED/Lib/Functions/MyFunc/DxFunc.h>
#include <SEED/Lib/Functions/MyFunc/Easing.h>
#include <SEED/Lib/Structs/blendMode.h>
#include <SEED/Source/Manager/ImGuiManager/ImGuiManager.h>

// lib
#include <vector>
#include <list>
#include <unordered_set>
#include <memory>
#include <string>
#include <d3d12.h>

class EmitterGroup;

// パーティクルの種類
enum class ParticleType : int32_t{
    kRadial = 0,
    kPrimitive,
    kPrimitive2D
};

enum class EmitPtimitive{
    Quad,
    Quad2D,
    Triangle,
    Triangle2D,
    Line,
    Line2D,
    Ring,
    Ring2D,
    Cube,
    Cylinder,
};


// パーティクルの発生方法
enum class EmitType : int32_t{
    kOnce = 0,// 一度だけ発生
    kInfinite,// 無限に発生
    kCustom,// カスタム
};


// パーティクルを発生させるための構造体
class Emitter_Base{
    friend EmitterGroup;
    friend class EffectSystem;
public:
    Emitter_Base();
    virtual ~Emitter_Base() = default;
    Emitter_Base(const Emitter_Base& other) = default;
    Emitter_Base(Emitter_Base&& other) = default;
    Emitter_Base& operator=(const Emitter_Base& other) = default;
    Emitter_Base& operator=(Emitter_Base&& other) = default;

    // 更新処理
    void Update();

    // 編集処理
    virtual void Edit() = 0;
    virtual nlohmann::json ExportToJson() = 0;
    virtual void LoadFromJson(const nlohmann::json& j) = 0;

protected:
    virtual void DrawEditData() = 0;

public:
    Vector3 GetCenter();

    //---------------------- フラグ類 ----------------------//
public:
    bool isActive = true;// アクティブかどうか
    bool isAlive = true;// 生存しているかどうか
    bool emitOrder = false;// 発生命令が出されたかどうか
    bool isEdittting = false;// 編集中かどうか

    //-------------------- 発生パラメータ ------------------//
public:
    // parameter
    EmitterGroup* parentGroup = nullptr;// 親グループ
    ParticleType particleType = ParticleType::kRadial;// パーティクルの種類
    Vector3 center;// 中心座標
    Vector3 emitRange = { 10.0f,10.0f,10.0f };// 発生範囲
    float interval = 0.1f;// 発生間隔
    int32_t numEmitEvery = 1;// 一度に発生させる数

    // texture
    static inline std::unordered_map<std::string, ImTextureID> textureDict;// テクスチャの辞書
    std::vector<std::string> texturePaths;// テクスチャハンドルの一覧
    std::unordered_set<std::string> textureSet;// テクスチャのセット（重複を避けるため）

    // material
    std::vector<Vector4> colors;// 発生させる色の一覧
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
    inline static int nextEmitterID_ = 0; // エミッターIDのカウンター
    std::string idTag_;// IDタグ（エディター用）
};



// パーティクルをプリセット化するための構造体
class EmitterGroup{
    friend class EffectSystem;
public:
    EmitterGroup();
    EmitterGroup(const Matrix4x4* parentMat) : parentMat(parentMat){}

public:
    bool GetIsAlive() const{
        // エミッターが1つもない場合はtrueを返す
        if(emitters.size() == 0){ 
            return true;
        }
       
        if(isEditMode_){
            // 編集モードの場合は常にtrueを返す
            return true;
        }

        // 1つでも生存しているエミッターがあればtrueを返す
        for(const auto& emitter : emitters){
            if(emitter->isAlive){
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
            emitter->parentGroup = this;
        }
    }


    void Edit();
    void OutputGUI();
    void Output();

private:
    void Reactivation();

public:
    std::string name = "";
    const Matrix4x4* parentMat = nullptr;
    std::list<std::unique_ptr<Emitter_Base>> emitters;
    Vector3 offset;

private:
    char outputFileName_[64];
    inline static int nextGroupID_ = 0; // グループIDのカウンター
    std::string idTag_;
    static inline Emitter_Base* selectedEmitter_ = nullptr;
    static inline auto selectedItEmitter_ = std::list<std::unique_ptr<Emitter_Base>>::iterator();
    std::string selectedEmitterName_ = "";
    bool isEditMode_ = false; // 編集モードかどうか
    float kReactiveTime_ = 3.0f; // 再復活までの時間
    float curReactiveTime_ = 0.0f; // 現在の再復活時間
};