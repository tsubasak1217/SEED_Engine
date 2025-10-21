#include "EmitterGroup2D.h"
#include <SEED/Source/Manager/ParticleManager/CPUParticle/Emitter/EmitterBase.h>
#include <SEED/Source/Manager/ClockManager/ClockManager.h>
#include <SEED/Source/Manager/TextureManager/TextureManager.h>

// Emitter
#include "Emitter2D.h"

// EmitterGroupのコンストラクタ
EmitterGroup2D::EmitterGroup2D() : EmitterGroupBase(){}

// 中心座標を取得
Vector2 EmitterGroup2D::GetPosition() const{
    if(parentMat){
        return ExtractTranslation(*parentMat) + offset;
    } else{
        return offset;
    }
}