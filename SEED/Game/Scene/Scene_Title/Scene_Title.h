#pragma once
#include <SEED/Source/Basic/Scene/Scene_Base.h>

// stl
#include <memory>
#include <unordered_map>
#include <string>

// object
#include <SEED/Lib/Structs/Sprite.h>
#include <SEED/Lib/Structs/Model.h>
#include <SEED/Lib/Structs/Timer.h>

// audio
#include <Game/Manager/AudioDictionary.h>

// manager
#include <SEED/Source/Manager/SceneTransitionDrawer/SceneTransitionDrawer.h>
#include <SEED/Source/Basic/SceneTransition/HexagonTransition.h>

class Scene_Title : public Scene_Base{
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
    void UpdateTitleLogo();
    void UpdateButtonSprites();
    void UpdateAudioVolume();

private:
    std::unordered_map<std::string, Sprite> sprites_;
    Vector2 basePos_ = { 640.0f,360.0f };

    // タイトルロゴ用の変数
    float logoTimer_ = 0.0f;
    Vector2 titleOffset_;

    // scene
    Timer transisitionTimer_ = Timer(1.0f);
    Timer sceneStartTimer_ = Timer(1.0f);
    bool isExitScene_ = false;

    // Text
    TextBox2D uiText_;

    // audio;
    AudioHandle bgmHandle_;
    float kBgmVolume_ = 0.2f;

    //hexagon
    std::vector<Vector4> hexagonColors_;
    float hexagonSize_ = 80.0f;


};