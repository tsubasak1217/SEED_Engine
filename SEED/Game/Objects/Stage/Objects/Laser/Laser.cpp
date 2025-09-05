#include "Laser.h"

void Laser::Initialize() {

    // スプライトの初期化
    sprite_ = Sprite("DefaultAssets/white1x1.png");
    sprite_.anchorPoint = Vector2(0.5f);
}

void Laser::Update() {


    // オブジェクトの更新を行う
    switch (commonState_) {
    case StageObjectCommonState::None:
        break;
    case StageObjectCommonState::Hologram:

        sprite_.color = Vector4(1.0f, 0.0f, 0.0f, 1.0f);
        break;
    }
}

void Laser::Draw() {

    // スプライトの描画
    sprite_.Draw();
}
