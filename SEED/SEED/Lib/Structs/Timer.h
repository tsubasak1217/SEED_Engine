#pragma once
#include <algorithm>
#include <SEED/Source/Manager/ClockManager/ClockManager.h>
#include <SEED/Lib/Functions/Easing.h>


/// <summary>
/// タイマー
/// </summary>
struct Timer{
    Timer() = default;
    Timer(float _duration,float current = 0.0f,bool defaultStop = false);

public:
    void Initialize(float _duration,float current = 0.0f, bool defaultStop = false);
    float GetProgress() const;
    float GetDuration() const;
    float GetEase(Easing::Type easeType);
    float GetPrevProgress() const;
    float GetLeftTime() const;
    bool IsFinished() const;
    bool IsFinishedNow() const;
    bool IsLoopedNow() const;
    bool IsReturnNow() const;
    bool GetTrigger(float triggerTime) const;
    void Reset();
    void ToEnd();
    void Stop();
    void Restart();
    void Update(float timeScale = 1.0f,bool isLoop = false);

public:
    bool isStop = false;
    bool isLoopedNow = false;
    float currentTime = 0.0f;
    float prevTime = 0.0f;
    float duration = 0.0f;
    float overtime = 0.0f;
};


/// <summary>
/// 複数のタイムポイントを保持できるタイマー
/// </summary>
struct TimerArray{
    TimerArray() = default;
    TimerArray(std::initializer_list<float> timePoints);
    void Update(float timeScale = 1.0f);
    int32_t GetCurrentIndex() const;
    int32_t IsFinishedNow() const;
    float GetProgress() const;
    float GetProgress(int32_t index) const;
    bool IsFinished() const;
    bool IsAllFinishedNow() const;
    std::vector<Timer> timers;
};


