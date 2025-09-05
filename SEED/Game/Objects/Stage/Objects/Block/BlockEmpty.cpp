#include "BlockEmpty.h"

void BlockEmpty::Initialize() {
    // スプライトの初期化
    sprite_ = Sprite("Scene_Game/StageObject/dottedLine.png");
    sprite_.anchorPoint = Vector2(0.5f);
}

void BlockEmpty::Update() {

    // オブジェクトの更新を行う
    switch (commonState_) {
    case StageObjectCommonState::None:
        break;
    case StageObjectCommonState::Hologram:

        sprite_.color = Vector4(0.0f, 0.0f, 1.0f, 1.0f);
        break;
    }
}

void BlockEmpty::Draw() {

    // スプライトの描画
    sprite_.Draw();
}
