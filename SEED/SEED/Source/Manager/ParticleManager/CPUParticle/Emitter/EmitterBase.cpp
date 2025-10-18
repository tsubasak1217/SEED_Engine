#include "EmitterBase.h"
#include <SEED/Source/Manager/ClockManager/ClockManager.h>
#include <SEED/Source/Manager/TextureManager/TextureManager.h>


EmitterBase::EmitterBase(){
    colors.push_back(Color());
    totalTime = interval;
    // タグの生成
    idTag_ = "##" + std::to_string(nextEmitterID_); // ユニークなIDタグを生成
    nextEmitterID_++;
}

////////////////////////////////////////////////////////
// 更新関数
////////////////////////////////////////////////////////
void EmitterBase::Update(){

    if(!isAlive){
        return;
    }

    totalTime += ClockManager::DeltaTime();

    if(totalTime >= interval){
        totalTime = 0.0f;
        emitCount++;

        // アクティブな時だけ発生依頼を出す
        if(isActive){
            emitOrder = true;
        }

        // 無限発生の場合はこのまま処理を続ける
        if(emitType == EmitType::kInfinite){
            return;
        }

        // カスタム発生 or 1回発生の場合は最大発生回数に達したら非アクティブにする
        if(emitCount >= kMaxEmitCount or emitType == EmitType::kOnce){

            isAlive = false;
            emitCount = 0;
            return;
        }
    }
}