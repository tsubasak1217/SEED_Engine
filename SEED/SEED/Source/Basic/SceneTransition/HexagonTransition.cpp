#include "HexagonTransition.h"
#include <SEED/Lib/Functions/MyFunc/Easing.h>


void HexagonTransition::Update(){

    sceneOutTimer_.Update();

    // 時間を彩度抽選
    if(sceneOutTimer_.IsFinished()){
        if(sceneOutTimer_.IsFinishedNow()){
            for(auto& row : hexagons_){
                for(auto& hex : row){
                    hex.time = MyFunc::Random(sceneInTimer_.GetDuration() * 0.1f, sceneInTimer_.GetDuration());
                }
            }
        }

        // 次のシーン開始時のタイマー
        sceneInTimer_.Update();
    }

#ifdef _DEBUG
    ImFunc::CustomBegin("HexagonTransition", MoveOnly_TitleBar);
    {
        ImGui::Text("end %d", sceneOutTimer_.IsFinished());
        ImGui::End();
    }
#endif // _DEBUG
}

void HexagonTransition::Draw(){

    // 六角形の描画
    for(auto& row : hexagons_){
        for(auto& hex : row){

            bool isDraw = false;
            if(!sceneOutTimer_.IsFinished()){
                isDraw = hex.time <= sceneOutTimer_.currentTime;
            } else{
                isDraw = hex.time >= sceneInTimer_.currentTime;
            }

            if(isDraw){
                SEED::DrawHexagon(
                    hex.pos,
                    hexagonRadius_,
                    angle_,
                    hex.color,
                    BlendMode::NORMAL,
                    DrawLocation::Front,
                    300,
                    false
                );
            }
        }
    }
}


void HexagonTransition::SetHexagonInfo(float hexagonRadius, const std::vector<Vector4>& colorList){
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
        hexagons_.emplace_back();
        int col = 0;

        while(currentPos.y < kWindowSize.y){

            int random = MyFunc::Random(0, (int)colorList_.size() - 1);
            TransitionHexagon info;
            currentPos.y = initialPos.y + col * (hexagonRadius_ * 3) + (row % 2) * dy;
            currentPos.x = initialPos.x + row * dx;
            info.pos = currentPos;
            info.color = colorList_[random];
            info.time = MyFunc::Random(0.0f, sceneOutTimer_.GetDuration() * 0.9f);
            hexagons_.back().emplace_back(info);
            col++;
        }

        currentPos.y = initialPos.y;// y座標を初期化
        row++;
    }
}
