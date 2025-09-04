#include "Player.h"

//============================================================================
//	include
//============================================================================
#include <SEED/Lib/MagicEnumAdapter/EnumAdapter.h>
#include <SEED/Lib/JsonAdapter/JsonAdapter.h>

// inputDevice
#include <Game/Objects/Stage/Objects/Player/Input/Device/PlayerKeyInput.h>
#include <Game/Objects/Stage/Objects/Player/Input/Device/PlayerGamePadInput.h>

// imgui
#include <SEED/Source/Manager/ImGuiManager/ImGuiManager.h>

//============================================================================
//	Player classMethods
//============================================================================

void Player::Initialize(const std::string& filename) {

    // 入力クラスを初期化
    inputMapper_ = std::make_unique<InputMapper<PlayerInputAction>>();
    inputMapper_->AddDevice(std::make_unique<PlayerKeyInput>());     // キー操作
    inputMapper_->AddDevice(std::make_unique<PlayerGamePadInput>()); // パッド操作

    // 状態を初期化
    stateController_ = std::make_unique<PlayerStateController>();
    stateController_->Initialize(inputMapper_.get());

    // スプライトを初期化
    sprite_ = Sprite(filename);
    sprite_.anchorPoint = Vector2(0.5f);
    // 見にくいので赤
    sprite_.color = Vector4(1.0f, 0.0f, 0.0f, 1.0f);

    // json適応
    ApplyJson();

    // 初期化値
    // 最初は右向き
    moveDirection_ = LR::RIGHT;
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

bool Player::IsJumpInput() const{
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

void Player::OnGroundTrigger(){
    // 着地した瞬間
    if(stateController_->GetJumpVelocity() > 0.0f){
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

    // 描画
    sprite_.Draw();
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

                ImGui::DragFloat2("spriteAnchor", &sprite_.anchorPoint.x, 0.1f);
                ImGui::DragFloat2("spriteTranslate", &sprite_.translate.x, 0.1f);
                ImGui::EndTabItem();
            }
            if (ImGui::BeginTabItem("State")) {

                stateController_->Edit(*this);
                ImGui::EndTabItem();
            }
            ImGui::EndTabBar();
        }
        ImGui::PopItemWidth();

        if(owner_->GetIsOnGround()){
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