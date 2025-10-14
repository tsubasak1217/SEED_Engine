#pragma once
#include <algorithm>
#include <SEED/Source/Manager/ClockManager/ClockManager.h>
#include <SEED/Lib/Functions/MyFunc/Easing.h>

struct Timer{
    Timer() = default;
    Timer(float _duration,float current = 0.0f);

public:
    void Initialize(float _duration,float current = 0.0f);
    float GetProgress() const;
    float GetDuration() const;
    float GetEase(Easing::Type easeType);
    float GetPrevProgress() const;
    bool IsFinished() const;
    bool IsFinishedNow() const;
    void Reset();
    void ToEnd();
    void Stop();
    void Restart();
    void Update(float timeScale = 1.0f,bool isLoop = false);

public:
    bool isStop = false;
    float currentTime = 0.0f;
    float prevTime = 0.0f;
    float duration = 0.0f;
    float overtime = 0.0f;
};


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


