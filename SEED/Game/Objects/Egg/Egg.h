#pragma once


#include "../Base/BaseCharacter.h"

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

    void OnCollision(const BaseObject* other,ObjectType objectType) override;
public:// ステートから呼び出す関数
    void Break();
private:
    EggManager* eggManager_ = nullptr;

    BaseObject* player_ = nullptr;

    bool isBreak_ = false;

    float weight_ = 1.0f;
public:// アクセッサ
    void SetPlayer(BaseObject* _player){ player_ = _player; }
    void SetEggManager(EggManager* _eggManager){ eggManager_ = _eggManager; }

    float GetWeight()const{ return weight_; }

    bool GetIsBreak()const{ return isBreak_; }
};