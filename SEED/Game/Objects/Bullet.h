#pragma once
#include <list>
#include "SEED.h"
#include "ColliderObject.h"

class Bullet{
public:
    Bullet();
    ~Bullet();

    void Update();
    void Draw();

    // コピームーブ許可
    Bullet(const Bullet& other) = default;
    Bullet& operator=(const Bullet& other) = default;
    Bullet(Bullet&& other) = default;
    Bullet& operator=(Bullet&& other) = default;

public:
    bool GetIsAlive() const{ return isAlive_; }
    void SetIsAlive(bool isAlive){ isAlive_ = isAlive; }
    void SetPosition(Vector2 position){ position_ = position; }
    void SetMoveDirection(Vector2 moveDirection){ moveDirection_ = moveDirection; }
    void SetSpeed(float speed){ speed_ = speed; }
    const std::list<ColliderObject>& GetColliders() const{ return colliders_; }

private:
    Quad2D bullet_;
    Vector2 position_;
    Vector2 moveDirection_;
    float speed_;
    float lifeTime_ = 3.0f;
    float radius_ = 10.0f;
    bool isAlive_ = true;

    // コライダー
    std::list<ColliderObject> colliders_;
};