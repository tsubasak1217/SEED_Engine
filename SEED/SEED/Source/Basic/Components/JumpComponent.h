#pragma once
#include <SEED/Source/Basic/Components/IComponent.h>
#include <SEED/Source/Manager/InputManager/InputManager.h>

class JumpComponent : public IComponent{
public:
    JumpComponent(GameObject* pOwner, const std::string& tagName = "");
    ~JumpComponent() = default;
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
    // 入力を受け付けるボタン配列(keyboard)
    std::vector<uint8_t> jumpKeys_;

    // 入力を受け付けるボタン配列(pad)
    std::vector<PAD_BUTTON> jumpPadButtons_;

    // パラメーター
    float jumpSpeed_ = 8.0f; // 移動速度
    bool isJumping_ = false; // ジャンプ中かどうか
};