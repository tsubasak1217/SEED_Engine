#pragma once
#include <SEED/Lib/Structs/Sprite.h>
#include <SEED/Lib/Structs/Range2D.h>
#include <SEED/Lib/Structs/Timer.h>

struct EffectInfo {
    Vector2 position; // エフェクトの位置
    Vector2 scale; // エフェクトのスケール
    Vector2 Velocity; // エフェクトの速度
    Timer lifetime; // エフェクトの寿命
    Vector4 color; // エフェクトの色
    bool isAlive_; // エフェクトが生きているかどうか
};

class PlayerDeadEffect {
public:

    PlayerDeadEffect() = default;
    ~PlayerDeadEffect() = default;

    void Initialize(const Vector2& playerPos);
    void Update(Timer& timer);
    void Draw(bool isDead);

    void Emit();

    void Edit(bool isDead);

private:

    std::vector<Sprite> particles_; // パーティクルのスプライト
    std::vector<EffectInfo> effects_; // エフェクト情報の配列
    const int kNumParticles_ = 20; // パーティクルの数
    Vector3 colorPattern_[3];

    Vector2 emitCenter_; // エフェクトの発生中心位置
    Range2D emitaria_;
    Timer emitTimer_;
    //
    Timer timer_;

    Vector2 particleSize_;
    float lifetime_ = 0.05f; // エフェクトの寿命

    int emitParcentMax = 70;
    int emitParcentMin = 0;
};