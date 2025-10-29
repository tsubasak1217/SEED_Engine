#include "ComboObject.h"
#include <Game/GameSystem.h>

ComboObject::ComboObject() = default;

void ComboObject::Initialize(){

    // コンボテキストオブジェクトの読み込み
    Hierarchy* hierarchy = GameSystem::GetScene()->GetHierarchy();
    comboTextObj_ = hierarchy->LoadObject2D("PlayScene/ComboCount.prefab");

    // テキストの初期化
    if(comboTextObj_){
        text_ = &comboTextObj_->GetComponent<UIComponent>(0)->GetText(0);
        text_->text = "%d";
        text_->BindDatas({ comboCount_ });
    }

    // タイマーの初期化
    scalingTimer_.Initialize(0.3f, 0.3f);
}

void ComboObject::Update(){

    // 未初期化なら初期化
    if(!comboTextObj_){
        Initialize();
        if(!comboTextObj_){
            return;
        }
    }

    // タイマーの更新
    scalingTimer_.Update();

    // 時間に応じてスケーリング
    float ease = scalingTimer_.GetEase(Easing::OutBack);
    text_->transform.scale = Vector2(ease);
    //text->transform.rotate = 3.14f * (1.0f - scalingTimer.GetProgress());
}
