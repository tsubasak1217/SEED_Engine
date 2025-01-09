#pragma once
#include "BaseCamera.h"
#include "DebugCamera.h"
#include "StageViewCamera.h"

#include <memory>
#include <unordered_map>
#include <string>

class CameraManager {
private:
    CameraManager() = default;

public:
    ~CameraManager();
    static void Initialize();
    static void Update();
    static CameraManager* GetInstance();

private:
    // インスタンス
    static CameraManager* instance_;

    // コピー禁止
    CameraManager(const CameraManager& other) = delete;
    CameraManager& operator=(const CameraManager& other) = delete;

public:

    // カメラのポインタ取得
    static BaseCamera* GetCamera(const std::string& name);
    static void AddCamera(const std::string& name, BaseCamera* camera);
    static void DeleteCamera(const std::string& name);
    static BaseCamera* GetActiveCamera();

    // アクティブなカメラを設定
    static void SetActiveCamera(const std::string& name);


private:

    // カメラの一覧
    std::unordered_map<std::string, BaseCamera*>cameras_;

    //stageを見渡すためのカメラ
    std::unique_ptr<StageViewCamera> stageViewCamera_;
    // メインのカメラ
    std::unique_ptr<BaseCamera> mainCamera_;
    std::unique_ptr<DebugCamera> debugCamera_;

    // アクティブなカメラを保持するポインタ
    BaseCamera* activeCamera_ = nullptr;
};