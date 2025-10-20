#pragma once
#include <SEED/Source/SEED.h>
#include <SEED/Source/Manager/ImGuiManager/ImGuiManager.h>

// lib
#include <vector>
#include <list>
#include <unordered_set>
#include <memory>
#include <string>
#include <d3d12.h>

class EmitterBase;

// パーティクルをプリセット化するための構造体
class EmitterGroupBase{
public:
    EmitterGroupBase();
    virtual ~EmitterGroupBase() = default;

public:
    // アクセッサ
    bool GetIsAlive() const;

public:
    // 編集,出力関数
    virtual void Edit();
    virtual void OutputGUI();
    nlohmann::json GetJson();
    void LoadFromJson(const nlohmann::json& j);

protected:
    // ヘルパー関数
    void ContextMenu();
    void Reactivation();
    void InitEmitters();
    void TeachParent();

protected:
    inline static int nextGroupID_ = 0; // グループIDのカウンター
    std::string idTag_;
    std::string name = "";
    std::vector<std::unique_ptr<EmitterBase>> emitters;
    std::string outputFileName_;
    float kReactiveTime_ = 3.0f; // 再復活までの時間
    float curReactiveTime_ = 0.0f; // 現在の再復活時間
    float initUpdateTime_ = 0.0f;// 初期化時にあらかじめ更新しておく時間
    std::unordered_map<std::string, ImTextureID> icons_;

#ifdef _DEBUG
    int32_t selectEmitterIdx_ = -1;
#endif // _DEBUG
};