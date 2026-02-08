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
    // スコアテキストオブジェクトの読み込み
    SEED::Hierarchy* hierarchy = SEED::GameSystem::GetScene()->GetHierarchy();
    scoreTextObj = hierarchy->LoadObject2D("PlayScene/Score.prefab");

    // テキストの初期化
    if(scoreTextObj){
        scoreText = &scoreTextObj->GetComponent<SEED::UIComponent>(0)->GetText(0);
        scoreText->text = "%f";
        scoreText->BindDatas({ score });
    }
}
