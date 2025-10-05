#include "ComboObject.h"
#include <Game/GameSystem.h>

ComboObject::ComboObject() = default;

void ComboObject::Initialize(){

    // テキストの初期化
    comboTextObj = GameSystem::GetScene()->GetGameObject2D("comboCount");
    if(comboTextObj){
        text = &comboTextObj->GetComponent<UIComponent>()->GetText(0);
        text->text = "%d";
        text->BindDatas({ comboCount });
    }

    // タイマーの初期化
    scalingTimer.Initialize(0.3f, 0.3f);
}

void ComboObject::Update(){

    // 未初期化なら初期化
    if(!comboTextObj){
        Initialize();
        if(!comboTextObj){
            return;
        }
    }

    // タイマーの更新
    scalingTimer.Update();

    // 時間に応じてスケーリング
    float ease = scalingTimer.GetEase(Easing::OutBack);
    text->transform.scale = Vector2(ease);
    //text->transform.rotate = 3.14f * (1.0f - scalingTimer.GetProgress());
}
