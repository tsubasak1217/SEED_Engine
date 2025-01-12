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
public:
    void SetSpawnPos(const Vector3& _spawnPos){ spawnPos_ = _spawnPos; }
    const Vector3& GetSpawnPos()const{ return spawnPos_; }
};