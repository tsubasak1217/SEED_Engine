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

    // ファイル名を保持
    fileName_ = "Scene_Game/StageObject/laserLauncherForward.png";
}

void LaserLauncher::SetTranslate(const Vector2& translate) {

    translate_ = translate;

    // 全てのスプライトに対して処理を行う
    for (auto& sprite : launchSprites_) {

        sprite.translate = translate;
    }
}

void LaserLauncher::SetSize(const Vector2& size) {

    // 発射台と同じサイズ
    laserSize_ = size;

    // 全てのスプライトに対して処理を行う
    for (auto& sprite : launchSprites_) {

        sprite.size = size;
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

    // 方向別で初期化
    for (const auto& direction : launchDirections_) {

        // ホログラムかどうかで向きを決定する
        DIRECTION4 stateDirection = LaserHelper::GetStateDirection(commonState_, direction);

        // スプライトを初期化
        Sprite& sprite = launchSprites_.emplace_back(Sprite(fileName_));
        sprite.anchorPoint = Vector2(0.5f);
        sprite.rotate = LaserHelper::GetRotateFromDirection(stateDirection);
    }
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

void LaserLauncher::SetIsActive(bool isActive) {

    // レーザーにアクティブを設定する
    for (const auto& laser : std::views::join(std::array{ lasers_, warpedLasers_ })) {

        laser->SetIsActive(isActive);
    }
}

void LaserLauncher::Update() {
}

void LaserLauncher::Draw() {

    // スプライトの描画
    for (auto& launchSprite : launchSprites_) {

        launchSprite.Draw();
    }
}

void LaserLauncher::Edit() {
}