#pragma once
#include <SEED/Source/Basic/Components/IComponent.h>
#include <SEED/Source/Manager/InputManager/InputManager.h>
#include <SEED/Lib/Structs/Transform.h>
#include <list>

class RoutineComponent : public IComponent{
public:
    RoutineComponent(GameObject* pOwner, const std::string& tagName = "");
    ~RoutineComponent() = default;
    void Initialize()override;
    void BeginFrame() override;
    void Update() override;
    void Draw() override;
    void EndFrame() override;
    void Finalize() override;
    // GUI編集
    void EditGUI() override;
    // json
    nlohmann::json GetJsonData() const override;
    void LoadFromJson(const nlohmann::json& jsonData) override;

private:
    std::list<Transform> points_; // ルーチンのポイント
    bool isActive_ = true; // ルーチンが有効かどうか
    float duration_ = 1.0f; // ポイント間の移動時間
    float currentTime_ = 0.0f; // 現在の時間
    float speedRate_ = 1.0f; // 移動の倍率
    bool isLoop_ = true; // ループするかどうか
    bool isConnectLoop_ = false; // ループの接続を行うかどうか
    bool isRotateByDirection_ = true; // 方向に応じて回転するかどうか
};