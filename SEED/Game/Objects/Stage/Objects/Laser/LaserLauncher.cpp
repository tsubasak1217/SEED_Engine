#include "LaserLauncher.h"

//============================================================================
//	include
//============================================================================
#include <SEED/Lib/MagicEnumAdapter/EnumAdapter.h>
#include <Game/Objects/Stage/Objects/Laser/Methods/LaserBuilder.h>
#include <Game/Objects/Stage/Objects/Laser/Methods/LaserHelper.h>
#include <Game/Components/LaserObjectComponent.h>

//============================================================================
//	LaserLauncher classMethods
//============================================================================

LaserLauncher::~LaserLauncher() {

    // レーザーをすべて破棄
    for (auto& laser : lasers_) {

        delete laser;
        laser = nullptr;
    }
    lasers_.clear();
    for (auto& laser : warpedLasers_) {

        delete laser;
        laser = nullptr;
    }
    warpedLasers_.clear();
}

void LaserLauncher::Initialize() {

    // スプライトの初期化
    // フレーム
    frameSprite_ = Sprite("Scene_Game/StageObject/normalBlock.png");
    frameSprite_.anchorPoint = 0.5f;
    // 星
    centerStarSprite_ = Sprite("Scene_Game/StageObject/laserLauncherStar.png");
    centerStarSprite_.anchorPoint = 0.5f;
}

void LaserLauncher::SetTranslate(const Vector2& translate) {

    // 座標の保持
    translate_ = translate;

    // 全てのスプライトに対して処理を行う
    for (Sprite& sprite : std::array{ std::ref(frameSprite_), std::ref(centerStarSprite_) }) {

        sprite.translate = translate;
    }
}

void LaserLauncher::SetSize(const Vector2& size) {

    // 発射台と同じサイズ
    laserSize_ = size;

    // 全てのスプライトに対して処理を行う
    frameSprite_.size = size;
    centerStarSprite_.size = size;
}

void LaserLauncher::SetCommonState(StageObjectCommonState state) {

    // 状態に応じて色を設定
    commonState_ = state;
    switch (state) {
    case StageObjectCommonState::None:

        frameSprite_.color = normalColor_;
        centerStarSprite_.color = normalColor_;
        break;
    case StageObjectCommonState::Hologram:

        frameSprite_.color = hologramColor_;
        centerStarSprite_.color = hologramColor_;
        break;
    }
}

void LaserLauncher::SetLaunchDirections(uint8_t directions) {

    // 値を保持
    bitDirection_ = directions;

    // ビット値で方向を追加
    launchDirections_.clear();
    launchDirections_ = LaserHelper::GetLaserDirections(directions);
}

void LaserLauncher::SetIsLaserActive(bool isActive) {

    // レーザーにアクティブを設定する
    for (const auto& laser : std::views::join(std::array{ lasers_, warpedLasers_ })) {

        laser->SetIsActive(isActive);
    }
}

void LaserLauncher::InitializeLaunchSprites() {
}

void LaserLauncher::InitializeLasers() {

    // レーザーのインスタンスを作成する
    LaserBuilder laserBuilder{};
    lasers_ = laserBuilder.CreateLasersFromDirection(launchDirections_, commonState_,
        GetOwner()->GetWorldTranslate(), laserSize_);
    // コライダーの登録
    laserBuilder.CreateLaserColliders(lasers_);
}

void LaserLauncher::WarpLaserFromController(
    const Vector2& translate, const GameObject2D* sourceLaserObject) {

    // 対象のレーザーをワープの場所に複製する
    for (const auto& laserObject : lasers_) {
        if (laserObject == sourceLaserObject) {

            LaserBuilder laserBuilder{};
            warpedLasers_.push_back(laserBuilder.CopyLaser(translate, laserObject));
            break;
        }
    }
}

void LaserLauncher::RemoveWarpLasers() {

    for (auto& laser : warpedLasers_) {

        delete laser;
        laser = nullptr;
    }
    warpedLasers_.clear();
}

void LaserLauncher::Update() {

}

void LaserLauncher::Draw() {

    // レーザーの描画
    for (const auto& laser : std::views::join(std::array{ lasers_, warpedLasers_ })) {

        laser->GetComponent<LaserObjectComponent>()->GetLaserObject<Laser>()->Draw();
    }

    // フレーム
    frameSprite_.Draw();

    // 星
    centerStarSprite_.Draw();
}

void LaserLauncher::Edit() {
}