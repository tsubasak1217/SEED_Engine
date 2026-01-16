#pragma once
#include <SEED/Source/Basic/Scene/Scene_Base.h>

// stl
#include <memory>
#include <vector>

// object
#include <SEED/Lib/Structs/Sprite.h>
#include <SEED/Lib/Structs/Model.h>
#include <SEED/Lib/Structs/VideoPlayer.h>
#include <SEED/Source/Manager/InputManager/InputHandler.h>

/// <summary>
/// タイトルシーン(まだ実装してない)
/// </summary>
class Scene_Title : public SEED::Scene_Base{
public:
    Scene_Title();
    ~Scene_Title() override;

public:
    void Initialize() override;
    void Update() override;
    void Draw() override;
    void Finalize() override;
    void BeginFrame() override;
    void EndFrame() override;
    void HandOverColliders() override;

private:

};