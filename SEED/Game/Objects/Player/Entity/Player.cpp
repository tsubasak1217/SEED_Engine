#include "Player.h"

//============================================================================
//	include
//============================================================================
#include <SEED/Lib/JsonAdapter/JsonAdapter.h>

// imgui
#include <SEED/Source/Manager/ImGuiManager/ImGuiManager.h>

//============================================================================
//	Player classMethods
//============================================================================

void Player::Initialize() {

    // 状態を初期化
    stateController_ = std::make_unique<PlayerStateController>();
    stateController_->Initialize();

    // スプライトを初期化
    sprite_ = Sprite("DefaultAssets/ellipse.png");
    // 仮初期位置(X適当、Yブロックの上位置)
    sprite_.translate = Vector2(128.0f, 612.0f);
    // 見にくいので赤
    sprite_.color = Vector4(1.0f, 0.0f, 0.0f, 1.0f);

    // json適応
    ApplyJson();
}

void Player::Update() {

    // エディターを更新
    Edit();

    // 状態を更新
    stateController_->Update(*this);
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

                ImGui::DragFloat2("size", &sprite_.size.x, 0.1f);
                ImGui::EndTabItem();
            }
            if (ImGui::BeginTabItem("State")) {

                stateController_->Edit(*this);
                ImGui::EndTabItem();
            }
            ImGui::EndTabBar();
        }
        ImGui::PopItemWidth();
        ImGui::End();
    }
#endif // _DEBUG
}

void Player::ApplyJson() {

    nlohmann::json data;
    if (!JsonAdapter::LoadCheck(kJsonPath_, data)) {
        return;
    }

    from_json(data.value("sprite_.size", nlohmann::json()), sprite_.size);

    // 状態
    stateController_->FromJson(data["StateParam"]);
}

void Player::SaveJson() {

    nlohmann::json data;

    to_json(data["sprite_.size"], sprite_.size);

    // 状態
    stateController_->ToJson(data["StateParam"]);

    JsonAdapter::Save(kJsonPath_, data);
}