#pragma once
#include <vector>
#include <numbers>
#include <SEED/Lib/Tensor/Vector2.h>
#include <SEED/Lib/Tensor/Vector4.h>
#include <SEED/Source/SEED.h>
#include <SEED/Lib/Structs/Timer.h>
#include <SEED/Lib/Structs/Range2D.h>

struct HexagonInfo {
    Vector2 pos;
    Vector4 color;
    float radius;
    float angle;
    float fatness = 0.2f;
    float time;
    static inline float kMaxTime = 0.25f;
};

// アニメーションの種類
enum class HexagonAnimaionMode {

    Border, // 帯状
    Radial, // 放射状
};

class StageBackDrawer {
public:
    StageBackDrawer() = default;
    ~StageBackDrawer() = default;
    void Update(const Vector2& playerPos, float animTime = HexagonInfo::kMaxTime);
    void Draw();

public:
    void SetActive(bool isActive) { isActive_ = isActive; }
    void SetBorder(float x, LR direction, const Range2D& range) {
        borderLineX_ = x;
        direction_ = direction;
        displayRange_ = range;
        putTimer_.Reset();
        CreateHexagonInfo();
    }
    void SetAnimaionMode(HexagonAnimaionMode mode) { animationMode_ = mode; }
    void SetDefaultColorAlpha(float alpha) { defaultColor_.w = alpha; }
private:
    void CreateHexagonInfo();

private:

    HexagonAnimaionMode animationMode_ = HexagonAnimaionMode::Border;
    bool isActive_ = false;
    std::vector<std::vector<HexagonInfo>> hexagons_;
    float hexagonSize_ = 80.0f;
    float angle = std::numbers::pi_v<float> / 2.0f;
    LR direction_;
    float borderLineX_;
    Timer putTimer_ = Timer(1.0f);
    Range2D displayRange_;
    Vector4 defaultColor_ = MyMath::FloatColor(255, 0, 71, 50, false);
};