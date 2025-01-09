#pragma once

#include "Base/ICharacterState.h"

/// <summary>
/// Player が Spawn するまでの状態
/// </summary>
class PlayerState_Spawn
    :public ICharacterState{
public:
    PlayerState_Spawn();
    PlayerState_Spawn(const std::string& stateName,BaseCharacter* player);
    virtual ~PlayerState_Spawn();

    void Initialize(const std::string& stateName,BaseCharacter* character);
    void Update()override;
    void Draw() override;
protected:
    virtual void ManageState()override;
private:
    Vector3 spawnPos_;
    float spawnTime_;
    float spawnTimeLimit_;
public:
    void SetSpawnPos(const Vector3& _spawnPos){ spawnPos_ = _spawnPos; }
    void SetSpawnTime(float _spawnTime){ spawnTime_ = _spawnTime; }
    void SetSpawnTimeLimit(float _spawnTimeLimit){ spawnTimeLimit_ = _spawnTimeLimit; }
    const Vector3& GetSpawnPos()const{ return spawnPos_; }
    float GetSpawnTime()const{ return spawnTime_; }
    float GetSpawnTimeLimit()const{ return spawnTimeLimit_; }
};