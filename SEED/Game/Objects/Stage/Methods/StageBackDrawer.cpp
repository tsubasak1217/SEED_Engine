#include "StageBackDrawer.h"
#include <SEED/Lib/Functions/MyFunc/Easing.h>

void StageBackDrawer::Update(const Vector2& playerPos, float animTime) {
    // タイマーの更新
    if (isActive_) {
        putTimer_.Update();
    } else {
        // タイマーを減らす
        putTimer_.Update(-2.0f);
    }

    if (putTimer_.GetProgress() == 0.0f) return;

    // 六角形の更新
    float borderT = 1.0f / (float)hexagons_.size();

    // 放射状用の更新
    float maxDistance = 1.0f;
    if (animationMode_ == HexagonAnimaionMode::Radial) {

        maxDistance = 0.0001f;
        // 中心から一番遠い六角形の距離を取得する
        for (const auto& row : hexagons_) {
            for (const auto& hexagon : row) {

                float distance = MyMath::Length(hexagon.pos - playerPos);
                if (distance > maxDistance) {
                    maxDistance = distance;
                }
            }
        }
    }

    if (ambientEnabled_ && putTimer_.IsFinished()) {
        for (auto& spot : ambientSpots_) {

            spot.t += ClockManager::DeltaTime();
            if (spot.t >= spot.period) {

                spot.t = std::fmod(spot.t, spot.period);
                if (spot.relocate) {

                    spot.pos = RandomPointInArea(ambientArea_);
                }
            }
        }
    }

    for (int i = 0; i < hexagons_.size(); ++i) {
        for (int j = 0; j < hexagons_[i].size(); ++j) {

            bool shouldGrow = false;
            if (animationMode_ == HexagonAnimaionMode::Border) {

                shouldGrow = (putTimer_.GetProgress() > i * borderT);
            } else {

                float appearT = MyMath::Length(hexagons_[i][j].pos - playerPos) / maxDistance;
                shouldGrow = (putTimer_.GetProgress() > appearT);
            }

            float t = std::clamp(hexagons_[i][j].time / animTime, 0.0f, 1.0f);
            float easedT = EaseOutExpo(t);
            float distanceToPlayer = MyMath::Length(hexagons_[i][j].pos - playerPos);
            float effecctRange = 200.0f;
            float distanceRate = std::clamp(1.0f - (distanceToPlayer / effecctRange), 0.0f, 1.0f);
            hexagons_[i][j].radius = hexagonSize_ * 0.5f * easedT;
            hexagons_[i][j].angle = angle * easedT;
            hexagons_[i][j].color.x = defaultColor_.x - 0.8f * distanceRate;
            hexagons_[i][j].color.z = defaultColor_.z + 1.0f * distanceRate;
            hexagons_[i][j].color.w = defaultColor_.w + 0.35f * distanceRate;

            // 指定エリア以外をグラデーションする
            if (ambientEnabled_ && putTimer_.IsFinished()) {

                float extra = 0.0f;
                for (const auto& s : ambientSpots_) {

                    float d = MyMath::Length(hexagons_[i][j].pos - s.pos);
                    float prox = std::clamp(1.0f - (d / s.radius), 0.0f, 1.0f);
                    float lfo = 0.5f * (1.0f - std::cos((s.t / s.period) * 2.0f * std::numbers::pi_v<float>));
                    extra += prox * lfo;
                }
                hexagons_[i][j].color.z = std::clamp(hexagons_[i][j].color.z + ambientPurpleAmp_ * extra, 0.0f, 1.0f);
                hexagons_[i][j].color.w = std::clamp(hexagons_[i][j].color.w + ambientAlphaAmp_ * extra, 0.0f, 1.0f);
            }

            if (isActive_) {
                if (shouldGrow) {
                    hexagons_[i][j].time = std::clamp(
                        hexagons_[i][j].time + ClockManager::DeltaTime(),
                        0.0f,
                        animTime
                    );
                }
            } else {
                hexagons_[i][j].time = std::clamp(
                    hexagons_[i][j].time - ClockManager::DeltaTime(),
                    0.0f,
                    animTime
                );
            }
        }
    }

    playerPos;
}

void StageBackDrawer::Draw(bool isApplyViewMat) {
    if (putTimer_.GetProgress() == 0.0f) return;
    // 六角形の描画
    for (auto& row : hexagons_) {
        for (auto& hex : row) {
            if (hex.time <= 0.0f) continue;
            SEED::DrawHexagonFrame(
                hex.pos,
                hex.radius,
                hex.angle,
                hex.fatness,
                hex.color,
                BlendMode::NORMAL,
                DrawLocation::Back,
                0,
                isApplyViewMat
            );
        }
    }

#ifdef _DEBUG
    ImFunc::CustomBegin("color", MoveOnly_TitleBar);
    {
        ImGui::ColorEdit4("Default Color", &defaultColor_.x);
        ImGui::End();
    }
#endif // _DEBUG
}


void StageBackDrawer::EnableAmbientPurpleFade(int count, float radius, float period,
    const Range2D& area, bool relocateEachCycle, float purpleAmp, float alphaAmp) {

    ambientSpots_.clear();
    ambientSpots_.reserve((std::max)(0, count));
    ambientArea_ = area;
    ambientEnabled_ = true;
    ambientPurpleAmp_ = purpleAmp;
    ambientAlphaAmp_ = alphaAmp;

    for (int i = 0; i < count; ++i) {

        AmbientSpot spot;
        spot.pos = RandomPointInArea(area);
        spot.radius = radius;
        spot.period = (std::max)(0.1f, period);
        spot.t = 0.0f;
        spot.relocate = relocateEachCycle;
        ambientSpots_.push_back(spot);
    }
}

Vector2 StageBackDrawer::RandomPointInArea(const Range2D& area) {

    float rx = (float)std::rand() / (float)RAND_MAX;
    float ry = (float)std::rand() / (float)RAND_MAX;
    return { area.min.x + rx * (area.max.x - area.min.x),
        area.min.y + ry * (area.max.y - area.min.y) };
}

// 画面に敷き詰める六角形の情報を作成
void StageBackDrawer::CreateHexagonInfo() {

    hexagons_.clear();
    float radius = hexagonSize_ * 0.5f;
    float dx = std::sqrt(3.0f) * radius * 0.5f;
    float dy = 1.5f * radius;

    Vector2 initialPos = {
        borderLineX_ + (direction_ == LR::LEFT ? -radius : radius),
        displayRange_.max.y
    };

    Vector2 currentPos = initialPos;
    int row = 0;

    while (true) {

        if (direction_ == LR::LEFT) {
            if (currentPos.x < displayRange_.min.x) { break; }
        } else {
            if (currentPos.x > displayRange_.max.x) { break; }
        }

        // 要素の追加
        hexagons_.emplace_back();
        int col = 0;

        while (currentPos.y > displayRange_.min.y) {

            HexagonInfo info;
            currentPos.y = initialPos.y - col * (radius * 3) - (row % 2) * dy;
            currentPos.x = initialPos.x + row * dx * (direction_ == LR::LEFT ? -1.0f : 1.0f);
            info.pos = currentPos;
            info.radius = radius;
            info.color = defaultColor_;
            info.time = 0.0f;
            info.fatness = 0.1f;
            hexagons_.back().emplace_back(info);
            col++;
        }

        currentPos.y = initialPos.y;// y座標を初期化
        row++;
    }
}
