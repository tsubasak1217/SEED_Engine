#include "Player.h"

//============================================================================
//	include
//============================================================================
#include <SEED/Lib/MagicEnumAdapter/EnumAdapter.h>
#include <SEED/Lib/JsonAdapter/JsonAdapter.h>

// inputDevice
#include <Game/Objects/Stage/Objects/Player/Input/Device/PlayerKeyInput.h>
#include <Game/Objects/Stage/Objects/Player/Input/Device/PlayerGamePadInput.h>

// manager
#include <SEED/Source/Manager/TextureManager/TextureManager.h>
#include <SEED/Source/Manager/ImGuiManager/ImGuiManager.h>

//============================================================================
//	Player classMethods
//============================================================================

void Player::Initialize() {

    // 画像ハンドルの初期化
    static bool isFirstInitialize = true;
    if(isFirstInitialize){
        imageMap_["Body"] = TextureManager::LoadTexture("Scene_Game/StageObject/Player/PlayerBody.png");
        imageMap_["Body_Hologram"] = TextureManager::LoadTexture("Scene_Game/StageObject/Player/PlayerBody_Hologram.png");
        imageMap_["Leg"] = TextureManager::LoadTexture("Scene_Game/StageObject/Player/PlayerLeg.png");
        imageMap_["Leg_Hologram"] = TextureManager::LoadTexture("Scene_Game/StageObject/Player/PlayerLeg_Hologram.png");
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
    for(int i = 0; i < 2; i++){
        legs_[i] = Sprite(imageMap_["Leg"]);
        legs_[i].anchorPoint = Vector2(0.5f, 0.0f);
        legs_[i].layer = baseLayer_ + (i == 0 ? 1 : -1);
        legs_[i].uvTransform = ScaleMatrix({ i == 0 ? 1.0f : -1.0f, 1.0f, 1.0f });
    }

    // json適応
    ApplyJson();

    // 初期化値
    // 最初は右向き
    moveDirection_ = LR::RIGHT;
}

// spriteのサイズ設定
void Player::SetSize(const Vector2& size){
    body_.size = size;
    legs_[0].size = Vector2(size.x * 0.5f, size.y * 0.5f);
    legs_[1].size = Vector2(size.x * 0.5f, size.y * 0.5f);
}

void Player::SetWarpState(const Vector2& start, const Vector2& target) {

    // 状態管理クラスに通知
    stateController_->SetWarpState(start, target);
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

void Player::Update() {

    // エディターを更新
    Edit();

    // 状態を更新
    stateController_->Update(*this);

    // 向いている方向を更新
    UpdateMoveDirection();
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
void Player::SpriteMotion(){
}

void Player::OnGroundTrigger() {
    // 着地した瞬間
    if (stateController_->GetJumpVelocity() > 0.0f) {
        stateController_->OnGroundTrigger();
    }
}

void Player::OnCeilingTrigger(){
    // 天井に当たった瞬間
    if(stateController_->GetJumpVelocity() <= 0.0f){
        stateController_->OnCeilingTrigger();
    }
}

void Player::Draw() {

    // 胴体の描画
    body_.Draw();

    // 足を描画
    for(int i = 0; i < 2; i++){
        legs_[i].Draw();
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
                ImGui::DragFloat2("spriteTranslate", &body_.translate.x, 0.1f);
                ImGui::EndTabItem();
            }
            if (ImGui::BeginTabItem("State")) {

                stateController_->Edit(*this);
                ImGui::EndTabItem();
            }
            ImGui::EndTabBar();
        }
        ImGui::PopItemWidth();

        if (owner_->GetIsOnGround()) {
            ImGui::Text("OnGround");
        }

        ImGui::End();
    }
#endif // _DEBUG
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