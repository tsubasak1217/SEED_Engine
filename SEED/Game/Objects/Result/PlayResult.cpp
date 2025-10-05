#include "PlayResult.h"

void PlayResult::ScoreTextUpdate(){
    // 未初期化なら初期化
    if(!scoreTextObj){
        Initialize();
        if(!scoreTextObj){
            return;
        }
    }
}

void PlayResult::Initialize(){
    // テキストの初期化
    scoreTextObj = GameSystem::GetScene()->GetGameObject2D("score");
    if(scoreTextObj){
        scoreText = &scoreTextObj->GetComponent<UIComponent>()->GetText(0);
        scoreText->text = "%f";
        scoreText->BindDatas({ score });
    }
}
