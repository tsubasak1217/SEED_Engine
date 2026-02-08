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

private:// 内部関数
    void SelectTutorial();

private:
    // 読み込むオブジェクト類
    SEED::GameObject2D* titleLogo_ = nullptr;
    SEED::GameObject2D* pressSpace_ = nullptr;
    SEED::GameObject2D* rotateRects_ = nullptr;
    SEED::GameObject2D* tutorialSelectItems_ = nullptr;
    SEED::GameObject2D* mouseCursor_ = nullptr;
    SEED::GameObject* cameraParents_ = nullptr;
    SEED::GameObject* toSelectCamerawork_ = nullptr;
    SEED::GameObject* models_ = nullptr;
    SEED::GameObject* display_ = nullptr;

    // タイマー類
    SEED::Timer titleStartTimer_;
    SEED::Timer titleEndTimer_;
    SEED::Timer sceneOutTimer_;
    float totalTime_ = 0.0f;
    float cameraParentTime_ = 3.0f;

    // Video
    std::unique_ptr<SEED::VideoPlayer> videoPlayer_;

    // 入力格納用
    SEED::InputHandler<bool> decideButtonInput_;
    SEED::InputHandler<bool> tutorialSelectInput_;

    // BGM
    AudioHandle bgmHandle_;

    // チュートリアル選択用
    bool isPlayTutorial_ = true;

    // その他メンバー変数
    int32_t prevCameraParentIdx_;
    bool isStartButtonPressed_ = false;
    bool sceneChangeOrder_ = false;
};