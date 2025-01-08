#pragma once

#include "../Base/BaseCharacter.h"

#include "Vector3.h"

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
private:
    /// <summary>
    /// 投げられたか
    /// </summary>
    bool isThrowed_ = false;

    BaseObject* player_ = nullptr;
public:
    /// <summary>
    /// 投げる
    /// </summary>
    void ThrowThis();
    bool GetThrow()const{ return isThrowed_; }
};