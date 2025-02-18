#pragma once
#include <memory>
#include <SEED/Source/Manager/CollisionManager/CollisionManager.h>
#include <SEED/Source/Manager/CameraManager/CameraManager.h>
#include <Game/Scene/Base/Scene_Base.h>

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
    static void ChangeScene(const std::string& sceneName);

private:
    void Update();
    void Draw();
    void BeginFrame();
    void EndFrame();
    void DrawGUI();

public:
    static Scene_Base* GetScene(){ return instance_->pScene_.get(); }

private:
    static std::unique_ptr<GameSystem>instance_;
    std::unique_ptr<Scene_Base> pScene_;
    float resolutionRate_ = 1.0f;

};