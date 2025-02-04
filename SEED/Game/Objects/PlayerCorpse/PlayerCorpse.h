#pragma once

#include "Base/BaseObject.h"

class PlayerCorpseManager;

class PlayerCorpse
    : public BaseObject{
public:
    PlayerCorpse();
    ~PlayerCorpse();

    void Initialize() override;
    void Initialize(const Vector3& scale);
    void Update() override;

private:
    bool isAlive_ = true;

    //member
    PlayerCorpseManager* manager_ = nullptr;
public:
    //accessor
    void SetManager(PlayerCorpseManager* manager){ manager_ = manager; }
    PlayerCorpseManager* GetManager(){ return manager_; }

    bool GetIsAlive(){ return isAlive_; }
    void SetIsAlive(bool isAlive){ isAlive_ = isAlive; }
};
