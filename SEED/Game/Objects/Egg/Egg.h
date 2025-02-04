#pragma once


#include "../Base/BaseCharacter.h"
#include "Shadow/Shadow.h"

#include "Vector3.h"

class EggManager;

////////////////////////////////////////////////////
//  Player が 扱う 卵
////////////////////////////////////////////////////

//           産む
// Playerが  投げる  Object
//           食べる

class Egg
    : public BaseCharacter{
public:
    Egg(BaseObject* _player);
    ~Egg();

    void Initialize()override;
    void Update()override;
    void Draw()override;

    void OnCollision(const BaseObject* other,ObjectType objectType) override;
public:// ステートから呼び出す関数
    void SpawnPlayer();
private:
private:
    // 管理主
    EggManager* eggManager_ = nullptr;
    // 追従先
    BaseObject* player_ = nullptr;

    std::unique_ptr<Shadow> shadow_ = nullptr;

    // 速度ベクトル (thrown された時に使用)
    Vector3 velocity_;

    bool isBreak_ = false;
    bool isThrown_ = false;
public:// アクセッサ
    void SetPlayer(BaseObject* _player){ player_ = _player; }

    EggManager* GetEggManager()const{ return eggManager_; }
    void SetEggManager(EggManager* _eggManager){ eggManager_ = _eggManager; }

    float GetWeight()const{ return weight_; }

    bool GetIsThrown()const{ return isThrown_; }
    void SetIsThrown(bool _isThrown){ isThrown_ = _isThrown; }

    bool GetIsBreak()const{ return isBreak_; }
    void Break(){ isBreak_ = true; }

    const Vector3& GetVelocity()const{ return velocity_; }
    void SetVelocity(const Vector3& _velocity){ velocity_ = _velocity; }

};