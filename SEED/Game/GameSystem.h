#pragma once
#include <memory>
#include <SEED/Source/Manager/CollisionManager/CollisionManager.h>
#include <SEED/Source/Manager/CameraManager/CameraManager.h>
#include <SEED/Source/Basic/Scene/Scene_Base.h>

/// <summary>
/// ゲームのシーンの所有者
/// </summary>
class GameSystem{
private:
    GameSystem() = default;
    GameSystem(const GameSystem&) = delete;
    GameSystem& operator=(const GameSystem&) = delete;

public:
    ~GameSystem(){};
    static GameSystem* GetInstance();

public:// 外から触れる基本関数
    static void Initialize();
    static void Finalize();
    static void Run();
    static void BeginFrame();
    static void EndFrame();
    static void ChangeScene(const std::string& sceneName);

private:// 内部の基本関数
    void Update();
    void Draw();
    void DrawGUI();

public:
    static Scene_Base* GetScene(){ return instance_->pScene_.get(); }

private:
    static std::unique_ptr<GameSystem>instance_;
    std::unique_ptr<Scene_Base> pScene_;
};