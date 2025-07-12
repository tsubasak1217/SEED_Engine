#pragma once
#include <SEED/Source/Basic/Components/IComponent.h>
#include <SEED/Source/Manager/InputManager/InputManager.h>

class MoveComponent : public IComponent{
public:
    MoveComponent(GameObject* pOwner, const std::string& tagName = "");
    ~MoveComponent() = default;
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

public:
    void SetCameraPtr(BaseCamera* pCamera){ pCamera_ = pCamera; }
    bool IsMoving() const{ return isMoving_; }

private:
    Vector3 direction_;
    float speed_ = 10.0f;
    BaseCamera* pCamera_;
    bool isMoving_ = false;
};