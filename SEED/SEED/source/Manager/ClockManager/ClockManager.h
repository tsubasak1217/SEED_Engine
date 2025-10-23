#pragma once
#include <memory>
#include <unordered_map>
#include <string>
#include <chrono>

/// <summary>
/// ゲーム内時間を管理するクラス
/// </summary>
class ClockManager{

private:
    ClockManager();

public:
    ~ClockManager();
    static void Initialize();
    static void BeginFrame();
    static void EndFrame();
    static void Update();
    static ClockManager* GetInstance();

private:
    // インスタンス
    static ClockManager* instance_;

    // コピー禁止
    ClockManager(const ClockManager& other) = delete;
    ClockManager& operator=(const ClockManager& other) = delete;


private:
    std::unordered_map<std::string, float>timeCounts_;
    float deltaTime_;
    float totalTime_ = 0.0f;
    float timeRate_ = 1.0f;
    float fps_;
    bool isStop_ = false;
    bool isRestartNow_ = false;
    static const float kFrameTime_;


    std::chrono::steady_clock::time_point preTime_;
    std::chrono::steady_clock::time_point currentTime_;

public:
    static void AddNewCount(const std::string& name);
    static float DeltaTime(){ return instance_->deltaTime_; }
    static float TimeRate(){ return instance_->timeRate_; }
    static float TotalTime(){ return instance_->totalTime_; }
    static float FPS(){ return instance_->fps_; }
    static void Stop(){ instance_->isStop_ = true; }
    static void Restart(){
        instance_->isRestartNow_ = true;
        instance_->isStop_ = false;
    }
};