#include "Goal.h"

//============================================================================
//	Goal classMethods
//============================================================================

void Goal::Initialize() {

    // スプライトの初期化
    sprite_ = Sprite("DefaultAssets/monsterBall.png");
    sprite_.anchorPoint = Vector2(0.5f);
}

void Goal::Update() {

    // オブジェクトの更新を行う
    switch (commonState_) {
    case StageObjectCommonState::None:
        break;
    case StageObjectCommonState::Hologram:

        sprite_.color = Vector4(1.0f, 0.0f, 0.0f, 1.0f);
        break;
    }
}

void Goal::Draw() {

    // スプライトの描画
    sprite_.Draw();
}