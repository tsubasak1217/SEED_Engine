#pragma once
#include <unordered_map>
#include <string>
#include <functional>
#include <optional>
#include <Scene_Base.h>
class SceneManager;

class SceneFactory {
public:
    SceneFactory();

public:// 登録・作成
    void Register(const std::string& sceneName, std::function<Scene_Base* (void)> createFunc);
    Scene_Base* Create(const std::string& sceneName);

public:// アクセッサ
    static void SetStartupSceneName(const std::string& sceneName) { startupSceneName_ = sceneName; }
    static const std::string& GetStartupSceneName() { return startupSceneName_.value(); }

private:// メンバ変数
    static std::optional<std::string> startupSceneName_;
    static std::unordered_map < std::string, std::function<Scene_Base* (void)>> sceneMap_;
};