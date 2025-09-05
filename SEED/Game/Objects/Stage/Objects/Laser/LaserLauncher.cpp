#include "LaserLauncher.h"

void LaserLauncher::Initialize() {


    // スプライトの初期化
    sprite_ = Sprite("DefaultAssets/white1x1.png");
    sprite_.anchorPoint = Vector2(0.5f);
}

void LaserLauncher::Update() {


    // オブジェクトの更新を行う
    switch (commonState_) {
    case StageObjectCommonState::None:
        break;
    case StageObjectCommonState::Hologram:

        sprite_.color = Vector4(1.0f, 0.0f, 0.0f, 1.0f);
        break;
    }
}

void LaserLauncher::Draw() {

    // スプライトの描画
    sprite_.Draw();
}


void LaserLauncher::SetEmitDirections(const std::unordered_map<Direction, int>& directionsCount) {

    emitDirectionsCount_ = directionsCount;
}

const std::unordered_map<LaserLauncher::Direction, int>& LaserLauncher::GetEmitDirections() const {
    // TODO: return ステートメントをここに挿入します
    return emitDirectionsCount_;
}
