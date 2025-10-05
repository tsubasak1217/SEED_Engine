#include "Timer.h"

// コンストラクタ
Timer::Timer(float _duration, float current){
    Initialize(_duration, current);
}

// 初期化
void Timer::Initialize(float _duration, float current){
    this->duration = _duration;
    currentTime = current;
    prevTime = current;
}

// 進捗を取得(0~1)
float Timer::GetProgress() const{
    if(duration == 0.0f){
        return 1.0f;
    }
    return currentTime / duration;
}

// durationを取得
float Timer::GetDuration() const{
    return duration;
}

// 進捗度をイージングして返す
float Timer::GetEase(Easing::Type easeType){
    return Easing::Ease[easeType](GetProgress());
}


float Timer::GetPrevProgress() const{
    return prevTime / duration;
}

// 完了しているかどうか
bool Timer::IsFinished() const{
    return currentTime >= duration;
}

// 今完了したばかりかどうか
bool Timer::IsFinishedNow() const{
    return prevTime < duration && currentTime >= duration;
}

// リセット
void Timer::Reset(){
    currentTime = 0.0f;
}

// 最後まで進める
void Timer::ToEnd(){
    currentTime = duration;
}

// 停止
void Timer::Stop(){
    isStop = true;
}

// 再開
void Timer::Restart(){
    isStop = false;
}

// 時間の更新
void Timer::Update(float timeScale){
    prevTime = currentTime;
    currentTime += ClockManager::DeltaTime() * timeScale * !isStop;

    // 超過時間の記録
    if(currentTime > duration){
        overtime = currentTime - duration;
    }

    // Clamp
    currentTime = std::clamp(currentTime, 0.0f, duration);
}


// TimerArray
TimerArray::TimerArray(std::initializer_list<float> timePoints){
    for(const auto& time : timePoints){
        timers.emplace_back(time);
    }
}

// 時間の更新
void TimerArray::Update(float timeScale){
    for(auto& timer : timers){
        if(!timer.IsFinished()){
            timer.Update(timeScale);
            break;
        }
    }
}

// タイマー番号を取得
int32_t TimerArray::GetCurrentIndex() const{
    for(size_t i = 0; i < timers.size(); ++i){
        if(!timers[i].IsFinished()){
            return static_cast<int32_t>(i);
        }
    }
    return static_cast<int32_t>(timers.size());
}

// 全て完了しているか
float TimerArray::GetProgress() const{
    int32_t index = GetCurrentIndex();
    if(index < 0 || index >= static_cast<int32_t>(timers.size())){
        return 1.0f;
    }
    return timers[index].GetProgress();
}

int32_t TimerArray::IsFinishedNow() const{
    for(size_t i = 0; i < timers.size(); ++i){
        if(timers[i].IsFinishedNow()){
            return static_cast<int32_t>(i);
        }
    }
    return -1;
}

bool TimerArray::IsFinished() const{
    for(const auto& timer : timers){
        if(!timer.IsFinished()){
            return false;
        }
    }
    return true;
}

bool TimerArray::IsAllFinishedNow() const{
    return timers.back().IsFinishedNow();
}
