#include "ParticleManager.h"

ParticleManager* ParticleManager::instance_ = nullptr;


/// <summary>
/// 
/// </summary>
ParticleManager::~ParticleManager(){
    delete instance_;
    instance_ = nullptr;
}


/// <summary>
/// 
/// </summary>
/// <returns></returns>
ParticleManager* ParticleManager::GetInstance(){
    if(!instance_){
        instance_ = new ParticleManager();
    }
    return instance_;
}

