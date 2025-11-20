#include "Timer.h"

// コンストラクタ
Timer::Timer(float _duration, float current, bool defaultStop){
    Initialize(_duration, current,defaultStop);
}

// 初期化
void Timer::Initialize(float _duration, float current, bool defaultStop){
    this->duration = _duration;
    currentTime = current;
    prevTime = current;
    isStop = defaultStop;
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

// 残り時間を取得
float Timer::GetLeftTime() const{
    return duration - currentTime;
}

// 完了しているかどうか
bool Timer::IsFinished() const{
    return currentTime >= duration;
}

// 今完了したばかりかどうか
bool Timer::IsFinishedNow() const{
    return prevTime < duration && currentTime >= duration;
}

bool Timer::IsLoopedNow() const{
    return isLoopedNow;
}

// 0に戻ったか
bool Timer::IsReturnNow() const{
    return currentTime == 0.0f && prevTime > 0.0f;
}

// 指定時間に達した瞬間かどうか
bool Timer::GetTrigger(float triggerTime) const{
    return prevTime < triggerTime && currentTime >= triggerTime;
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
void Timer::Update(float timeScale,bool isLoop){
    // ループフラグのリセット
    isLoopedNow = false;
    // 時間の更新
    prevTime = currentTime;
    currentTime += ClockManager::DeltaTime() * timeScale * !isStop;

    // 超過時間の記録
    if(currentTime >= duration){
        overtime = currentTime - duration;

        // ループフラグがあればループ
        if(isLoop){
            currentTime = overtime;
            isLoopedNow = true;
        }

    } else if(currentTime <= 0.0f){// 逆再生時の処理
        overtime = currentTime;
        // ループフラグがあればループ
        if(isLoop){
            currentTime = duration + overtime;
        }

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
        } else{
            timer.prevTime = timer.duration;
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

float TimerArray::GetProgress(int32_t index) const{
    if(index < 0 || index >= static_cast<int32_t>(timers.size())){
        return 0.0f;
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
