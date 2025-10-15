#pragma once
#include <SEED/Source/Basic/Components/IComponent.h>
#include <SEED/Lib/Structs/Timer.h>


class LifetimeComponent : public IComponent{
public:// 基礎関数
    LifetimeComponent(std::variant<GameObject*, GameObject2D*> pOwner, const std::string& tagName = "");
    ~LifetimeComponent() = default;
    void Initialize()override;
    void BeginFrame() override;
    void Update() override;
    void Draw() override;
    void EndFrame() override;
    void Finalize() override;

public:// 入出力
    // GUI編集
    void EditGUI() override;
    // json
    nlohmann::json GetJsonData() const override;
    void LoadFromJson(const nlohmann::json& jsonData) override;

private:
    Timer timer_ ;
    bool stayAlive_ = false;
};