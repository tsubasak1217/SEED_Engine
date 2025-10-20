#include "EmitterBase.h"
#include <SEED/Source/Manager/ClockManager/ClockManager.h>
#include <SEED/Source/Manager/TextureManager/TextureManager.h>
#include <SEED/Source/Manager/ParticleManager/CPUParticle/ParticleManager.h>


EmitterBase::EmitterBase(){
    colors.push_back(Color());
    totalTime = interval;
    // タグの生成
    CreateTag();
}

////////////////////////////////////////////////////////
// 更新関数
////////////////////////////////////////////////////////
void EmitterBase::Update(){

    if(!isAlive){
        return;
    }

    // 初期化時にあらかじめ指定時間分更新しておく場合の処理
    if(initUpdateTime_ > 0.0f){
        // 初期化時にあらかじめ指定時間分更新しておく
        while(initUpdateTime_ > 0.0f){
            totalTime += 1.0f/60.0f;
            if(totalTime >= interval){
                totalTime = 0.0f;
                emitCount++;
                // アクティブな時だけ発生依頼を出す
                if(isActive){
                    // ここでは直接出す
                    emitOrder = true;
                    ParticleManager::Emit(this);
                    emitOrder = false;
                }

                // 無限発生の場合はこのまま処理を続ける
                if(emitType == EmitType::kInfinite){
                    continue;
                }

                // カスタム発生 or 1回発生の場合は最大発生回数に達したら非アクティブにする
                if(emitCount >= kMaxEmitCount or emitType == EmitType::kOnce){
                    isAlive = false;
                    emitCount = 0;
                    break;
                }
            }
            initUpdateTime_ -= 1.0f / 60.0f;
        }
    
    } else{// 通常の更新処理

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
}


///////////////////////////////////////////////////////////
// タグの作成
////////////////////////////////////////////////////////////
void EmitterBase::CreateTag(){
    idTag_ = "##" + MyFunc::PtrToStr(this);
}