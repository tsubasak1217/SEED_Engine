#pragma once

#include "../GameObject/GameObject.h"

#include "Vector3.h"

////////////////////////////////////////////////////
//  Player が 扱う 卵
////////////////////////////////////////////////////

//           産む
// Playerが  投げる  Object
//           食べる

class Egg
    : public GameObject{
public:
    Egg();
    ~Egg();

    void Initialize()override;
    void Update()override;
private:
    Vector3 velocity_ = {0.0f,0.0f,0.0f};

    const float maxLifeTime_ = 3.0f;
    float lifeTime_ = 0.0f;

    const float maxSpeed_ = 10.0f;

    /// <summary>
    /// 投げられたか
    /// </summary>
    bool isThrowed_ = false;
public:
    /// <summary>
    /// 速度 の ゲッター
    /// </summary>
    /// <returns></returns>
    const Vector3& GetVelocity()const{ return velocity_; }

    /// <summary>
    /// 速度の セッター
    /// </summary>
    /// <param name="_velocity">新しい 速度</param>
    /// <returns></returns>
    void  SetVelocity(const Vector3& _velocity){ velocity_ = _velocity; }

    /// <summary>
    /// 投げる
    /// </summary>
    void ThrowThis(const Vector3& _velocity);

    void SetIsThrow(bool _isThrowed){ isThrowed_  = _isThrowed; }
    bool GetThrow()const{ return isThrowed_; }
};