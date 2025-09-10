#include "Player.h"

//============================================================================
//	include
//============================================================================
#include <SEED/Lib/MagicEnumAdapter/EnumAdapter.h>
#include <SEED/Lib/JsonAdapter/JsonAdapter.h>
#include <SEED/Source/SEED.h>

// inputDevice
#include <Game/Objects/Stage/Objects/Player/Input/Device/PlayerKeyInput.h>
#include <Game/Objects/Stage/Objects/Player/Input/Device/PlayerGamePadInput.h>

// manager
#include <SEED/Source/Manager/TextureManager/TextureManager.h>
#include <SEED/Source/Manager/ImGuiManager/ImGuiManager.h>

// func
#include <SEED/Lib/Functions/MyFunc/Easing.h>

//============================================================================
//	Player classMethods
//============================================================================


//////////////////////////////////////////////////////////////////
//
// 初期化処理
//
//////////////////////////////////////////////////////////////////
void Player::Initialize() {

    // 画像ハンドルの初期化
    static bool isFirstInitialize = true;
    if (isFirstInitialize) {
        imageMap_["Body"] = "Scene_Game/StageObject/Player/PlayerBody.png";
        imageMap_["Body_Hologram"] = "Scene_Game/StageObject/Player/PlayerBody_Hologram.png";
        imageMap_["Leg"] = "Scene_Game/StageObject/Player/PlayerLeg.png";
        imageMap_["Leg_Hologram"] = "Scene_Game/StageObject/Player/PlayerLeg_Hologram.png";
        isFirstInitialize = false;
    }

    // 入力クラスを初期化
    inputMapper_ = std::make_unique<InputMapper<PlayerInputAction>>();
    inputMapper_->AddDevice(std::make_unique<PlayerKeyInput>());     // キー操作
    inputMapper_->AddDevice(std::make_unique<PlayerGamePadInput>()); // パッド操作

    // 状態を初期化
    stateController_ = std::make_unique<PlayerStateController>();
    stateController_->Initialize(inputMapper_.get());

    // 胴体のスプライトを初期化
    body_ = Sprite(imageMap_["Body"]);
    body_.anchorPoint = Vector2(0.5f);
    body_.layer = baseLayer_;

    // 足のスプライトを初期化
    for (int i = 0; i < 2; i++) {
        legs_[i] = Sprite(imageMap_["Leg"]);
        legs_[i].anchorPoint = Vector2(0.5f, 0.5f);
        legs_[i].layer = baseLayer_ + (i == 0 ? 1 : -1);
        legs_[i].uvTransform.scale = { i == 0 ? 1.0f : -1.0f, 1.0f };
    }

    // json適応
    ApplyJson();

    // 初期化値
    // 最初は右向き
    moveDirection_ = LR::RIGHT;
    prevDirection_ = LR::NONE;

    //ownerのOnGround判定をtrueにする
    owner_->SetIsOnGround(true);
}

// spriteのサイズ設定
void Player::SetSize(const Vector2& size) {
    body_.size = size;
    legs_[0].size = size;
    legs_[1].size = size;
}

void Player::SetWarpState(const Vector2& start, const Vector2& target) {

    // 状態管理クラスに通知
    stateController_->SetWarpState(start, target);
}

bool Player::IsDeadFinishTrigger() const {
    // 死亡状態かどうか
    return stateController_->IsDeadFinishTrigger();
}

bool Player::IsOutOfCamera(const Range2D& cameraRange) const {
    bool isOut = false;

    if (cameraRange.min.x > body_.transform.translate.x + body_.size.x * 1.5f ||
        cameraRange.max.x < body_.transform.translate.x - body_.size.x * 1.5f ||
        cameraRange.max.y < body_.transform.translate.y - body_.size.y * 1.5f) {
        isOut = true;
    }
    return isOut;
}

void Player::SetDeadState() {

    stateController_->SetDeadState();
}

bool Player::IsFinishedWarp() const {

    // ワープ状態かどうか
    return stateController_->IsFinishedWarp();
}

bool Player::IsPutBorder() const {

    // 境界線を操作できる状態かチェック
    if (!stateController_->IsCanOperateBorder()) {
        return false;
    }
    return inputMapper_->IsTriggered(PlayerInputAction::PutBorder);
}

bool Player::IsRemoveBorder() const {

    // 境界線を操作できる状態かチェック
    if (!stateController_->IsCanOperateBorder()) {
        return false;
    }
    return inputMapper_->IsTriggered(PlayerInputAction::RemoveBorder);
}

bool Player::IsJumpInput() const {
    return inputMapper_->IsTriggered(PlayerInputAction::Jump);
}

bool Player::IsJumpState() const {
    return stateController_->GetCurrentState() == PlayerState::Jump;
}

bool Player::IsDeadState() const {
    return stateController_->GetCurrentState() == PlayerState::Dead;
}

//////////////////////////////////////////////////////////////
//
// 更新処理
//
//////////////////////////////////////////////////////////////

void Player::Update() {

    // エディターを更新
    Edit();

    // ポーズ中 or クリア後は移動処理を止める
    if (isPaused_ || isGetCrown_) {
        // スプライトの動きだけは更新する（アニメ演出用）
        SpriteMotion();
        return;
    }

    // 通常時の処理
    UpdateMoveDirection();
    stateController_->Update(*this);

    // スプライトの動きを更新
    SpriteMotion();

    prevDirection_ = moveDirection_;
}

void Player::UpdateMoveDirection() {

    // 入力値に応じて向きを更新
    float vector = inputMapper_->GetVector(PlayerInputAction::MoveX);
    if (vector < 0.0f) {

        moveDirection_ = LR::LEFT;
    } else if (0.0f < vector) {

        moveDirection_ = LR::RIGHT;
    }


}

// スプライトの動きを更新
void Player::SpriteMotion() {

    // ホログラム状態に応じた設定
    if (isHologram_) {
        body_.GH = TextureManager::LoadTexture(imageMap_["Body_Hologram"]);
        legs_[0].GH = TextureManager::LoadTexture(imageMap_["Leg_Hologram"]);
        legs_[1].GH = TextureManager::LoadTexture(imageMap_["Leg_Hologram"]);
    } else {
        body_.GH = TextureManager::LoadTexture(imageMap_["Body"]);
        legs_[0].GH = TextureManager::LoadTexture(imageMap_["Leg"]);
        legs_[1].GH = TextureManager::LoadTexture(imageMap_["Leg"]);
    }

    // 足のtranslateを設定
    for (int i = 0; i < 2; i++) {
        legs_[i].transform.translate = body_.transform.translate;
    }

    // 向きに応じた設定
    if (moveDirection_ == LR::LEFT) {
        body_.uvTransform.scale = { 1.0f, 1.0f };
        legs_[0].uvTransform.scale = { 1.0f, 1.0f };
        legs_[1].uvTransform.scale = { -1.0f, 1.0f };
    } else {
        body_.uvTransform.scale = { -1.0f, 1.0f };
        legs_[0].uvTransform.scale = { -1.0f, 1.0f };
        legs_[1].uvTransform.scale = { 1.0f, 1.0f };
    }

    // 徐々に回転を戻す
    body_.transform.rotate *= 0.9f;

    // 状態に応じた設定
    static float motionTimer = 0.0f;
    static float landingTimer = 0.0f;
    auto state = stateController_->GetCurrentState();

    bool isMoving = stateController_->GetIsMoving();

    // 王冠取得後は移動状態を無効化する. ポーズ中も無効化
    if (isGetCrown_ == true || isPaused_ == true) {
        isMoving = false;
    }


    if (state == PlayerState::Idle && !isMoving) {

        static float waveRadius = 8.0f;
        float sin = waveRadius * std::sinf(motionTimer * 3.14f * 1.2f);
        isMove_ = false;

        if (sin > 0.0f) {
            sin *= -0.5f;
        }

        body_.transform.scale = body_.transform.scale + (Vector2(1.0f) - body_.transform.scale) * 0.05f * ClockManager::TimeRate();
        body_.transform.translate.y = body_.transform.translate.y + sin;

    } else if (state == PlayerState::Warp) {

        const Vector2 pos = GetOwner()->GetWorldTranslate();
        const Vector2 scale = GetOwner()->GetWorldScale();

        body_.transform.translate = pos;
        legs_[0].transform.translate = pos;
        legs_[1].transform.translate = pos;

        body_.transform.scale = Vector2(scale);
        legs_[0].transform.scale = Vector2(scale);
        legs_[1].transform.scale = Vector2(scale);
        return;
    } else if (state == PlayerState::Jump or state == PlayerState::Warp) {

        float jumpVel = stateController_->GetJumpVelocity();
        landingTimer = 0.0f;
        motionTimer = 0.0f;
        isMove_ = false;
        if (jumpVel <= 0.0f) {
            float jumpT = std::clamp(jumpVel / -60.0f, 0.0f, 1.0f);
            // 上昇中は縦に伸ばす
            body_.transform.scale.y = 1.0f + jumpT * 0.2f;
            body_.transform.scale.x = 1.0f - jumpT * 0.1f;
            legs_[0].transform.scale.y = body_.transform.scale.y;
            legs_[1].transform.scale.y = body_.transform.scale.y;
            legs_[0].transform.translate.y = body_.transform.translate.y + (body_.size.y * 0.25f * body_.transform.scale.y * jumpT);
            legs_[1].transform.translate.y = body_.transform.translate.y + (body_.size.y * 0.25f * body_.transform.scale.y * jumpT);

        } else {
            // 下降中は横に伸ばす
            float fallT = std::clamp(jumpVel / 60.0f, 0.0f, 1.0f);
            body_.transform.scale.y = 1.0f - fallT * 0.1f;
            body_.transform.scale.x = 1.0f + fallT * 0.2f;
            legs_[0].transform.translate.y = body_.transform.translate.y - (body_.size.y * 0.25f * body_.transform.scale.y * fallT);
            legs_[1].transform.translate.y = body_.transform.translate.y - (body_.size.y * 0.25f * body_.transform.scale.y * fallT);
        }


    }
    // 歩いている場合のアニメーション
    // 王冠を取ったら歩行アニメーションを止める
    if (isMoving) {
        // 歩いている場合の足の動き
        if (state != PlayerState::Jump) {
            // timerのリセット
            if (!isMove_) {
                motionTimer = 0.0f;
                isMove_ = true;
            }

            static float waveRadius = 16.0f;
            static float rotRange = 3.14f / 6.0f;
            float sin = std::sinf(motionTimer * 3.14f * 4.0f);
            float cos = std::cosf(motionTimer * 3.14f * 4.0f);
            float rot = 0.0f;
            float bodyScaleT = std::clamp(motionTimer / 0.4f, 0.0f, 1.0f);

            if (moveDirection_ == LR::LEFT) {
                rot = rotRange * (sin > 0.0f ? sin : sin * -0.7f);
            } else {
                rot = rotRange * (sin <= 0.0f ? sin : sin * -0.7f);
                cos *= -1.0f;
            }

            body_.transform.scale.y = 0.6f + 0.4f * EaseOutQuad(1.0f - (1.0f - bodyScaleT));
            body_.transform.scale.x = body_.transform.scale.x + (1.0f - body_.transform.scale.x) * 0.05f * ClockManager::TimeRate();
            body_.transform.translate.y = body_.transform.translate.y + (body_.size.y - (body_.size.y * body_.transform.scale.y)) * 0.5f;
            body_.transform.rotate = rot;

            // 足を交互に上げる
            if (sin > 0.0f) {
                legs_[0].transform.translate.y = body_.transform.translate.y + waveRadius * -sin;
                legs_[0].transform.translate.x = body_.transform.translate.x + (body_.size.x * 0.1f * body_.transform.scale.x * cos);
                legs_[1].transform.translate.y = body_.transform.translate.y;
                legs_[1].transform.translate.x = body_.transform.translate.x + (body_.size.x * -0.1f * body_.transform.scale.x * cos);
            } else {
                legs_[0].transform.translate.y = body_.transform.translate.y;
                legs_[0].transform.translate.x = body_.transform.translate.x + (body_.size.x * 0.1f * body_.transform.scale.x * cos);
                legs_[1].transform.translate.y = body_.transform.translate.y + waveRadius * sin;
                legs_[1].transform.translate.x = body_.transform.translate.x + (body_.size.x * -0.1f * body_.transform.scale.x * cos);
            }
        }
    }

    // 着地した直後のアニメーション
    if (state != PlayerState::Jump) {
        //float landingT = std::clamp(landingTimer / 0.2f, 0.0f, 1.0f);
        //if (landingT < 1.0f) {
        //    float landingAnimationT = EaseOutBounce(EaseOutBounce(landingT));
        //    body_.transform.scale.y *= (0.7f + (0.3f * landingAnimationT)) *ClockManager::TimeRate();
        //    body_.transform.scale.x *= (1.15f - (0.15f * landingAnimationT)) * ClockManager::TimeRate();
        //    body_.transform.translate.y += ((body_.size.y * 0.3f * landingAnimationT) * 0.5f) * ClockManager::TimeRate();
        //}
    }

    // 時間の更新
    motionTimer += ClockManager::DeltaTime();
    landingTimer += ClockManager::DeltaTime();
}

void Player::OnGroundTrigger() {
    // 着地した瞬間
    if (stateController_->GetJumpVelocity() > 0.0f) {
        stateController_->OnGroundTrigger();
    }
}

void Player::OnCeilingTrigger() {
    // 天井に当たった瞬間
    if (stateController_->GetJumpVelocity() <= 0.0f) {
        stateController_->OnCeilingTrigger();
    }
}

bool Player::IsTriggredAnyDevice() const {

    return inputMapper_->IsTriggered(PlayerInputAction::PutBorder);
}

bool Player::TouchLaser() const {
    // レーザーに触れた
    return istouchedLaser_;
}

////////////////////////////////////////////////////////////////////////////
// 描画
/////////////////////////////////////////////////////////////////////////////

void Player::Draw() {

    // 各状態の描画
    stateController_->Draw(*this);

    // 胴体の描画
    if (stateController_->IsDead() == false) {
        body_.Draw();

        // 足を描画
        for (int i = 0; i < 2; i++) {
            legs_[i].Draw();
        }
    }

}

void Player::Edit() {
#ifdef _DEBUG
    ImFunc::CustomBegin("Player", MoveOnly_TitleBar);
    {
        ImGui::PushItemWidth(192.0f);
        if (ImGui::Button("Save Json")) {

            SaveJson();
        }

        if (ImGui::BeginTabBar("PlayerTab")) {
            if (ImGui::BeginTabItem("PlayerParam")) {

                ImGui::Text("moveDirection: %s", EnumAdapter<LR>::ToString(moveDirection_));

                ImGui::Separator();

                ImGui::DragFloat2("spriteAnchor", &body_.anchorPoint.x, 0.1f);
                ImGui::DragFloat2("spriteTranslate", &body_.transform.translate.x, 0.1f);
                ImGui::Checkbox("isHologram", &isHologram_);
                ImGui::EndTabItem();
            }
            if (ImGui::BeginTabItem("State")) {

                stateController_->Edit(*this);
                ImGui::EndTabItem();
            }
            ImGui::EndTabBar();
        }
        ImGui::PopItemWidth();

        ImGui::Checkbox("Hologram", &isHologram_);

        ImGui::End();
    }
#endif // _DEBUG
}

void Player::OnCollisionEnter([[maybe_unused]] GameObject2D* other) {

}

void Player::OnCollisionStay([[maybe_unused]] GameObject2D* other) {
    if (other->GetObjectType() == ObjectType::Laser) {

        istouchedLaser_ = true;
    } else {

        istouchedLaser_ = false;
    }
}

void Player::OnCollisionExit([[maybe_unused]] GameObject2D* other) {

}

void Player::ApplyJson() {

    nlohmann::json data;
    if (!JsonAdapter::LoadCheck(kJsonPath_, data)) {
        return;
    }

    // 状態
    stateController_->FromJson(data["StateParam"]);
}

void Player::SaveJson() {

    nlohmann::json data;

    // 状態
    stateController_->ToJson(data["StateParam"]);

    JsonAdapter::Save(kJsonPath_, data);
}