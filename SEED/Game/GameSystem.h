#pragma once
#include <memory>
#include <SEED/Source/Manager/CollisionManager/CollisionManager.h>
#include <SEED/Source/Manager/CameraManager/CameraManager.h>
#include <SEED/Source/Basic/Scene/Scene_Base.h>
#include <Game/Objects/Select/Methods/StageAchievementCollector.h>
#include <Game/Objects/Select/Methods/SelectStageCreator.h>

class GameSystem{
private:
    GameSystem() = default;
    GameSystem(const GameSystem&) = delete;
    GameSystem& operator=(const GameSystem&) = delete;

public:
    ~GameSystem(){};
    static GameSystem* GetInstance();

public:
    static void Initialize();
    static void Finalize();
    static void Run();
    static void BeginFrame();
    static void EndFrame();
    static void ChangeScene(const std::string& sceneName);

private:
    void Update();
    void Draw();
    void DrawGUI();

public:
    static Scene_Base* GetScene(){ return instance_->pScene_.get(); }

    static StageProgressCollector* GetStageProgressCollector() { return instance_->stageProgressCollector_.get(); }

    static SelectStageCreator* GetCreator() { return instance_->stageCreator_.get(); }

private:
    static std::unique_ptr<GameSystem>instance_;
    std::unique_ptr<Scene_Base> pScene_;

    // ステージ達成度管理
    std::unique_ptr<StageProgressCollector> stageProgressCollector_;

    std::unique_ptr<SelectStageCreator> stageCreator_;
};