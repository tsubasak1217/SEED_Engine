#include "Goal.h"

//============================================================================
//	Goal classMethods
//============================================================================

void Goal::Initialize(const std::string& filename) {

    // スプライトの初期化
    sprite_ = Sprite(filename);
    // とりあえずブロックとサイズを合わせる
    sprite_.size = Vector2(32.0f, 32.0f);
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