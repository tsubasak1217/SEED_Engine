#include "HexagonTransition.h"
#include <SEED/Lib/Functions/Easing.h>


void HexagonTransition::Update(){

    if(!isExitScene_){
        // 現在のシーン終了タイマー
        curSceneExitTimer_.Update();

        // 終わったら時間を再度抽選
        if(curSceneExitTimer_.IsFinishedNow()){
            for(auto& hex : hexagons_){
                hex.appearTime = MyFunc::Random(newSceneEnterTimer_.GetDuration() * 0.1f, newSceneEnterTimer_.GetDuration());
                hex.scalingTimer.ToEnd();
            }

            isExitScene_ = true;
        }

    } else{
        // 次のシーン開始タイマー
        newSceneEnterTimer_.Update();
    }
}

void HexagonTransition::Draw(){

    // 六角形の描画
    for(auto& hex : hexagons_){

        bool isDraw = false;
        float ease = 0.0f;
        if(!curSceneExitTimer_.IsFinished()){
            isDraw = hex.appearTime <= curSceneExitTimer_.currentTime;
            if(isDraw){
                hex.scalingTimer.Update();
                ease = hex.scalingTimer.GetEase(Easing::InOutQuint);
            }

        } else{
            ease = hex.scalingTimer.GetEase(Easing::InOutQuint);
            if(hex.appearTime <= newSceneEnterTimer_.currentTime){
                hex.scalingTimer.Update(-1.0f);
            }
            isDraw = ease > 0.0f;
        }

        if(isDraw){

           SEED::Instance::DrawHexagon(
                hex.pos,
                hexagonRadius_ * ease,
                angle_,
                hex.color,
                BlendMode::NORMAL,
                DrawLocation::Front,
                300,
                false,
                false
            );
        }
    }
}


void HexagonTransition::SetHexagonInfo(float hexagonRadius, const std::vector<Color>& colorList){
    // 六角形の半径を設定
    hexagonRadius_ = hexagonRadius;

    // 色のリストを設定
    colorList_ = colorList;

    if(colorList_.empty()){
        colorList_.emplace_back(0.0f, 0.0f, 0.0f, 1.0f);
    }
}


// 画面に敷き詰める六角形の情報を作成
void HexagonTransition::CreateHexagonInfo(){

    hexagons_.clear();
    float dx = std::sqrt(3.0f) * hexagonRadius_ * 0.5f;
    float dy = 1.5f * hexagonRadius_;

    Vector2 initialPos = { 0.0f,0.0f };

    Vector2 currentPos = initialPos;
    int row = 0;

    while(true){

        // x座標が範囲外なら終了
        if(currentPos.x > kWindowSize.x){ break; }

        // 要素の追加
        int col = 0;

        while(currentPos.y < kWindowSize.y){

            int random = MyFunc::Random(0, (int)colorList_.size() - 1);
            TransitionHexagon info;
            currentPos.y = initialPos.y + col * (hexagonRadius_ * 3) + (row % 2) * dy;
            currentPos.x = initialPos.x + row * dx;
            info.pos = currentPos;
            info.color = colorList_[random];
            info.appearTime = MyFunc::Random(0.0f, curSceneExitTimer_.GetDuration() * 0.9f);
            info.scalingTimer = Timer(scaleTime);
            hexagons_.emplace_back(info);
            col++;
        }

        currentPos.y = initialPos.y;// y座標を初期化
        row++;
    }
}
