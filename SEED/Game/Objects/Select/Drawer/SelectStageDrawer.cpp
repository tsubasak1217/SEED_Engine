#include "SelectStageDrawer.h"

//============================================================================
//	include
//============================================================================
#include <Environment/Environment.h>
#include <SEED/Lib/JsonAdapter/JsonAdapter.h>
#include <SEED/Lib/MagicEnumAdapter/EnumAdapter.h>
#include <SEED/Source/Manager/SceneManager/SceneManager.h>
#include <SEED/Source/Manager/AudioManager/AudioManager.h>
#include <Game/Objects/Stage/Enum/StageObjectType.h>
#include <Game/Objects/Select/Methods/SelectStageBuilder.h>
#include <Game/Manager/AudioDictionary.h>
#include <Game/GameSystem.h>

// imgui
#include <SEED/Source/Manager/ImGuiManager/ImGuiManager.h>

//============================================================================
//	SelectStageDrawer classMethods
//============================================================================

void SelectStageDrawer::Initialize(uint32_t firstFocusStage) {

    // ステージのデータを受け取る
    stages_.clear();
    stages_ = SceneManager::GetCreator()->GetStages();

    // ステージの名前
    stageNameText_ = TextBox2D("");
    stageNameText_.anchorPos = Vector2(0.5f, 0.0f);
    stageNameText_.glyphSpacing = 0.0f;
    stageNameText_.size = Vector2(1280.0f, 400.0f);
    stageNameText_.textBoxVisible = false;
    stageNameText_.isApplyViewMat = false;

    // json適応
    ApplyJson();

    // 初期化値設定
    // 最初のフォーカス先を設定
    focusIndex_ = std::min<uint32_t>(firstFocusStage, static_cast<uint32_t>(stages_.size()) - 1);
    animFrom_ = static_cast<float>(focusIndex_);
    animTo_ = static_cast<float>(focusIndex_);
    moveTimer_.Reset();
    stageInfo_.isNextScene = false;
    currentState_ = State::Select;

    // 中央のアニメーションフレームの初期化
    focusAnimFrame_ = Sprite("Scene_Select/selectStageRect.png");
    focusAnimFrame_.anchorPoint = 0.5f;
    focusAnimFrame_.isApplyViewMat = false;
    focusAnimFrame_.color = frameColor_;
    focusAnimTimer_.Reset();
    focusAnimForward_ = true;
    // ステージ番号背景アニメーションの初期化
    stageIndexBackAnim_ = Sprite("Scene_Select/hexagonDesign.png");
    stageIndexBackAnim_.anchorPoint = 0.5f;
    stageIndexBackAnim_.isApplyViewMat = false;
    stageIndexBackAnim_.color = stageIndexBackColor_;

    // 矢印
    // 左
    leftArrow_ = Sprite("Scene_Select/selectArrow.png");
    leftArrow_.anchorPoint = 0.5f;
    leftArrow_.transform.scale = 0.64f;
    leftArrow_.transform.rotate = std::numbers::pi_v<float>;
    leftArrow_.color = MyMath::FloatColor(0xD61DDAFF);
    leftArrow_.blendMode = BlendMode::ADD;
    // 右
    rightArrow_ = Sprite("Scene_Select/selectArrow.png");
    rightArrow_.anchorPoint = 0.5f;
    rightArrow_.transform.scale = 0.64f;
    rightArrow_.color = MyMath::FloatColor(0xD61DDAFF);
    rightArrow_.blendMode = BlendMode::ADD;
    // 共通設定
    leftArrow_.transform.translate.y = arrowTranslateY_;
    rightArrow_.transform.translate.y = arrowTranslateY_;
    leftArrow_.transform.translate.x = kWindowCenter.x - arrowSpacing_;
    rightArrow_.transform.translate.x = kWindowCenter.x + arrowSpacing_;

    // 倍速で設定
    leftArrowReactTimer_.duration = arrowAnimTimer_.duration * reactRate_;
    rightArrowReactTimer_.duration = arrowAnimTimer_.duration * reactRate_;

    // テキスト設定
    stageNameText_.color = stageNameTextColor_;
    stageNameText_.fontSize = stageNameTextSize_;
    stageNameText_.transform.translate = stageNameTextTranslate_;
}

void SelectStageDrawer::SetNextFocus() {

    // 次のステージへフォーカスさせる
    if(stages_.size() <= focusIndex_ + 1){
        return;
    }
    StartMoveToNext(focusIndex_ + 1);
    // 右リアクション
    TriggerRightArrowReact();

    const float kSEVolume = 0.24f;
    AudioManager::PlayAudio(AudioDictionary::Get("セレクトシーン_左右"), false, kSEVolume);
}

void SelectStageDrawer::SetPrevFocus(){

    // 1つ前のステージをフォーカスさせる
    if(focusIndex_ == 0){
        return;
    }
    StartMoveToNext(focusIndex_ - 1);
    // 左リアクション
    TriggerLeftArrowReact();

    const float kSEVolume = 0.24f;
    AudioManager::PlayAudio(AudioDictionary::Get("セレクトシーン_左右"), false, kSEVolume);
}

void SelectStageDrawer::SetEndFocus(){

    // なにか処理中でなければステージの決定にする
    if(currentState_ != State::Select){
        return;
    }
    // 決定アニメーションに移る
    currentState_ = State::End;
    endPhase_ = EndPhase::Decide;

    // フォーカスフレームを2倍速で再生させる
    focusAnimTimer_.duration = focusAnimBaseDuration_ * 0.5f;
    focusAnimTimer_.Reset();
    focusAnimForward_ = true;

    // ステージ番号を記録
    stageInfo_.decideStage = focusIndex_;

    const float kSEVolume = 0.24f;
    AudioManager::PlayAudio(AudioDictionary::Get("セレクトシーン_決定"), false, kSEVolume);
}

void SelectStageDrawer::Update(){

    // 次のシーンに進ませるので処理しない
    if(stageInfo_.isNextScene){
        return;
    }

    switch(currentState_){
    case SelectStageDrawer::State::Start:
        break;
    case SelectStageDrawer::State::Select:

        // 中央フォーカスの更新
        UpdateFocusAnim();
        break;
    case SelectStageDrawer::State::Move:

        // 補間中にのみタイマーを進める
        moveTimer_.Update();
        // 補間終了後選択可能にする
        if(moveTimer_.IsFinished()){

            currentState_ = State::Select;
            animFrom_ = animTo_;
        }

        // 動いているときはフォーカスタイマーをリセットする
        focusAnimTimer_.Reset();
        break;
    case SelectStageDrawer::State::End:

        // 決定処理
        switch(endPhase_){
        case SelectStageDrawer::EndPhase::Decide:

            // 2倍速フォーカス
            focusAnimTimer_.Update();
            if(focusAnimTimer_.IsFinished()){

                // 終了後値を元に戻してリセットしてズーム処理に移る
                focusAnimTimer_.duration = focusAnimBaseDuration_;
                endZoomTimer_.Reset();
                endPhase_ = EndPhase::Zoom;
            }
            break;
        case SelectStageDrawer::EndPhase::Zoom:

            // ズーム処理を進める
            endZoomTimer_.Update();

            if(endZoomTimer_.IsFinishedNow()){

                // 次のシーンに進める
                stageInfo_.isNextScene = true;
            }
            break;
        }
        break;
    }

    // 矢印の更新処理
    UpdateArrow();
    // 難易度の星の更新
    UpdateDifficultyStar();
}

void SelectStageDrawer::UpdateArrow(){

    // 通常振幅タイマー
    arrowAnimTimer_.Update();
    if(arrowAnimTimer_.IsFinished()){
        arrowAnimTimer_.Reset();
    }

    // リアクション中だけタイマーを変える
    if(leftArrowReacting_){

        leftArrowReactTimer_.Update();
        if(leftArrowReactTimer_.IsFinished()){

            leftArrowReacting_ = false;
        }
    }
    if(rightArrowReacting_){

        rightArrowReactTimer_.Update();
        if(rightArrowReactTimer_.IsFinished()){

            rightArrowReacting_ = false;
        }
    }

    // 進行度
    const float tBase = arrowAnimTimer_.GetEase(arrowEasing_);
    const float easedLeft = leftArrowReacting_ ? leftArrowReactTimer_.GetEase(arrowEasing_) : tBase;
    const float easedRight = rightArrowReacting_ ? rightArrowReactTimer_.GetEase(arrowEasing_) : tBase;

    // 正弦波に変換
    const float sinLeft = std::sin(easedLeft * std::numbers::pi_v<float> *2.0f);
    const float sinRight = std::sin(easedRight * std::numbers::pi_v<float> *2.0f);
    leftArrow_.offset.x = -sinLeft * arrowAmplitude_ * (leftArrowReacting_ ? 2.0f : 1.0f);
    rightArrow_.offset.x = sinRight * arrowAmplitude_ * (rightArrowReacting_ ? 2.0f : 1.0f);
}

void SelectStageDrawer::Draw(){

    // ズーム処理中
    if(currentState_ == State::End &&
        endPhase_ == EndPhase::Zoom){

        DrawEndZoom();
        return;
    }

    // 各ステージを描画する
    float f = (currentState_ == State::Move)
        ? animFrom_ + (animTo_ - animFrom_) * moveTimer_.GetEase(moveEasing_)
        : static_cast<float>(focusIndex_);

    // ステージの名前をセット
    stageNameText_.text = stages_[focusIndex_].stageName;
    for(size_t i = 0; i < stages_.size(); ++i){

        float off = static_cast<float>(i) - f;
        if(off < -1.01f || off > 1.01f){
            continue;
        }

        // アニメーション後の値を参照で渡す
        Vector2 translate, size;
        PoseFromOffset(off, translate, size);
        ApplyPoseToStage(stages_[i], translate, size);

        // 背景描画
        stages_[i].background.Draw();
        // 背景アニメーションを描画
        DrawFocusAnim();
        // フレーム描画
        stages_[i].frame.Draw();
        // ステージ番号描画
        stages_[i].stageIndexBack.Draw();
        stages_[i].stageIndexText.Draw();
        // クリア済みUI描画
        if(stages_[i].isClear){
            stages_[i].achievementUI.Draw();
        }
        stageNameText_.Draw();
        // ステージ描画
        for(auto& sprite : stages_[i].objects){

            sprite.Draw();
        }
    }

    // 新しく表示するステージを補間しながら描画する
    DrawActivate(f);

    // 矢印の描画
    if(focusIndex_ != 0){

        leftArrow_.Draw();
    }
    if(focusIndex_ + 1 != maxStageCount_){

        rightArrow_.Draw();
    }

    // 難易度の星の表示
    {
        // 描画
        for(auto& star : difficultyStars_){
            star.Draw();
        }
    }
}

void SelectStageDrawer::Edit(){
#ifdef _DEBUG
    if(ImGui::Button("Save Json")){

        SaveJson();
    }

    if(ImGui::CollapsingHeader("EveryEditStageParam")){
        ImGui::Indent();

        // フォント設定
        static std::filesystem::path currentDir = "Resources/Fonts";
        std::string selectedFont = ImFunc::FolderView("フォント一覧", currentDir, false, { ".ttf",".otf" }, "Resources/Fonts");
        if(!selectedFont.empty()){
            stageNameText_.SetFont(selectedFont);
        }
        if(ImGui::Checkbox("isSameFontStageIndex", &isSameFontStageIndex_)){
            if(isSameFontStageIndex_){
                for(size_t i = 0; i < stages_.size(); ++i){

                    stages_[i].stageIndexText.SetFont(stageNameText_.fontName);
                }
            } else{
                for(size_t i = 0; i < stages_.size(); ++i){

                    stages_[i].stageIndexText.SetFont("Game/x10y12pxDonguriDuel.ttf");
                }
            }
        }
        ImGui::Separator();

        // 保存配列のサイズをステージ数に合わせる
        if(stageNames_.size() != stages_.size()){
            stageNames_.resize(stages_.size());
            for(size_t i = 0; i < stages_.size(); ++i){
                if(stageNames_[i].empty()){
                    stageNames_[i] = stages_[i].stageName;
                }
            }
        }

        for(size_t i = 0; i < stages_.size(); ++i){
            ImGui::PushID(static_cast<int>(i));

            // 表示ラベル
            ImGui::Text("Stage %d", static_cast<int>(i + 1));
            ImGui::Checkbox("Clear", &stages_[i].isClear); // クリア済みフラグ
            ImGui::SameLine();

            // 編集ボックス
            if(ImFunc::InputText("Stage %d", stages_[i].stageName)){
                stageNames_[i] = stages_[i].stageName;
            }

            // 難易度
            ImGui::SliderInt("Difficulty %d", &stages_[i].difficulty, 1, 5);

            ImGui::PopID();
        }
        ImGui::Separator();
        ImGui::Unindent();
    }

    if(ImGui::CollapsingHeader("EditStageGeneralParam")){
        ImGui::Indent();
        {
            ImGui::DragFloat("moveDuration", &moveTimer_.duration, 0.01f);
            ImGui::DragFloat2("centerTranslate", &centerTranslate_.x, 1.0f);
            ImGui::DragFloat2("leftTranslate", &leftTranslate_.x, 1.0f);
            ImGui::DragFloat2("rightTranslate", &rightTranslate_.x, 1.0f);

            ImGui::DragFloat2("focusSize", &focusSize_.x, 1.0f);
            ImGui::DragFloat2("outSize", &outSize_.x, 1.0f);
            ImGui::DragFloat("tileScale", &tileScale_, 0.01f);
            ImGui::DragFloat("stageIndexTextOffsetY", &stageIndexTextOffsetY_, 1.0f);
            ImGui::DragFloat("stageIndexBackOffsetY", &stageIndexBackOffsetY_, 1.0f);
            ImGui::DragFloat("stageAchievementUIOffsetY", &stageAchievementUIOffsetY_, 1.0f);
            if(ImGui::DragFloat2("stageNameTextTranslate", &stageNameTextTranslate_.x, 1.0f)){

                stageNameText_.transform.translate = stageNameTextTranslate_;
            }

            if(ImGui::DragFloat("stageIndexBackSize", &stageIndexBackSize_)){

                // 全てのフレームに適応
                for(auto& stage : stages_){

                    stage.stageIndexBack.size = stageIndexBackSize_;
                }
            }
            if(ImGui::DragFloat("stageIndexTextSize", &stageIndexTextSize_)){

                // 全てのフレームに適応
                for(auto& stage : stages_){

                    stage.stageIndexText.fontSize = stageIndexTextSize_;
                }
            }
            if(ImGui::DragFloat("stageNameTextSize", &stageNameTextSize_)){

                // 全てのフレームに適応
                stageNameText_.fontSize = stageNameTextSize_;
            }
            if(ImGui::ColorEdit4("stageIndexTextColor", &stageIndexTextColor_.x)){

                // 全てのフレームに適応
                for(auto& stage : stages_){

                    stage.stageIndexText.color = stageIndexTextColor_;
                }
            }
            if(ImGui::ColorEdit4("stageIndexBackColor", &stageIndexBackColor_.x)){

                // 全てのフレームに適応
                for(auto& stage : stages_){

                    stage.stageIndexBack.color = stageIndexBackColor_;
                }
            }
            if(ImGui::ColorEdit4("stageNameTextColor", &stageNameTextColor_.x)){

                // 全てのフレームに適応
                stageNameText_.color = stageNameTextColor_;
            }
            if(ImGui::ColorEdit4("frameColor", &frameColor_.x)){

                // 全てのフレームに適応
                for(auto& stage : stages_){

                    stage.frame.color = frameColor_;
                }
            }
            if(ImGui::ColorEdit4("backgroundColor", &backgroundColor_.x)){

                // 全てのフレームに適応
                for(auto& stage : stages_){

                    stage.background.color = backgroundColor_;
                }
            }

            EnumAdapter<Easing::Type>::Combo("moveEasing", &moveEasing_);

            ImGui::SeparatorText("FocusAnim");

            ImGui::DragFloat("animDuration", &focusAnimTimer_.duration, 0.01f);
            ImGui::DragFloat("animFrom", &focusAnimFrom_, 0.001f);
            ImGui::DragFloat("animTo", &focusAnimTo_, 0.001f);
            EnumAdapter<Easing::Type>::Combo("focusAnimEasing", &focusAnimEasing_);

            ImGui::SeparatorText("EndAnim");

            if(ImGui::Button("SetEndFocus")){

                SetEndFocus();
            }

            ImGui::DragFloat("endZoomDuration", &endZoomTimer_.duration, 0.01f);
            ImGui::DragFloat("endZoomToScale", &endZoomToScale_, 0.01f);
            EnumAdapter<Easing::Type>::Combo("endZoomEasing", &endZoomEasing_);

            if(ImGui::CollapsingHeader("Arrow")){
                ImGui::Indent();

                if(ImGui::DragFloat("translateY", &arrowTranslateY_, 1.0f)){

                    leftArrow_.transform.translate.y = arrowTranslateY_;
                    rightArrow_.transform.translate.y = arrowTranslateY_;
                }
                if(ImGui::DragFloat("arrowSpacing", &arrowSpacing_, 1.0f)){

                    leftArrow_.transform.translate.x = kWindowCenter.x - arrowSpacing_;
                    rightArrow_.transform.translate.x = kWindowCenter.x + arrowSpacing_;
                }
                ImGui::DragFloat("arrowAmplitude", &arrowAmplitude_, 0.1f);
                ImGui::DragFloat("reactRate", &reactRate_, 0.1f);
                if(ImGui::DragFloat("arrowDuration", &arrowAnimTimer_.duration, 0.1f)){

                    leftArrowReactTimer_.duration = arrowAnimTimer_.duration * reactRate_;
                    rightArrowReactTimer_.duration = arrowAnimTimer_.duration * reactRate_;
                }
                EnumAdapter<Easing::Type>::Combo("arrowEasing", &arrowEasing_);
                if(ImGui::CollapsingHeader("Left")){
                    ImGui::Indent();
                    ImGui::PushID("Left");
                    leftArrow_.Edit();
                    ImGui::PopID();
                    ImGui::Unindent();
                }
                if(ImGui::CollapsingHeader("Right")){
                    ImGui::Indent();
                    ImGui::PushID("Right");
                    rightArrow_.Edit();
                    ImGui::PopID();
                    ImGui::Unindent();
                }
                ImGui::Unindent();
            }
            if(ImGui::CollapsingHeader("StageName")){
                ImGui::Indent();
                stageNameText_.Edit();
                ImGui::Unindent();
            }

            ImGui::Separator();
            if(ImGui::CollapsingHeader("Difficulty")){
                ImGui::Indent();
                ImGui::DragFloat2("difficultyStarBasePos", &difficultyStarBasePos_.x, 1.0f);
                ImGui::DragFloat("starDrawRangeX", &starDrawRangeX_, 1.0f);
                ImGui::Text("--StarSprite--");
                difficultyStars_[0].Edit();
                ImGui::Unindent();
            }
        }
        ImGui::Unindent();
    }
#endif // _DEBUG
}

void SelectStageDrawer::ApplyJson(){

    nlohmann::json data = SceneManager::GetCreator()->GetDrawerParams();

    moveTimer_.duration = data.value("moveTimer_.duration", 0.32f);
    tileScale_ = data.value("tileScale_", 1.0f);
    stageIndexTextOffsetY_ = data.value("stageIndexTextOffsetY_", 128.0f);
    stageIndexBackOffsetY_ = data.value("stageIndexBackOffsetY_", 128.0f);
    stageAchievementUIOffsetY_ = data.value("stageAchievementUIOffsetY_", 180.0f);
    stageIndexBackSize_ = data.value("stageIndexBackSize_", 128.0f);
    stageIndexTextSize_ = data.value("stageIndexTextSize_", 128.0f);
    stageNameTextSize_ = data.value("stageNameTextSize_", 128.0f);
    stageAchievementUISize_ = data.value("stageAchievementUISize_", 64.0f);

    from_json(data["centerTranslate_"], centerTranslate_);
    from_json(data["leftTranslate_"], leftTranslate_);
    from_json(data["rightTranslate_"], rightTranslate_);
    from_json(data["focusSize_"], focusSize_);
    from_json(data["outSize_"], outSize_);
    from_json(data.value("stageNameTextTranslate_", nlohmann::json()), stageNameTextTranslate_);
    from_json(data.value("stageIndexTextColor_", nlohmann::json()), stageIndexTextColor_);
    from_json(data.value("stageIndexBackColor_", nlohmann::json()), stageIndexBackColor_);
    from_json(data.value("stageNameTextColor_", nlohmann::json()), stageNameTextColor_);
    from_json(data.value("frameColor_", nlohmann::json()), frameColor_);
    from_json(data.value("backgroundColor_", nlohmann::json()), backgroundColor_);

    moveEasing_ = EnumAdapter<Easing::Type>::FromString(data["moveEasing_"]).value();

    focusAnimTimer_.duration = data.value("focusAnimDuration_", 0.9f);
    focusAnimBaseDuration_ = focusAnimTimer_.duration;
    focusAnimFrom_ = data.value("focusAnimFrom_", 1.0f);
    focusAnimTo_ = data.value("focusAnimTo_", 1.0f);
    focusAnimEasing_ = EnumAdapter<Easing::Type>::FromString(data["focusAnimEasing_"]).value();

    endZoomTimer_.duration = data.value("endZoomTimer_.duration", 0.9f);
    endZoomToScale_ = data.value("endZoomToScale_", 1.0f);
    endZoomEasing_ = EnumAdapter<Easing::Type>::FromString(data["endZoomEasing_"]).value();

    leftArrow_.FromJson(data.value("leftArrow_", nlohmann::json()));
    rightArrow_.FromJson(data.value("rightArrow_", nlohmann::json()));
    arrowAnimTimer_.duration = data.value("arrowAnimTimer_.duration", 0.9f);

    arrowSpacing_ = data.value("arrowSpacing_", 0.9f);
    arrowAmplitude_ = data.value("arrowAmplitude_", 0.9f);
    arrowTranslateY_ = data.value("arrowTranslateY_", 0.9f);
    reactRate_ = data.value("reactRate_", 0.9f);
    isSameFontStageIndex_ = data.value("isSameFontStageIndex_", false);
    arrowEasing_ = EnumAdapter<Easing::Type>::FromString(data["arrowEasing_"]).value();

    stageNames_.clear();
    if (data.contains("stageNames") && data["stageNames"].is_array()) {
        for (auto& name : data["stageNames"]) if (name.is_string()) stageNames_.push_back(name.get<std::string>());
    }

    if (data.contains("stageNameText_.fontName")) {
        stageNameText_.SetFont(data["stageNameText_.fontName"]);
    }

    stageDifficulties_.clear();
    if (data.contains("stageDifficulties") && data["stageDifficulties"].is_array()) {
        for (auto& diff : data["stageDifficulties"]) if (diff.is_number_integer()) stageDifficulties_.push_back(diff.get<int32_t>());
    }
    if (data.contains("difficultyStarBasePos")) difficultyStarBasePos_ = data["difficultyStarBasePos"].get<Vector2>();
    if (data.contains("starDrawRangeX"))        starDrawRangeX_ = data["starDrawRangeX"].get<float>();
    if (data.contains("starSprite")) {
        for (auto& s : difficultyStars_) s.FromJson(data["starSprite"]);
    } else {
        for (auto& s : difficultyStars_) s = Sprite("DefaultAssets/white1x1.png");
    }
}

void SelectStageDrawer::SaveJson(){

    nlohmann::json data;

    data["moveTimer_.duration"] = moveTimer_.duration;
    data["tileScale_"] = tileScale_;
    data["stageIndexTextOffsetY_"] = stageIndexTextOffsetY_;
    data["stageIndexBackOffsetY_"] = stageIndexBackOffsetY_;
    data["stageAchievementUIOffsetY_"] = stageAchievementUIOffsetY_;

    to_json(data["stageNameTextTranslate_"], stageNameTextTranslate_);
    to_json(data["centerTranslate_"], centerTranslate_);
    to_json(data["leftTranslate_"], leftTranslate_);
    to_json(data["rightTranslate_"], rightTranslate_);
    to_json(data["focusSize_"], focusSize_);
    to_json(data["outSize_"], outSize_);
    to_json(data["stageIndexTextColor_"], stageIndexTextColor_);
    to_json(data["stageIndexBackColor_"], stageIndexBackColor_);
    to_json(data["stageNameTextColor_"], stageNameTextColor_);
    to_json(data["frameColor_"], frameColor_);
    to_json(data["backgroundColor_"], backgroundColor_);
    to_json(data["stageIndexBackSize_"], stageIndexBackSize_);
    to_json(data["stageIndexTextSize_"], stageIndexTextSize_);
    to_json(data["stageNameTextSize_"], stageNameTextSize_);
    to_json(data["stageAchievementUISize_"], stageAchievementUISize_);

    data["moveEasing_"] = EnumAdapter<Easing::Type>::ToString(moveEasing_);

    data["focusAnimDuration_"] = focusAnimTimer_.duration;
    data["focusAnimFrom_"] = focusAnimFrom_;
    data["focusAnimTo_"] = focusAnimTo_;
    data["focusAnimEasing_"] = EnumAdapter<Easing::Type>::ToString(focusAnimEasing_);

    data["endZoomTimer_.duration"] = endZoomTimer_.duration;
    data["endZoomToScale_"] = endZoomToScale_;
    data["endZoomEasing_"] = EnumAdapter<Easing::Type>::ToString(endZoomEasing_);

    data["leftArrow_"] = leftArrow_.ToJson();
    data["rightArrow_"] = rightArrow_.ToJson();
    data["arrowAnimTimer_.duration"] = arrowAnimTimer_.duration;

    data["arrowSpacing_"] = arrowSpacing_;
    data["arrowAmplitude_"] = arrowAmplitude_;
    data["arrowTranslateY_"] = arrowTranslateY_;
    data["reactRate_"] = reactRate_;
    data["arrowEasing_"] = EnumAdapter<Easing::Type>::ToString(arrowEasing_);

    stageNames_.resize(stages_.size());
    for(size_t i = 0; i < stages_.size(); ++i){
        stageNames_[i] = stages_[i].stageName;
    }


    data["stageNames"] = stageNames_;
    data["stageNameText_.fontName"] = stageNameText_.fontName;
    data["isSameFontStageIndex_"] = isSameFontStageIndex_;

    // 難易度関連---------------------------------
    {
        stageDifficulties_.resize(stages_.size());
        for(size_t i = 0; i < stages_.size(); ++i){
            stageDifficulties_[i] = stages_[i].difficulty;
        }
        data["stageDifficulties"] = stageDifficulties_;
        data["difficultyStarBasePos"] = difficultyStarBasePos_;
        data["starDrawRangeX"] = starDrawRangeX_;
        data["starSprite"] = difficultyStars_[0].ToJson(); // 全て同じなので1つだけ保存
    }

    JsonAdapter::Save("SelectScene/selectStageDrawer.json", data);
}

void SelectStageDrawer::UpdateFocusAnim(){

    focusAnimTimer_.Update();
    if(focusAnimTimer_.IsFinished()){

        //focusAnimForward_ = !focusAnimForward_;
        focusAnimTimer_.Reset();
    }
}

void SelectStageDrawer::DrawFocusAnim(){

    // 移動中は描画しない
    if(currentState_ == State::Move){
        return;
    }

    // 中央のステージ情報
    const Stage& center = stages_[focusIndex_];

    // フラグで補間先を切り替え
    float from = focusAnimForward_ ? focusAnimFrom_ : focusAnimTo_;
    float to = focusAnimForward_ ? focusAnimTo_ : focusAnimFrom_;
    float easedT = focusAnimTimer_.GetEase(focusAnimEasing_);
    // スケール補間
    float scale = from + (to - from) * easedT;
    // アルファ補間
    float alpha = std::clamp(1.0f - easedT, 0.0f, 1.0f);

    // フレーム
    focusAnimFrame_.transform.translate = center.translate;
    focusAnimFrame_.size = Vector2(center.size.x * scale, center.size.y * scale);
    focusAnimFrame_.color.w = alpha;
    // 描画
    focusAnimFrame_.Draw();

    // ステージ番号背景
    stageIndexBackAnim_.transform.translate = center.stageIndexBack.transform.translate;
    stageIndexBackAnim_.size = center.stageIndexBack.size * (scale + 0.08f);
    stageIndexBackAnim_.color = stageIndexBackColor_;
    stageIndexBackAnim_.color.w = alpha;
    stageIndexBackAnim_.Draw();
}

void SelectStageDrawer::DrawEndZoom(){

    // 進行度
    float t = endZoomTimer_.GetEase(endZoomEasing_);

    // フォーカス中のステージの処理
    {
        // サイズ補間処理を行う
        float scale = 1.0f + (endZoomToScale_ - 1.0f) * t;
        Vector2 size = Vector2(focusSize_.x * scale, focusSize_.y * scale);
        ApplyPoseToStage(stages_[focusIndex_], centerTranslate_, size);
        // 全ての描画処理
        stages_[focusIndex_].background.Draw();
        stages_[focusIndex_].frame.Draw();
        stages_[focusIndex_].stageIndexBack.Draw();
        stages_[focusIndex_].stageIndexText.Draw();
        if(stages_[focusIndex_].isClear){
            stages_[focusIndex_].achievementUI.Draw(); // クリア済みUI描画
        }
        for(auto& spite : stages_[focusIndex_].objects){

            spite.Draw();
        }
    }

    // 左ステージ処理
    if(focusIndex_ > 0){

        // サイズ補間処理を行う
        float scale = 1.0f - t;
        Vector2 size = Vector2(outSize_.x * scale, outSize_.y * scale);
        ApplyPoseToStage(stages_[focusIndex_ - 1], leftTranslate_, size);
        if(0.0f < scale){

            // 全ての描画処理
            stages_[focusIndex_ - 1].background.Draw();
            stages_[focusIndex_ - 1].frame.Draw();
            stages_[focusIndex_ - 1].stageIndexBack.Draw();
            stages_[focusIndex_ - 1].stageIndexText.Draw();
            if(stages_[focusIndex_ - 1].isClear){
                stages_[focusIndex_ - 1].achievementUI.Draw(); // クリア済みUI描画
            }
            for(auto& spite : stages_[focusIndex_ - 1].objects){

                spite.Draw();
            }
        }
    }
    // 右
    if(focusIndex_ + 1 < stages_.size()){

        float scale = 1.0f - t;
        Vector2 size = Vector2(outSize_.x * scale, outSize_.y * scale);
        ApplyPoseToStage(stages_[focusIndex_ + 1], rightTranslate_, size);
        if(0.0f < scale){

            // 全ての描画処理
            stages_[focusIndex_ + 1].background.Draw();
            stages_[focusIndex_ + 1].frame.Draw();
            stages_[focusIndex_ + 1].stageIndexBack.Draw();
            stages_[focusIndex_ + 1].stageIndexText.Draw();
            if(stages_[focusIndex_ + 1].isClear){
                stages_[focusIndex_ + 1].achievementUI.Draw(); // クリア済みUI描画
            }
            for(auto& spite : stages_[focusIndex_ + 1].objects){

                spite.Draw();
            }
        }
    }
}

void SelectStageDrawer::StartMoveToNext(uint32_t next){

    next = (std::min)(next, static_cast<uint32_t>(stages_.size()) - 1);
    // 同じ場所に補間できないようにする
    if(next == focusIndex_){
        return;
    }

    // 補間途中位置を開始点にする
    float t = (currentState_ == State::Move) ? moveTimer_.GetEase(moveEasing_) : 1.0f;
    animFrom_ = animFrom_ + (animTo_ - animFrom_) * t;
    animTo_ = static_cast<float>(next);
    focusIndex_ = next;

    // タイマーを初期化する
    moveTimer_.Reset();
    currentState_ = State::Move;
}

void SelectStageDrawer::ApplyPoseToStage(Stage& stage, const Vector2& center, const Vector2& size){

    // 座標、サイズを設定する
    stage.translate = center;
    stage.size = size;
    stage.frame.transform.translate = center;
    stage.frame.size = size;
    stage.background.transform.translate = center;
    stage.background.size = size;
    // ステージ番号はフレームY座標からオフセットをかける
    {
        const float scaleY = size.y / focusSize_.y;
        const float offsetY = stageIndexTextOffsetY_ * scaleY;
        stage.stageIndexText.transform.translate = Vector2(center.x, center.y + offsetY);
        stage.stageIndexText.fontSize = stageIndexTextSize_ * scaleY;
    }
    {
        const float scaleY = size.y / focusSize_.y;
        const float offsetY = stageIndexBackOffsetY_ * scaleY;
        stage.stageIndexBack.transform.translate = Vector2(center.x, center.y + offsetY);
        stage.stageIndexBack.size = stageIndexBackSize_ * scaleY;
    }
    //クリア済みUIは更にその上に描画させる
    {
        const float scaleY = size.y / focusSize_.y;
        const float offsetY = stageAchievementUIOffsetY_ * scaleY;
        stage.achievementUI.transform.translate = Vector2(center.x, center.y + offsetY);
        stage.achievementUI.size = stageAchievementUISize_ * scaleY;
    }

    // タイルを最大の正方形の最大サイズにする
    const float cols = static_cast<float>(stage.cols);
    const float rows = static_cast<float>(stage.rows);
    const float cell = (std::min)(size.x / cols, size.y / rows);

    // このステージのグリッドサイズ
    const float gridWidth = cell * cols * tileScale_;
    const float gridHeight = cell * rows * tileScale_;

    // フレーム内で中央に配置
    const float left = center.x - gridWidth * 0.5f;
    const float top = center.y - gridHeight * 0.5f;

    // 各タイルを配置
    for(size_t k = 0; k < stage.objects.size(); ++k){

        const Vector2  uv = stage.objectUVs[k];

        // uvをグリッドサイズに変換して左上オフセットを足す
        stage.objects[k].transform.translate = Vector2(left + uv.x * gridWidth, top + uv.y * gridHeight);
        stage.objects[k].size = cell;
    }
}

void SelectStageDrawer::PoseFromOffset(float offset, Vector2& outPos, Vector2& outSize){

    // 処理が終了している時は目標値を返す
    if(offset <= -1.0f){
        outPos = leftTranslate_;
        outSize = outSize_;
        return;
    }
    if(offset >= 1.0f){
        outPos = rightTranslate_;
        outSize = outSize_;
        return;
    }

    // 左から真ん中へ補間
    if(offset < 0.0f){

        float t = offset + 1.0f;
        outPos = MyMath::Lerp(leftTranslate_, centerTranslate_, t);
        outSize = MyMath::Lerp(outSize_, focusSize_, t);
    }
    // 真ん中から右へ補間する
    else{

        float t = offset;
        outPos = MyMath::Lerp(centerTranslate_, rightTranslate_, t);
        outSize = MyMath::Lerp(focusSize_, outSize_, t);
    }
}

void SelectStageDrawer::TriggerLeftArrowReact(){

    leftArrowReacting_ = true;
    leftArrowReactTimer_.duration = arrowAnimTimer_.duration * reactRate_;
    leftArrowReactTimer_.Reset();
}

void SelectStageDrawer::TriggerRightArrowReact(){

    rightArrowReacting_ = true;
    rightArrowReactTimer_.duration = arrowAnimTimer_.duration * reactRate_;
    rightArrowReactTimer_.Reset();
}

void SelectStageDrawer::UpdateDifficultyStar(){

    static float motionTimer = 0.0f;
    Vector2 minPos = difficultyStarBasePos_ - Vector2(starDrawRangeX_ * 0.5f, 0.0f);
    Vector2 maxPos = difficultyStarBasePos_ + Vector2(starDrawRangeX_ * 0.5f, 0.0f);

    for(size_t i = 0; i < difficultyStars_.size(); ++i){
        float t = (static_cast<float>(i) / static_cast<float>(difficultyStars_.size() - 1));

        difficultyStars_[i] = difficultyStars_[0];
        difficultyStars_[i].transform.translate = MyMath::Lerp(minPos, maxPos, t);

        // ステージの星の数だけ明るくする
        if(i < stages_[focusIndex_].difficulty){
            // 少しスケーリングを揺らします
            float sin = std::sin(motionTimer * 3.14f - 0.3f * i);
            difficultyStars_[i].transform.scale = Vector2(1.0f + sin * (sin > 0 ? 0.2f : -0.1f));
            // キラキラさせます
            difficultyStars_[i].color = MyMath::FloatColor(255, 198, 57, 255);
            if(sin < 0.0f){ difficultyStars_[i].color *= 1.0f + 5.0f * -sin; }
        } else{
            // 灰色にする
            difficultyStars_[i].color = Vector4(1.0f, 1.0f, 1.0f, 0.1f);
            difficultyStars_[i].transform.scale = Vector2(1.0f);
        }
    }

    motionTimer += ClockManager::DeltaTime();
}

void SelectStageDrawer::DrawActivate(float f){

    // 移動処理以外で処理しない
    if(currentState_ != State::Move){
        return;
    }

    // 進行度
    float easedT = moveTimer_.GetEase(moveEasing_);
    // 右からきたか左からか取得する
    bool movingRight = (animTo_ > animFrom_);

    // 出現させるステージのインデックスと補間対象を設定する
    uint32_t enterIndex = 0;
    Vector2 slotPos, spawnPos;
    // 右から補間させる
    if(movingRight){

        // 最後のステージのときは描画を何もしない
        if(focusIndex_ + 1 >= stages_.size()){
            return;
        }

        // 中心から右ステージの向き
        Vector2 direction = MyMath::Normalize(centerTranslate_ - rightTranslate_);
        float distance = MyMath::Length(centerTranslate_ - rightTranslate_);
        // 発生位置を設定
        spawnPos = Vector2(rightTranslate_.x - direction.x * distance,
            rightTranslate_.y - direction.y * distance);
        // 目標座標は右座標
        slotPos = rightTranslate_;

        // 移動するステージのインデックス
        enterIndex = static_cast<uint32_t>(std::floor(animTo_)) + 1;
        if(enterIndex >= stages_.size()){
            return;
        }
    }
    // 左から補間させる
    else{

        // 最初のステージの時は何もしない
        if(focusIndex_ == 0){
            return;
        }

        // 中心から左ステージの向き
        Vector2 direction = MyMath::Normalize(centerTranslate_ - leftTranslate_);
        float distance = MyMath::Length(centerTranslate_ - leftTranslate_);
        // 発生位置を設定
        spawnPos = Vector2(leftTranslate_.x - direction.x * distance,
            leftTranslate_.y - direction.y * distance);
        // 目標座標は左座標
        slotPos = leftTranslate_;

        // 移動するステージのインデックス
        enterIndex = static_cast<uint32_t>(std::floor(animTo_)) - 1;
        // 0未満のときは無効
        if(static_cast<int>(enterIndex) < 0){
            return;
        }
    }

    // 3ステージ描画中は処理しない
    float off = static_cast<float>(enterIndex) - f;
    if(off > -1.01f && off < 1.01f){
        return;
    }

    // 座標、サイズを補間
    Vector2 pos = MyMath::Lerp(spawnPos, slotPos, easedT);
    Vector2 size = MyMath::Lerp(Vector2(0.0f, 0.0f), outSize_, easedT);

    // ステージへ適用して描画
    ApplyPoseToStage(stages_[enterIndex], pos, size);

    // 背景描画
    stages_[enterIndex].background.Draw();
    // フレーム描画
    stages_[enterIndex].frame.Draw();
    // ステージ番号描画
    stages_[enterIndex].stageIndexBack.Draw();
    stages_[enterIndex].stageIndexText.Draw();
    // クリア済みUI描画
    if(stages_[enterIndex].isClear){
        stages_[enterIndex].achievementUI.Draw();
    }
    for(auto& sprite : stages_[enterIndex].objects){

        // ステージ描画
        sprite.Draw();
    }
}