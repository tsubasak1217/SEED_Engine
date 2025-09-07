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

    // EmitState
    {
        // 星
        isStarGrowing_ = true;
        centerStarTimer_.duration = 0.64f;
        centerStarTimer_.Reset();

        // ユニット
        commonUnit_.spriteCount = 8;
        commonUnit_.duration = 1.6f;
        commonUnit_.phaseSpacing = 0.4f;
        commonUnit_.moveDistance = 16.0f;
        commonUnit_.gapFromBlock = 1.2f;
        commonUnit_.startScale = 0.8f;
        commonUnit_.targetScale = 0.0f;
        commonUnit_.scalineEasing = Easing::InCubic;
        commonUnit_.translateEasing = Easing::InSine;
    }
}

void LaserLauncher::InitializeLaunchSprites() {

    // 全てクリア
    units_.clear();

    // 方向を反映（
    for (const auto& direction : launchDirections_) {

        // 状態に応じた向きを取得
        DIRECTION4 stateDirection = LaserHelper::GetStateDirection(commonState_, direction);

        LaserUnit unit = commonUnit_;
        unit.direction = stateDirection;

        // ユニットの見た目生成
        unit.laserUnitSprite.clear();
        unit.timers.clear();
        unit.laserUnitSprite.resize(unit.spriteCount);
        unit.timers.resize(unit.spriteCount);
        for (int i = 0; i < unit.spriteCount; ++i) {

            // スプライトを初期化
            Sprite sprite("Scene_Game/StageObject/laserUnit.png");
            sprite.anchorPoint = 0.5f;
            sprite.rotate = LaserHelper::GetRotateFromDirection(stateDirection);
            // 発射台の色に合わせる
            sprite.color = frameSprite_.color;

            // 一定間隔でずらす
            Timer timer;
            timer.duration = unit.duration;
            float phase = std::fmod(unit.phaseSpacing * i, 1.0f);
            timer.currentTime = timer.duration * phase;
            timer.prevTime = timer.currentTime;

            // 配列に追加
            unit.laserUnitSprite[i] = sprite;
            unit.timers[i] = timer;
        }
        units_.push_back(std::move(unit));
    }
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

    // 中心の星の更新
    if (commonState_ == StageObjectCommonState::Hologram) {

        UpdateCenterStar();
    }

    // ユニットの更新処理
    UpdateUnits();
}

void LaserLauncher::UpdateCenterStar() {

    // 時間経過で大きくなったり小さくしたりする
    // 時間を進める
    centerStarTimer_.Update();

    // 拡大中か縮小中かでイージングと補間の向きを切替え
    float t = isStarGrowing_ ?
        centerStarTimer_.GetEase(Easing::OutSine) : centerStarTimer_.GetEase(Easing::InSine);

    // スケーリング
    const float scaleMin = 0.8f;
    const float scaleMax = 1.2f;
    float scale = isStarGrowing_ ? std::lerp(scaleMin, scaleMax, t) : std::lerp(scaleMax, scaleMin, t);
    centerStarSprite_.scale = Vector2(scale);

    // 反転してループさせる
    if (centerStarTimer_.IsFinishedNow()) {

        isStarGrowing_ = !isStarGrowing_;
        centerStarTimer_.Reset();
    }
}

void LaserLauncher::UpdateUnits() {

    // 空、サイズ以上の場合は処理しない
    if (units_.empty()) {
        return;
    }

    // 方向ごとに1本だけ更新
    for (auto& unit : units_) {

        switch (unit.currentState) {
        case State::Emit: {

            // 発生中の処理
            UpdateEmit(unit);
            break;
        }
        case State::EmitNotPossible: {

            // 発生できないときの処理
            UpdateEmitNotPossible(unit);
            break;
        }
        }
    }
}

void LaserLauncher::UpdateEmit(LaserUnit& unit) {

    // 発生起点座標
    const Vector2 origin = LaserHelper::GetTranslatedByDirection(
        unit.direction, translate_, laserSize_, unit.gapFromBlock);

    // 進行方向の軸と目標点
    const Vector2 axis = LaserHelper::GetAxisFromDirection(unit.direction);
    const Vector2 target = origin + axis * unit.moveDistance;

    // スプライトを更新
    for (size_t i = 0; i < unit.laserUnitSprite.size(); ++i) {

        // 発生できない場合は状態を変える
        if (!IsDirectionReadyForUnit(unit)) {
            
            // 表示を消す
            for (auto& sprite : unit.laserUnitSprite) {

                sprite.scale = Vector2(0.0f);
            }
            unit.currentState = State::EmitNotPossible;
            continue;
        }

        Timer& timer = unit.timers[i];
        Sprite& sprite = unit.laserUnitSprite[i];

        // 時間を進める
        timer.Update();

        // 座標補間
        float tEasedT = timer.GetEase(unit.translateEasing);
        sprite.translate = MyMath::Lerp(origin, target, tEasedT);

        // スケール補間
        float sEasedT = timer.GetEase(unit.scalineEasing);
        sprite.scale = std::lerp(unit.startScale, unit.targetScale, sEasedT);

        // 完了したらリスタート
        if (timer.IsFinishedNow()) {

            timer.Reset();
        }
    }
}

void LaserLauncher::UpdateEmitNotPossible(LaserUnit& unit) {

    // 再起動されたらリセットしてもう一度発生
    if (IsDirectionReadyForUnit(unit)) {

        InitializeLaunchSprites();
        unit.currentState = State::Emit;
    }
}

bool LaserLauncher::IsDirectionReadyForUnit(const LaserUnit& unit) const {

    for (const auto& laserObject : lasers_) {

        LaserObjectComponent* component = laserObject->GetComponent<LaserObjectComponent>();
        if (Laser* laser = component->GetLaserObject<Laser>()) {

            // このユニットの向きと一致するレーザーをチェック
            if (laser->GetDirection() == unit.direction) {
                return !laser->IsStopAll();
            }
        }
    }
    return false;
}

void LaserLauncher::Draw() {

    // レーザーの描画
    for (const auto& laser : std::views::join(std::array{ lasers_, warpedLasers_ })) {

        laser->GetComponent<LaserObjectComponent>()->GetLaserObject<Laser>()->Draw();
    }

    // ユニット描画
    for (auto& unit : units_) {
        for (auto& sprite : unit.laserUnitSprite) {

            sprite.Draw();
        }
    }

    // フレーム
    frameSprite_.Draw();

    // 星
    centerStarSprite_.Draw();
}

void LaserLauncher::Edit() {

    if (ImGui::Button("ReStart Param")) {

        InitializeLaunchSprites();
    }

    ImGui::DragInt("spriteCount", &commonUnit_.spriteCount, 1, 1, 128);
    ImGui::DragFloat("duration", &commonUnit_.duration, 0.01f);
    ImGui::DragFloat("phaseSpacing", &commonUnit_.phaseSpacing, 0.01f);
    ImGui::DragFloat("moveDistance", &commonUnit_.moveDistance, 0.01f);
    ImGui::DragFloat("gapFromBlock", &commonUnit_.gapFromBlock, 0.01f);
    ImGui::DragFloat("startScale", &commonUnit_.startScale, 0.01f);
    ImGui::DragFloat("targetScale", &commonUnit_.targetScale, 0.01f);

    EnumAdapter<Easing::Type>::Combo("scalineEasing", &commonUnit_.scalineEasing);
    EnumAdapter<Easing::Type>::Combo("translateEasing", &commonUnit_.translateEasing);
}