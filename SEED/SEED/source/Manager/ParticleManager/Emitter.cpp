#include "Emitter.h"
#include "ClockManager.h"

void Emitter::Update(){
    static float time = interval;
    time += ClockManager::DeltaTime();

    if(time >= interval){
        time = 0.0f;
        emitOrder = true;
        emitCount++;

        // 無限発生の場合はこのまま処理を続ける
        if(emitType == EmitType::kInfinite){
            return;
        }

        // カスタム発生 or 1回発生の場合は最大発生回数に達したら非アクティブにする
        if(emitCount >= kMaxEmitCount or emitType == EmitType::kOnce){
            isActive = false;
            return;
        }
    }
}
