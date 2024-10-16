#pragma once
#include "Camera.h"
#include "DebugCamera.h"
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
    static Camera* GetCamera(const std::string& name);
    static void AddCamera(const std::string& name, Camera* camera);

private:

    // カメラの一覧
    std::unordered_map<std::string, Camera*>cameras_;

    // メインのカメラ
    std::unique_ptr<Camera> mainCamera_;
    std::unique_ptr<DebugCamera> debugCamera_;
};