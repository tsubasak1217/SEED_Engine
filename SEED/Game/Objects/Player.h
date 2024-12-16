#pragma once
#include <list>
#include "SEED.h"
#include "Bullet.h"
#include "ColliderObject.h"

class Player{
public:
    Player();
    ~Player();

    void Update();
    void Draw();

private:

    void Shoot();
    void Move();

public:
    const std::list<ColliderObject>& GetColliders() const{ return colliders_; }
    int GetBulletSize() const{ return (int)bullets_.size(); }
    Bullet* GetBullet(int index){
        auto it = bullets_.begin();
        std::advance(it, index);
        return *it;
    }

private:

    // プレイヤーの変数
    Quad2D player_;
    Vector2 MpveDirection_;
    float speed_ = 10.0f;
    float radius_ = 10.0f;

    // 弾の変数
    float kCoolTime = 0.1f;
    float coolTime_ = 0.0f;
    std::list<Bullet*> bullets_;

    // コライダー
    std::list<ColliderObject> colliders_;
};