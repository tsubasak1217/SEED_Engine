#include "ClearSelectMenu.h"

//============================================================================
//	include
//============================================================================
#include <SEED/Source/Manager/ImGuiManager/ImGuiManager.h>
#include <SEED/Source/Manager/AudioManager/AudioManager.h>
#include <SEED/Lib/MagicEnumAdapter/EnumAdapter.h>
#include <SEED/Lib/JsonAdapter/JsonAdapter.h>
#include <SEED/Lib/Functions/myFunc/MyFunc.h>
#include <Game/Manager/AudioDictionary.h>
#include <Game/Scene/Input/Device/MenuBarGamePadInput.h>
#include <Game/Scene/Input/Device/MenuBarKeyInput.h>

//============================================================================
//	ClearSelectMenu classMethods
//============================================================================

void ClearSelectMenu::Initialize(uint32_t currentStageIndex, bool isLastStage, int32_t nextStageDifficulty) {

    isLastStage_ = isLastStage;
    nextStageDifficulty_ = nextStageDifficulty;

    // falseで初期化
    result_.isNextStage = false;
    result_.returnSelect = false;
    beginFrame_ = true;
    isFinishedOnce_ = false;

    // 入力クラスを初期化
    inputMapper_ = std::make_unique<InputMapper<PauseMenuInputAction>>();
    inputMapper_->AddDevice(std::make_unique<MenuBarKeyInput>());     // キー操作
    inputMapper_->AddDevice(std::make_unique<MenuBarGamePadInput>()); // パッド操作

    // メニューの初期化
    items_.clear();
    nlohmann::json data;
    if (!isLastStage) {
        kItemCount_ = 2;
        data = MyFunc::GetJson("Resources/Jsons/Clear/SelectMenu.json");
    } else {
        kItemCount_ = 1;
        data = MyFunc::GetJson("Resources/Jsons/Clear/SelectMenu_Last.json");
    }

    for (uint32_t index = 0; index < kItemCount_; ++index) {

        Item item{};

        // 基本初期化
        item.backSprite = Sprite("UI/menuItem.png");
        item.backSprite.layer = 20;
        item.backSprite.anchorPoint = 0.5f;
        item.backSprite.isApplyViewMat = false;

        // テキスト
        item.text = TextBox2D("text");
        item.text.layer = 21;
        item.text.isApplyViewMat = false;

        if (!data.empty()) {
            if (index < data.size()) {

                item.FromJson(data[index]);
            }
        }

        // アイテム追加
        items_.emplace_back(item);
    }

    // ステージ番号の初期化
    // スケール倍率0.0fで初期化
    // 背景
    stageIndexBack_ = Sprite("Scene_Select/hexagonDesign.png");
    stageIndexBack_.layer = 20;
    stageIndexBack_.anchorPoint = 0.5f;
    stageIndexBack_.isApplyViewMat = false;
    stageIndexBack_.transform.scale = 0.0f;
    stageIndexBack_.size = 160.0f;
    stageIndexBack_.color = MyMath::FloatColor(0x990099FF);
    // 番号
    stageIndexText_ = TextBox2D(std::to_string(currentStageIndex + 1));
    stageIndexText_.layer = 21;
    if (10 <= currentStageIndex + 1) {

        stageIndexText_.fontSize = 64.0f;
        stageIndexTextTranslate_ = Vector2(876.3f, 260.0f);
    } else {

        stageIndexText_.fontSize = 96.0f;
        stageIndexTextTranslate_ = Vector2(876.3f, 236.0f);
    }
    stageIndexText_.isApplyViewMat = false;
    stageIndexText_.transform.scale = 0.0f;
    stageIndexText_.SetFont("DefaultAssets/Digital/851Gkktt_005.ttf");
    stageIndexBackTranslate_ = Vector2(872.0f, 255.0f);
}

void ClearSelectMenu::Update() {

    if (beginFrame_) {

        // SE
        const float kSEVolume = 0.6f;
        AudioManager::PlayAudio(AudioDictionary::Get("クリアメニュー_開始"), false, kSEVolume);
        beginFrame_ = false;
    }

    if (result_.isNextStage || result_.returnSelect) {
        return;
    }

    // エディターの更新処理
    SelectEdit();

    if (menuTimer_.IsFinished()) {
        if (inputMapper_->IsTriggered(PauseMenuInputAction::Enter)) {

            // 現在フォーカスされている方をtrueにする
            if (isLastStage_) {
                result_.returnSelect = true;
            } else {
                if (currentMenu_ == 0) {

                    result_.isNextStage = true;

                } else {

                    result_.returnSelect = true;
                }
            }
            // SE
            const float kSEVolume = 0.24f;
            AudioManager::PlayAudio(AudioDictionary::Get("クリアメニュー_決定"), false, kSEVolume);
            return;
        }
        if (result_.isNextStage || result_.returnSelect) {
            return;
        }
    }

    // メニューの選択
    //上移動
    if (inputMapper_->GetVector(PauseMenuInputAction::MoveY) < 0.0f) {

        const float kSEVolume = 0.24f;
        currentMenu_ = MyFunc::Spiral(currentMenu_ - 1, 0, kItemCount_ - 1);
        AudioManager::PlayAudio(AudioDictionary::Get("クリアメニュー_選択"), false, kSEVolume);
    }
    //下移動
    if (inputMapper_->GetVector(PauseMenuInputAction::MoveY) > 0.0f) {

        const float kSEVolume = 0.24f;
        currentMenu_ = MyFunc::Spiral(currentMenu_ + 1, 0, kItemCount_ - 1);
        AudioManager::PlayAudio(AudioDictionary::Get("クリアメニュー_選択"), false, kSEVolume);
    }

    // メニュータイマーの更新
    float t = menuTimer_.GetProgress();
    float ease;
    if (isExit_) {

        menuTimer_.Update(-1.0f);
        ease = EaseInQuint(t);
    } else {

        menuTimer_.Update(1.0f);
        ease = EaseOutQuint(t);
    }

    // 選択中のメニューの色を変える
    for (size_t i = 0; i < kItemCount_; i++) {

        // 時間の更新
        items_[i].selectTime.Update(i == currentMenu_ ? 1.0f : -1.0f);

        // パラメーターの更新
        items_[i].backSprite.transform.translate = MyMath::Lerp(items_[i].start.translate, items_[i].end.translate, ease);
        items_[i].text.transform.translate = MyMath::Lerp(items_[i].start.translate, items_[i].end.translate, ease);
        items_[i].backSprite.size = items_[i].backSpriteSize;
        items_[i].backSprite.color = items_[i].backColor;

        float t2 = items_[i].selectTime.GetProgress();
        float ease2 = EaseOutBack(t2);
        items_[i].backSprite.transform.scale = 1.0f + 0.2f * ease2;
        items_[i].text.transform.scale = 1.0f + 0.2f * ease2;
    }

    // 座標を設定
    stageIndexBack_.transform.translate = stageIndexBackTranslate_;
    stageIndexText_.transform.translate = stageIndexTextTranslate_;

    // ステージ番号を補間
    stageIndexBack_.transform.scale = EaseOutExpo(t);
    stageIndexBack_.transform.rotate = std::lerp(std::numbers::pi_v<float>*2.0f, 0.0f, EaseOutExpo(t));
    stageIndexText_.transform.scale = std::lerp(0.0f, 1.4f, EaseOutExpo(t));

    if (menuTimer_.IsFinishedNow() && !isFinishedOnce_) {

        // SE
        const float kSEVolume = 0.6f;
        AudioManager::PlayAudio(AudioDictionary::Get("クリアメニュー_バー"), false, kSEVolume);
        isFinishedOnce_ = true;
    }
}

void ClearSelectMenu::Draw() {

    // 少し経過後に描画する
    if (menuTimer_.GetProgress() < 0.1f) {
        return;
    }

    // 描画処理
    for (size_t i = 0; i < kItemCount_; i++) {

        items_[i].backSprite.Draw();
        items_[i].text.Draw();

        // 次のステージの難易度を表示
        if (items_[i].isNextStageItem) {
            static Sprite star = Sprite("UI/star.png");
            static TextBox2D difficultyText = TextBox2D("難易度");
            static bool initialized = false;
            static float motionTimer = 0.0f;
            static Timer scaleTimer = Timer(0.25f);

            if (i != currentMenu_) {
                scaleTimer.Update(-1.0f);
                continue;
            }

            if (!initialized) {
                star.layer = 21;
                star.anchorPoint = Vector2(0.5f);
                star.isApplyViewMat = false;
                star.size = Vector2(46.0f);

                difficultyText.layer = 21;
                difficultyText.isApplyViewMat = false;
                difficultyText.fontSize = 28.0f;
                difficultyText.size = Vector2(400.0f, difficultyText.fontSize);
                difficultyText.anchorPos = Vector2(1.0f, 0.5f);
                difficultyText.align = TextAlign::RIGHT;
                difficultyText.SetFont("DefaultAssets/Digital/851Gkktt_005.ttf");
                difficultyText.useOutline = true;
                difficultyText.outlineWidth = 4.0f;
                difficultyText.textBoxVisible = false;
                difficultyText.glyphSpacing = 4.0f;
                difficultyText.color = { 1.0f,1.0f,1.0f,0.8f };
                difficultyText.outlineColor = { 0.01f,0.01f,0.01f,1.0f };

                initialized = true;
            }

            auto& bg = items_[i].backSprite;
            Vector2 starPos = bg.transform.translate + bg.size * Vector2(0.55f, 0.5f);

            for (int32_t j = 0; j < nextStageDifficulty_; j++) {
                star.transform.translate = starPos - Vector2(star.size.x * 1.0f * j, 0.0f);

                // 少しスケーリングを揺らします
                float sin = std::sin(motionTimer * 3.14f - 0.3f * j);
                star.transform.scale = Vector2(1.0f + sin * (sin > 0 ? 0.2f : -0.1f))
                    * Vector2(scaleTimer.GetProgress());
                // キラキラさせます
                star.color = MyMath::FloatColor(255, 198, 57, 255);
                if (sin < 0.0f) { star.color *= 1.0f + 5.0f * -sin; }

                star.Draw();
            }

            switch (nextStageDifficulty_) {
            case 1:
                difficultyText.text = "カンタン";
                break;
            case 2:
                difficultyText.text = "カンタン";
                break;
            case 3:
                difficultyText.text = "フツウ";
                break;
            case 4:
                difficultyText.text = "ムズカシイ";
                break;
            case 5:
                difficultyText.text = "ゲキムズ";
                break;
            default:
                difficultyText.text = "ERROR";
                break;
            }

            difficultyText.transform.translate = star.transform.translate - Vector2(star.size.x * 0.7f, 0.0f);
            difficultyText.transform.scale = Vector2(scaleTimer.GetProgress());
            difficultyText.Draw();

            scaleTimer.Update(1.0f);
            motionTimer += ClockManager::DeltaTime();
        }
    }

    // ステージ番号描画
    stageIndexBack_.Draw();
    stageIndexText_.Draw();
}

void ClearSelectMenu::SelectEdit() {
#ifdef _DEBUG
    ImFunc::CustomBegin("ClearSelectMenu", MoveOnly_TitleBar);
    {
        if (ImGui::CollapsingHeader("StageTextItem")) {
            ImGui::Indent();
            stageIndexText_.Edit();
            ImGui::DragFloat2("stageIndexTranslate", &stageIndexBackTranslate_.x, 0.01f);
            ImGui::DragFloat2("stageIndexTextTranslate", &stageIndexTextTranslate_.x, 0.01f);
            ImGui::Unindent();
        }

        std::string label;
        for (uint32_t i = 0; i < kItemCount_; i++) {
            label = "Menu" + std::to_string(i);
            if (ImGui::CollapsingHeader(label.c_str())) {

                items_[i].Edit();
            }
        }

        ImGui::Separator();

        // jsonに保存
        if (ImGui::Button("Save Json")) {

            MenuItemsToJson();
        }
        ImGui::End();
    }
#endif // _DEBUG
}

nlohmann::json ClearSelectMenu::Item::ToJson() const {

    nlohmann::json data;
    data["start"] = start;
    data["end"] = end;
    data["backColor"] = backColor;
    data["backSpriteSize"] = backSpriteSize;
    data["text"] = text.GetJsonData();
    data["isNextStageItem"] = isNextStageItem;
    return data;
}

void ClearSelectMenu::Item::FromJson(const nlohmann::json& data) {

    start = data.value("start", Transform2D());
    end = data.value("end", Transform2D());
    backColor = data.value("backColor", Vector4(1.0f));
    backSpriteSize = data.value("backSpriteSize", Vector2(200.0f, 50.0f));
    isNextStageItem = data.value("isNextStageItem", false);
    text.LoadFromJson(data["text"]);
}

void ClearSelectMenu::MenuItemsToJson() {

    nlohmann::ordered_json data = nlohmann::ordered_json::array();
    for (const auto& item : items_) {

        data.push_back(item.ToJson());
    }
    // JSONファイルに保存
    if (!isLastStage_) {
        MyFunc::CreateJsonFile("Resources/Jsons/Clear/SelectMenu.json", data);
    } else {
        MyFunc::CreateJsonFile("Resources/Jsons/Clear/SelectMenu_Last.json", data);
    }
}

void ClearSelectMenu::Item::Edit() {
#ifdef _DEBUG

    ImGuiManager::RegisterGuizmoItem(&start);
    ImGuiManager::RegisterGuizmoItem(&end);
    ImGui::DragFloat2("BackSpriteSize", &backSpriteSize.x, 1.0f, 1.0f, 1000.0f);
    ImGui::ColorEdit4("BackColor", &backColor.x);
    ImGui::Checkbox("isNextStageItem", &isNextStageItem);

    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();

    text.Edit();
#endif // _DEBUG
}