#pragma once
#include <SEED/Source/Basic/Scene/Scene_Base.h>

// stl
#include <memory>
#include <vector>

// object
#include <SEED/Lib/Structs/Sprite.h>
#include <SEED/Lib/Structs/Model.h>
#include <SEED/Lib/Structs/VideoPlayer.h>

/// <summary>
/// タイトルシーン(まだ実装してない)
/// </summary>
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
    // 読み込むオブジェクト類
    GameObject2D* titleLogo_ = nullptr;
    GameObject2D* rotateRects_ = nullptr;
    GameObject* cameraParents_ = nullptr;
    GameObject* models_ = nullptr;
    GameObject* display_ = nullptr;

    // タイトル表示用タイマー
    Timer titleStartTimer_;
    float totalTime_ = 0.0f;
    float cameraParentTime_ = 3.0f;

    // Video
    std::unique_ptr<VideoPlayer> videoPlayer_;
};