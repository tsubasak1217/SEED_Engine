#include "StageBackDrawer.h"
#include <SEED/Lib/Functions/MyFunc/Easing.h>

void StageBackDrawer::Update(const Vector2& playerPos){
    // タイマーの更新
    if(isActive_){
        putTimer_.Update();
    } else{
        // タイマーを減らす
        putTimer_.Update(-2.0f);
    }

    if(putTimer_.GetProgress() == 0.0f) return;

    // 六角形の更新
    float borderT = 1.0f / (float)hexagons_.size();

    for(int i = 0; i < hexagons_.size(); ++i){

        bool onBorder = putTimer_.GetProgress() > i * borderT;

        for(int j = 0; j < hexagons_[i].size(); ++j){

            float t = std::clamp(hexagons_[i][j].time / HexagonInfo::kMaxTime, 0.0f, 1.0f);
            float easedT = EaseOutExpo(t);
            float distanceToPlayer = MyMath::Length(hexagons_[i][j].pos - playerPos);
            float effecctRange = 200.0f;
            float distanceRate = std::clamp(1.0f - (distanceToPlayer / effecctRange), 0.0f, 1.0f);
            hexagons_[i][j].radius = hexagonSize_ * 0.5f * easedT;
            hexagons_[i][j].angle = angle * easedT;
            hexagons_[i][j].color.x = defaultColor_.x - 0.8f * distanceRate;
            hexagons_[i][j].color.z = defaultColor_.z + 1.0f * distanceRate;
            hexagons_[i][j].color.w = defaultColor_.w + 0.35f * distanceRate;

            if(isActive_){
                if(onBorder){
                    hexagons_[i][j].time = std::clamp(
                        hexagons_[i][j].time + ClockManager::DeltaTime(),
                        0.0f,
                        HexagonInfo::kMaxTime
                    );
                }
            } else{
                hexagons_[i][j].time = std::clamp(
                    hexagons_[i][j].time - ClockManager::DeltaTime(),
                    0.0f,
                    HexagonInfo::kMaxTime
                );
            }
        }
    }

    playerPos;
}

void StageBackDrawer::Draw(){
    if(putTimer_.GetProgress() == 0.0f) return;
    // 六角形の描画
    for(auto& row : hexagons_){
        for(auto& hex : row){
            if(hex.time <= 0.0f) continue;
            SEED::DrawHexagonFrame(
                hex.pos,
                hex.radius,
                hex.angle,
                hex.fatness,
                hex.color,
                BlendMode::NORMAL,
                DrawLocation::Back,
                0
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


// 画面に敷き詰める六角形の情報を作成
void StageBackDrawer::CreateHexagonInfo(){

    hexagons_.clear();
    float radius = hexagonSize_ * 0.5f;
    float dx = std::sqrt(3.0f) * radius * 0.5f;
    float dy = 1.5f * radius;

    Vector2 initialPos = {
        borderLineX_ + (direction_ == LR::LEFT ? -radius : radius),
        displayRange_.min.y
    };

    Vector2 currentPos = initialPos;
    int row = 0;

    while(true){

        if(direction_ == LR::LEFT){
            if(currentPos.x < displayRange_.min.x){ break; }
        } else{
            if(currentPos.x > displayRange_.max.x){ break; }
        }

        // 要素の追加
        hexagons_.emplace_back();
        int col = 0;

        while(currentPos.y < displayRange_.max.y){

            HexagonInfo info;
            currentPos.y = initialPos.y + col * (radius * 3) + (row % 2) * dy;
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
