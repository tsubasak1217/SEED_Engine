#include "BlockNormal.h"

//============================================================================
//	BlockNormal classMethods
//============================================================================

void BlockNormal::Initialize(const std::string& filename) {

    // スプライトの初期化
    sprite_ = Sprite(filename);
}

void BlockNormal::Update() {

    // オブジェクトの更新を行う
    switch (commonState_) {
    case StageObjectCommonState::None:
        break;
    case StageObjectCommonState::Hologram:

        sprite_.color = Vector4(0.0f, 0.0f, 1.0f, 1.0f);
        break;
    }
}

void BlockNormal::Draw() {

    // スプライトの描画
    sprite_.Draw();
}