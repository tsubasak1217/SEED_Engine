#pragma once

#include "Base/BaseObject.h"

class PlayerCorpseManager;

class PlayerCorpse
    : public BaseObject{
public:
    PlayerCorpse();
    ~PlayerCorpse();

    void Initialize() override;
    void Update() override;

private:
private:
    //member
    PlayerCorpseManager* manager_ = nullptr;
public:
    //accessor
    void SetManager(PlayerCorpseManager* manager){ manager_ = manager; }
    PlayerCorpseManager* GetManager(){ return manager_; }
};
