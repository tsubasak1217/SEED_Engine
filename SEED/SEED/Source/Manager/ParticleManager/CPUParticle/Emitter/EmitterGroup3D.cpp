#include "EmitterGroup3D.h"
#include <SEED/Source/Manager/ParticleManager/CPUParticle/Emitter/EmitterBase.h>
#include <SEED/Source/Manager/ClockManager/ClockManager.h>
#include <SEED/Source/Manager/TextureManager/TextureManager.h>

// Emitter
#include "Emitter3D.h"

// EmitterGroupのコンストラクタ
EmitterGroup3D::EmitterGroup3D() : EmitterGroupBase(){}

// 中心座標を取得
Vector3 EmitterGroup3D::GetPosition() const{
    if(parentMat){
        return ExtractTranslation(*parentMat) + offset;
    } else{
        return offset;
    }
}