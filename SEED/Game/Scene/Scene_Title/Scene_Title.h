#pragma once
#include <SEED/Source/Basic/Scene/Scene_Base.h>
#include <SEED/Lib/Structs/TextBox.h>
#include <memory>

class Scene_Title : public SEED::Scene_Base {
public:
    Scene_Title();
    ~Scene_Title() override;

    void Initialize() override;
    void Update() override;
    void Draw() override;
    void Finalize() override;

private:
    std::unique_ptr<SEED::TextBox2D> titleText_;
    std::unique_ptr<SEED::TextBox2D> infoText_;
};
