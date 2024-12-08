#include "Emitter.h"
#include "ClockManager.h"
#include "TextureManager.h"

Emitter::Emitter(){
    colors.push_back(Vector4(1.0f, 1.0f, 1.0f, 1.0f));
    texturePaths.push_back(std::string("ParticleTextures/particle.png"));
    totalTime = interval;
}

void Emitter::Update(){

    if(!isActive){ return; }

    totalTime += ClockManager::DeltaTime();

    if(totalTime >= interval){
        totalTime = 0.0f;
        emitOrder = true;
        emitCount++;

        // 無限発生の場合はこのまま処理を続ける
        if(emitType == EmitType::kInfinite){
            return;
        }

        // カスタム発生 or 1回発生の場合は最大発生回数に達したら非アクティブにする
        if(emitCount >= kMaxEmitCount or emitType == EmitType::kOnce){
            isAlive = false;
            return;
        }
    }
}
