#pragma once

#include "Base/ICharacterState.h"

#include <memory>

//object
class Egg;

/// <summary>
/// Player が Spawn するまでの状態
/// </summary>
class PlayerState_Spawn
    :public ICharacterState{
public:
    PlayerState_Spawn();
    PlayerState_Spawn(const std::string& stateName,BaseCharacter* player,Egg* _egg,bool _spawnCorpse = true);
    virtual ~PlayerState_Spawn();

    void Initialize(const std::string& stateName,BaseCharacter* character);
    void Update()override;
    void Draw() override;
protected:
    virtual void ManageState()override;

private:

private:
    Egg* egg_ = nullptr;

    bool movedGhost_ = false;

    Vector3 deadPos_;
    Vector3 spawnPos_;

    Vector3 beforePlayerScale_;

    std::unique_ptr<BaseObject> ghostObject_ = nullptr;

    float ghostMoveTime_ = 1.f;
    float elapsedTime_ = 0.0f;

    bool spawnCorpse_ = false;
    // 投げられた卵以外の卵があるか
    bool playerHasManyEggs_ = false;
public:
    void SetSpawnPos(const Vector3& _spawnPos){ spawnPos_ = _spawnPos; }
    const Vector3& GetSpawnPos()const{ return spawnPos_; }
};