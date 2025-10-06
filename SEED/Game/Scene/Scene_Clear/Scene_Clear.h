#pragma once

//host
#include <SEED/Source/Basic/Scene/Scene_Base.h>

///stl
#include <memory>

///local
//light
#include <SEED/Lib/Structs/DirectionalLight.h>
//objects
#include <SEED/Lib/Structs/Model.h>
#include <SEED/Source/Basic/Object/GameObject2D.h>
#include <SEED/Lib/Structs/Timer.h>
#include <Game/Objects/Result/PlayResult.h>

class Scene_Clear
    : public Scene_Base{
public:
    Scene_Clear();
    ~Scene_Clear()override;
    void Initialize()override;
    void Update()override;
    void Draw()override;
    void BeginFrame()override;
    void EndFrame()override;

public:
    static void SetResult(const PlayResult& result){ playResult_ = result; }
    static const PlayResult& GetResult(){ return playResult_; }
    static void SetJacketPath(const std::string& path){ jacketPath_ = path; }
    static const std::string& GetJacketPath(){ return jacketPath_; }

private:
    void CheckStep();

private:
    static inline PlayResult playResult_{};
    static inline std::string jacketPath_ = "DefaultAssets/uvChecker.png";
    int32_t step_ = 0;
    int32_t kMaxStep_ = 4;// 最大ステップ数
    Timer stepTimer_ = Timer(0.7f);
    std::unique_ptr<GameObject2D> resultUpdater_ = nullptr;

    bool sceneChangeOrder = false; // シーン変更のオーダーが出ているかどうか
};