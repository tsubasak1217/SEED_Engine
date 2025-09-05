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

// func
#include <SEED/Lib/Functions/MyFunc/Easing.h>

//============================================================================
//	Player classMethods
//============================================================================

void Player::Initialize(){

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
        legs_[i].anchorPoint = Vector2(0.5f, 0.5f);
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
    legs_[0].size = size;
    legs_[1].size = size;
}

void Player::SetWarpState(const Vector2& start, const Vector2& target){

    // 状態管理クラスに通知
    stateController_->SetWarpState(start, target);
}

bool Player::IsFinishedWarp() const{

    // ワープ状態かどうか
    return stateController_->IsFinishedWarp();
}

bool Player::IsPutBorder() const{

    // 境界線を操作できる状態かチェック
    if(!stateController_->IsCanOperateBorder()){
        return false;
    }
    return inputMapper_->IsTriggered(PlayerInputAction::PutBorder);
}

bool Player::IsRemoveBorder() const{

    // 境界線を操作できる状態かチェック
    if(!stateController_->IsCanOperateBorder()){
        return false;
    }
    return inputMapper_->IsTriggered(PlayerInputAction::RemoveBorder);
}

bool Player::IsJumpInput() const{
    return inputMapper_->IsTriggered(PlayerInputAction::Jump);
}

void Player::Update(){

    // エディターを更新
    Edit();

    // 状態を更新
    stateController_->Update(*this);

    // スプライトの動きを更新
    SpriteMotion();

    // 向いている方向を更新
    UpdateMoveDirection();
}

void Player::UpdateMoveDirection(){

    // 入力値に応じて向きを更新
    float vector = inputMapper_->GetVector(PlayerInputAction::MoveX);
    if(vector < 0.0f){

        moveDirection_ = LR::LEFT;
    } else if(0.0f < vector){

        moveDirection_ = LR::RIGHT;
    }
}

// スプライトの動きを更新
void Player::SpriteMotion(){

    // ホログラム状態に応じた設定
    if(isHologram_){
        body_.GH = imageMap_["Body_Hologram"];
        legs_[0].GH = imageMap_["Leg_Hologram"];
        legs_[1].GH = imageMap_["Leg_Hologram"];
    } else{
        body_.GH = imageMap_["Body"];
        legs_[0].GH = imageMap_["Leg"];
        legs_[1].GH = imageMap_["Leg"];
    }

    // 足のtranslateを設定
    for(int i = 0; i < 2; i++){
        legs_[i].translate = body_.translate;
    }

    // 向きに応じた設定
    if(moveDirection_ == LR::LEFT){
        body_.uvTransform = ScaleMatrix({ 1.0f, 1.0f, 1.0f });
        legs_[0].uvTransform = ScaleMatrix({ 1.0f, 1.0f, 1.0f });
        legs_[1].uvTransform = ScaleMatrix({ -1.0f, 1.0f, 1.0f });
    } else{
        body_.uvTransform = ScaleMatrix({ -1.0f, 1.0f, 1.0f });
        legs_[0].uvTransform = ScaleMatrix({ -1.0f, 1.0f, 1.0f });
        legs_[1].uvTransform = ScaleMatrix({ 1.0f, 1.0f, 1.0f });
    }

    // 徐々に回転を戻す
    body_.rotate *= 0.9f;

    // 状態に応じた設定
    static float motionTimer = 0.0f;
    static float landingTimer = 0.0f;
    auto state = stateController_->GetCurrentState();

    if(state == PlayerState::Idle && !stateController_->GetIsMoving()){

        static float waveRadius = 8.0f;
        float sin = waveRadius * std::sinf(motionTimer * 3.14f * 1.2f);
        isMove_ = false;

        if(sin > 0.0f){
            sin *= -0.5f;
        }

        body_.scale = body_.scale + (Vector2(1.0f) - body_.scale) * 0.05f * ClockManager::TimeRate();
        body_.translate.y = body_.translate.y + sin;

    } else if(state == PlayerState::Jump or state == PlayerState::Warp){

        float jumpVel = stateController_->GetJumpVelocity();
        landingTimer = 0.0f;
        motionTimer = 0.0f;
        isMove_ = false;
        if(jumpVel <= 0.0f){
            float jumpT = std::clamp(jumpVel / -60.0f, 0.0f, 1.0f);
            // 上昇中は縦に伸ばす
            body_.scale.y = 1.0f + jumpT * 0.2f;
            body_.scale.x = 1.0f - jumpT * 0.1f;
            legs_[0].scale.y = body_.scale.y;
            legs_[1].scale.y = body_.scale.y;
            legs_[0].translate.y = body_.translate.y + (body_.size.y * 0.25f * body_.scale.y * jumpT);
            legs_[1].translate.y = body_.translate.y + (body_.size.y * 0.25f * body_.scale.y * jumpT);

        } else{
            // 下降中は横に伸ばす
            float fallT = std::clamp(jumpVel / 60.0f, 0.0f, 1.0f);
            body_.scale.y = 1.0f - fallT * 0.1f;
            body_.scale.x = 1.0f + fallT * 0.2f;
            legs_[0].translate.y = body_.translate.y - (body_.size.y * 0.25f * body_.scale.y * fallT);
            legs_[1].translate.y = body_.translate.y - (body_.size.y * 0.25f * body_.scale.y * fallT);
        }


    } else if(stateController_->GetIsMoving()){
        // 歩いている場合の足の動き
        if(state != PlayerState::Jump){
            // timerのリセット
            if(!isMove_){
                motionTimer = 0.0f;
                isMove_ = true;
            }

            static float waveRadius = 16.0f;
            static float rotRange = 3.14f / 6.0f;
            float sin = std::sinf(motionTimer * 3.14f * 4.0f);
            float cos = std::cosf(motionTimer * 3.14f * 4.0f);
            float rot = 0.0f;
            float bodyScaleT = std::clamp(motionTimer / 0.4f, 0.0f, 1.0f);

            if(moveDirection_ == LR::LEFT){
                rot = rotRange * (sin > 0.0f ? sin : sin * -0.7f);
            } else{
                rot = rotRange * (sin <= 0.0f ? sin : sin * -0.7f);
                cos *= -1.0f;
            }

            body_.scale.y = 0.6f + 0.4f * EaseOutQuad(1.0f - (1.0f - bodyScaleT));
            body_.scale.x = body_.scale.x + (1.0f - body_.scale.x) * 0.05f * ClockManager::TimeRate();
            body_.translate.y = body_.translate.y + (body_.size.y - (body_.size.y * body_.scale.y)) * 0.5f;
            body_.rotate = rot;

            // 足を交互に上げる
            if(sin > 0.0f){
                legs_[0].translate.y = body_.translate.y + waveRadius * -sin;
                legs_[0].translate.x = body_.translate.x + (body_.size.x * 0.1f * body_.scale.x * cos);
                legs_[1].translate.y = body_.translate.y;
                legs_[1].translate.x = body_.translate.x + (body_.size.x * -0.1f * body_.scale.x * cos);
            } else{
                legs_[0].translate.y = body_.translate.y;
                legs_[0].translate.x = body_.translate.x + (body_.size.x * 0.1f * body_.scale.x * cos);
                legs_[1].translate.y = body_.translate.y + waveRadius * sin;
                legs_[1].translate.x = body_.translate.x + (body_.size.x * -0.1f * body_.scale.x * cos);
            }
        }
    }

    // 着地した直後のアニメーション
    if(state != PlayerState::Jump){
        float landingT = std::clamp(landingTimer / 0.2f, 0.0f, 1.0f);
        if(landingT < 1.0f){
            float landingAnimationT = EaseOutBounce(EaseOutBounce(landingT));
            body_.scale.y *= 0.7f + (0.3f * landingAnimationT);
            body_.scale.x *= 1.15f - (0.15f * landingAnimationT);
            body_.translate.y += (body_.size.y * 0.3f * landingAnimationT) * 0.5f;
        }
    }

    // 時間の更新
    motionTimer += ClockManager::DeltaTime();
    landingTimer += ClockManager::DeltaTime();
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

void Player::Draw(){

    // 胴体の描画
    body_.Draw();

    // 足を描画
    for(int i = 0; i < 2; i++){
        legs_[i].Draw();
    }
}

void Player::Edit(){
#ifdef _DEBUG
    ImFunc::CustomBegin("Player", MoveOnly_TitleBar);
    {
        ImGui::PushItemWidth(192.0f);
        if(ImGui::Button("Save Json")){

            SaveJson();
        }

        if(ImGui::BeginTabBar("PlayerTab")){
            if(ImGui::BeginTabItem("PlayerParam")){

                ImGui::Text("moveDirection: %s", EnumAdapter<LR>::ToString(moveDirection_));

                ImGui::Separator();

                ImGui::DragFloat2("spriteAnchor", &body_.anchorPoint.x, 0.1f);
                ImGui::DragFloat2("spriteTranslate", &body_.translate.x, 0.1f);
                ImGui::EndTabItem();
            }
            if(ImGui::BeginTabItem("State")){

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

void Player::ApplyJson(){

    nlohmann::json data;
    if(!JsonAdapter::LoadCheck(kJsonPath_, data)){
        return;
    }

    // 状態
    stateController_->FromJson(data["StateParam"]);
}

void Player::SaveJson(){

    nlohmann::json data;

    // 状態
    stateController_->ToJson(data["StateParam"]);

    JsonAdapter::Save(kJsonPath_, data);
}