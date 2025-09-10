#pragma once
#include <memory>
#include <functional>
#include <string>
#include <SEED/Source/SEED.h>
#include <SEED/Source/Basic/Scene/Scene_Base.h>
#include <Game/Objects/Select/Methods/SelectStageCreator.h>

// シーン管理クラス(シングルトン)
class SceneManager {

private:
    SceneManager();
public:
    ~SceneManager();
    static void Initialize();
    static SceneManager* GetInstance();

private:
    static std::unique_ptr<SceneManager>instance_;
    SceneManager(const SceneManager& other) = delete;
    SceneManager& operator=(const SceneManager& other) = delete;

public:// シーンの管理
    static void Register(const std::string& sceneName, std::function<Scene_Base* (void)> createFunc);
    static Scene_Base* CreateScene(const std::string sceneName);

    static SelectStageCreator* GetCreator() { return stageCreator_.get(); }
private:// メンバ変数

    std::unordered_map<std::string, std::function<Scene_Base* (void)>> sceneMap_;

    static inline std::unique_ptr<SelectStageCreator> stageCreator_ = nullptr;
};