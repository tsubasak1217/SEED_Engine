#pragma once
#include <vector>
#include <memory>
#include <numbers>
#include <json.hpp>
#include <SEED/Source/Manager/ImGuiManager/ImGuiManager.h>
#include <SEED/Lib/Tensor/Vector2.h>
#include <SEED/Lib/Tensor/Vector4.h>
#include <SEED/Lib/Structs/Range2D.h>
#include <SEED/Lib/Structs/Range1D.h>
#include <SEED/Lib/Structs/Timer.h>
#include <SEED/Lib/Structs/Transform.h>

struct Emitter2D{
    Vector2 position;
    Vector2 emitRange;
    Range1D rotateSpeedRange;
    Range1D speedRange;
    Range2D scaleRange;
    Range1D lifeTimeRange;
    Vector2 baseDirection;
    float directionRange;
    Timer emitTimer;
    int numEmitEvery;
    std::vector<Vector4> colorList;
};

struct Particle2D{
    Transform2D transform;
    Transform2D startTransform;
    Transform2D endTransform;
    Vector2 direction;
    float speed;
    float rotateSpeed;
    Vector4 color;
    Timer lifeTimer;
};


class ParticleMachine2D{
public:
    ParticleMachine2D() = default;
    ~ParticleMachine2D() = default;

    // 他に依存しないのでDrawの中で更新も行う
    void Draw();
    void Emit(const Emitter2D& emitter);

private:
    std::vector<std::unique_ptr<Emitter2D>> emitters_;
    std::vector<std::unique_ptr<Particle2D>> particles_;

public:
    nlohmann::json ToJson() const;
    void FromJson(const nlohmann::json& data);
    void Edit();
};