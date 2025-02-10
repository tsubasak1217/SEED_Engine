#include <SEED/Source/Object/Particle/Emitter.h>
#include <SEED/Source/Manager/ClockManager/ClockManager.h>
#include <SEED/Source/Manager/TextureManager/TextureManager.h>

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
            if(!isEdittting){
                isAlive = false;
            } else{
                isActive = false;
                emitCount = 0;
            }
            return;
        }
    }
}

// 中心座標を取得
Vector3 Emitter::GetCenter() const{
    if(parentGroup){
        return parentGroup->GetCenter() + center;
    } else{
        return center;
    }
}
