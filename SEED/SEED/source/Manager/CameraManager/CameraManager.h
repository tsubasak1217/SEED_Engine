#pragma once
#include <SEED/Source/Object/Camera/BaseCamera.h>
#include <SEED/Source/Object/Camera/DebugCamera.h>

#include <memory>
#include <unordered_map>
#include <string>

class CameraManager{
    friend class SEED;
    friend class DxManager;
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

private:

    // カメラのポインタ取得
    static BaseCamera* GetCamera(const std::string& name);
    static void RegisterCamera(const std::string& name, BaseCamera* camera);
    static void RemoveCamera(const std::string& name);

    // アクティブなカメラを設定
    static void SetIsCameraActive(const std::string& name,bool isActive);


private:

    // カメラの一覧
    std::unordered_map<std::string, BaseCamera*>cameras_;

};