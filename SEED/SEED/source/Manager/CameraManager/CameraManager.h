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
    static Camera* GetCamera(std::string name);

private:

    // カメラの一覧
    std::unordered_map<std::string, std::unique_ptr<Camera>>cameras_;
};