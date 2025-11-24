#pragma once
#include <SEED/Source/Basic/Components/IComponent.h>
#include <SEED/Lib/Structs/Timer.h>


/// <summary>
/// 寿命を設定するコンポーネント
/// </summary>
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

public:// アクセッサ
    void SetStayAlive(bool flag){ stayAlive_ = flag; }

private:
    Timer timer_ ;
    bool stayAlive_ = false;
};