#pragma once
#include "SEED.h"
#include <memory>

// 前方宣言
class Scene_Base;

// シーン管理クラス(シングルトン)
class SceneManager{

private:
    SceneManager();
public:
    ~SceneManager();
    static void Initialize();
    static void Update();
    static void Draw();
    static void BeginFrame();
    static void EndFrame();
    static SceneManager* GetInstance();

private:
    static SceneManager* instance_;
    SceneManager(const SceneManager& other) = delete;
    SceneManager& operator=(const SceneManager& other) = delete;

public:
    static void ChangeScene(Scene_Base* newScene);
    static void ChangeScene(std::unique_ptr<Scene_Base> newScene);
private:
    static std::unique_ptr<Scene_Base> pScene_;
};