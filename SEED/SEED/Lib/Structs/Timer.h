#pragma once
#include <algorithm>
#include <SEED/Source/Manager/ClockManager/ClockManager.h>
#include <SEED/Lib/Functions/MyFunc/Easing.h>

struct Timer{
    Timer() = default;
    Timer(float _duration);

public:
    void Initialize(float _duration);
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
    void Update(float timeScale = 1.0f);

public:
    bool isStop = false;
    float currentTime = 0.0f;
    float prevTime = 0.0f;
    float duration = 0.0f;
};



// コンストラクタ
inline Timer::Timer(float _duration) {
    Initialize(_duration);
}

// 初期化
inline void Timer::Initialize(float _duration) {
    this->duration = _duration;
    currentTime = 0.0f;
    prevTime = 0.0f;
}

// 進捗を取得(0~1)
inline float Timer::GetProgress() const {
    return currentTime / duration;
}

// durationを取得
inline float Timer::GetDuration() const {
    return duration;
}

// 進捗度をイージングして返す
inline float Timer::GetEase(Easing::Type easeType) {
    return Easing::Ease[easeType](GetProgress());
}


inline float Timer::GetPrevProgress() const {
    return prevTime / duration;
}

// 完了しているかどうか
inline bool Timer::IsFinished() const {
    return currentTime >= duration;
}

// 今完了したばかりかどうか
inline bool Timer::IsFinishedNow() const {
    return prevTime < duration && currentTime >= duration;
}

// リセット
inline void Timer::Reset() {
    currentTime = 0.0f;
}

// 最後まで進める
inline void Timer::ToEnd() {
    currentTime = duration;
}

// 停止
inline void Timer::Stop() {
    isStop = true;
}

// 再開
inline void Timer::Restart() {
    isStop = false;
}

// 時間の更新
inline void Timer::Update(float timeScale) {
    prevTime = currentTime;
    currentTime = std::clamp(currentTime + ClockManager::DeltaTime() * timeScale * !isStop, 0.0f, duration);
}