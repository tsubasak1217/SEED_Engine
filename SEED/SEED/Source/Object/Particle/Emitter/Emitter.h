#pragma once
#include <SEED/Lib/Structs/Range1D.h>
#include <SEED/Lib/Structs/Range3D.h>
#include <SEED/Lib/Tensor/Vector3.h>
#include <SEED/Lib/Tensor/Vector4.h>
#include <SEED/Lib/Tensor/Matrix4x4.h>
#include <SEED/Lib/Functions/MyFunc/MatrixFunc.h>
#include <SEED/Lib/Functions/MyFunc/MyMath.h>
#include <SEED/Lib/Functions/MyFunc/DxFunc.h>
#include <SEED/Lib/Structs/blendMode.h>
#include <SEED/Source/Manager/ImGuiManager/ImGuiManager.h>

// lib
#include <vector>
#include <list>
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

    // material
    std::vector<Vector4> colors;// 発生させる色の一覧
    BlendMode blendMode = BlendMode::ADD;// ブレンドモード

    //-------------------- 管理用パラメータ ------------------//
public:// アクティブ・非アクティブ管理のための変数
    EmitType emitType = EmitType::kInfinite;// 発生方法
    int32_t kMaxEmitCount = 1;// 最大発生回数(EmitType::kCustomの時用)

protected:
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

public:
    std::string name = "";
    const Matrix4x4* parentMat = nullptr;
    std::list<Emitter_Base*> emitters;
    Vector3 offset;

private:
    char outputFileName_[64];
};