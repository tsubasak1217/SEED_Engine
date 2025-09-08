#include "SelectStageDrawer.h"

//============================================================================
//	include
//============================================================================
#include <SEED/Lib/JsonAdapter/JsonAdapter.h>
#include <SEED/Lib/MagicEnumAdapter/EnumAdapter.h>
#include <Game/Objects/Stage/Enum/StageObjectType.h>
#include <Game/Objects/Select/Methods/SelectStageBuilder.h>

// imgui
#include <SEED/Source/Manager/ImGuiManager/ImGuiManager.h>

//============================================================================
//	SelectStageDrawer classMethods
//============================================================================

void SelectStageDrawer::Initialize(uint32_t firstFocusStage) {

    // json適応
    ApplyJson();

    // すべてのステージを構築する
    BuildAllStage();
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
}

void SelectStageDrawer::SetNextFocus() {

    // 次のステージへフォーカスさせる
    if (stages_.size() <= focusIndex_ + 1) {
        return;
    }
    StartMoveToNext(focusIndex_ + 1);
}

void SelectStageDrawer::SetPrevFocus() {

    // 1つ前のステージをフォーカスさせる
    if (focusIndex_ == 0) {
        return;
    }
    StartMoveToNext(focusIndex_ - 1);
}

void SelectStageDrawer::SetEndFocus() {

    // なにか処理中でなければステージの決定にする
    if (currentState_ != State::Select) {
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
}

void SelectStageDrawer::Update() {

    // 次のシーンに進ませるので処理しない
    if (stageInfo_.isNextScene) {
        return;
    }

    switch (currentState_) {
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
        if (moveTimer_.IsFinished()) {

            currentState_ = State::Select;
            animFrom_ = animTo_;
        }

        // 動いているときはフォーカスタイマーをリセットする
        focusAnimTimer_.Reset();
        break;
    case SelectStageDrawer::State::End:

        // 決定処理
        switch (endPhase_) {
        case SelectStageDrawer::EndPhase::Decide:

            // 2倍速フォーカス
            focusAnimTimer_.Update();
            if (focusAnimTimer_.IsFinished()) {

                // 終了後値を元に戻してリセットしてズーム処理に移る
                focusAnimTimer_.duration = focusAnimBaseDuration_;
                endZoomTimer_.Reset();
                endPhase_ = EndPhase::Zoom;
            }
            break;
        case SelectStageDrawer::EndPhase::Zoom:

            // ズーム処理を進める
            endZoomTimer_.Update();

            if (endZoomTimer_.IsFinishedNow()) {

                // 次のシーンに進める
                stageInfo_.isNextScene = true;
            }
            break;
        }
        break;
    }
}

void SelectStageDrawer::Draw() {

    // ズーム処理中
    if (currentState_ == State::End &&
        endPhase_ == EndPhase::Zoom) {

        DrawEndZoom();
        return;
    }

    // 各ステージを描画する
    float f = (currentState_ == State::Move)
        ? animFrom_ + (animTo_ - animFrom_) * moveTimer_.GetEase(moveEasing_)
        : static_cast<float>(focusIndex_);
    for (size_t i = 0; i < stages_.size(); ++i) {

        float off = static_cast<float>(i) - f;
        if (off < -1.01f || off > 1.01f) {
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
        // ステージ描画
        for (auto& sprite : stages_[i].objects) {

            sprite.Draw();
        }
    }

    // 新しく表示するステージを補間しながら描画する
    DrawActivate(f);
}

void SelectStageDrawer::Edit() {

    if (ImGui::Button("Save Json")) {

        SaveJson();
    }
    {
        ImGui::DragFloat("moveDuration", &moveTimer_.duration, 0.01f);
        ImGui::DragFloat2("centerTranslate", &centerTranslate_.x, 1.0f);
        ImGui::DragFloat2("leftTranslate", &leftTranslate_.x, 1.0f);
        ImGui::DragFloat2("rightTranslate", &rightTranslate_.x, 1.0f);

        ImGui::DragFloat2("focusSize", &focusSize_.x, 1.0f);
        ImGui::DragFloat2("outSize", &outSize_.x, 1.0f);
        ImGui::DragFloat("tileScale", &tileScale_, 0.01f);
        ImGui::DragFloat("stageIndexTextOffsetY", &stageIndexTextOffsetY_, 1.0f);
        ImGui::DragFloat("stageIndexBackOffsetY_", &stageIndexBackOffsetY_, 1.0f);

        if (ImGui::DragFloat("stageIndexBackSize", &stageIndexBackSize_)) {

            // 全てのフレームに適応
            for (auto& stage : stages_) {

                stage.stageIndexBack.size = stageIndexBackSize_;
            }
        }
        if (ImGui::DragFloat("stageIndexTextSize", &stageIndexTextSize_)) {

            // 全てのフレームに適応
            for (auto& stage : stages_) {

                stage.stageIndexText.fontSize = stageIndexTextSize_;
            }
        }
        if (ImGui::ColorEdit4("stageIndexTextColor", &stageIndexTextColor_.x)) {

            // 全てのフレームに適応
            for (auto& stage : stages_) {

                stage.stageIndexText.color = stageIndexTextColor_;
            }
        }
        if (ImGui::ColorEdit4("stageIndexBackColor", &stageIndexBackColor_.x)) {

            // 全てのフレームに適応
            for (auto& stage : stages_) {

                stage.stageIndexBack.color = stageIndexBackColor_;
            }
        }
        if (ImGui::ColorEdit4("frameColor", &frameColor_.x)) {

            // 全てのフレームに適応
            for (auto& stage : stages_) {

                stage.frame.color = frameColor_;
            }
        }
        if (ImGui::ColorEdit4("backgroundColor", &backgroundColor_.x)) {

            // 全てのフレームに適応
            for (auto& stage : stages_) {

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

        if (ImGui::Button("SetEndFocus")) {

            SetEndFocus();
        }

        ImGui::DragFloat("endZoomDuration", &endZoomTimer_.duration, 0.01f);
        ImGui::DragFloat("endZoomToScale", &endZoomToScale_, 0.01f);
        EnumAdapter<Easing::Type>::Combo("endZoomEasing", &endZoomEasing_);
    }
}

void SelectStageDrawer::ApplyJson() {

    nlohmann::json data;
    if (!JsonAdapter::LoadCheck("SelectScene/selectStageDrawer.json", data)) {
        return;
    }

    moveTimer_.duration = data.value("moveTimer_.duration", 0.32f);
    tileScale_ = data.value("tileScale_", 1.0f);
    stageIndexTextOffsetY_ = data.value("stageIndexTextOffsetY_", 128.0f);
    stageIndexBackOffsetY_ = data.value("stageIndexBackOffsetY_", 128.0f);
    stageIndexBackSize_ = data.value("stageIndexBackSize_", 128.0f);
    stageIndexTextSize_ = data.value("stageIndexTextSize_", 128.0f);

    from_json(data["centerTranslate_"], centerTranslate_);
    from_json(data["leftTranslate_"], leftTranslate_);
    from_json(data["rightTranslate_"], rightTranslate_);
    from_json(data["focusSize_"], focusSize_);
    from_json(data["outSize_"], outSize_);
    from_json(data.value("stageIndexTextColor_", nlohmann::json()), stageIndexTextColor_);
    from_json(data.value("stageIndexBackColor_", nlohmann::json()), stageIndexBackColor_);
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
}

void SelectStageDrawer::SaveJson() {

    nlohmann::json data;

    data["moveTimer_.duration"] = moveTimer_.duration;
    data["tileScale_"] = tileScale_;
    data["stageIndexTextOffsetY_"] = stageIndexTextOffsetY_;
    data["stageIndexBackOffsetY_"] = stageIndexBackOffsetY_;

    to_json(data["centerTranslate_"], centerTranslate_);
    to_json(data["leftTranslate_"], leftTranslate_);
    to_json(data["rightTranslate_"], rightTranslate_);
    to_json(data["focusSize_"], focusSize_);
    to_json(data["outSize_"], outSize_);
    to_json(data["stageIndexTextColor_"], stageIndexTextColor_);
    to_json(data["stageIndexBackColor_"], stageIndexBackColor_);
    to_json(data["frameColor_"], frameColor_);
    to_json(data["backgroundColor_"], backgroundColor_);
    to_json(data["stageIndexBackSize_"], stageIndexBackSize_);
    to_json(data["stageIndexTextSize_"], stageIndexTextSize_);

    data["moveEasing_"] = EnumAdapter<Easing::Type>::ToString(moveEasing_);

    data["focusAnimDuration_"] = focusAnimTimer_.duration;
    data["focusAnimFrom_"] = focusAnimFrom_;
    data["focusAnimTo_"] = focusAnimTo_;
    data["focusAnimEasing_"] = EnumAdapter<Easing::Type>::ToString(focusAnimEasing_);

    data["endZoomTimer_.duration"] = endZoomTimer_.duration;
    data["endZoomToScale_"] = endZoomToScale_;
    data["endZoomEasing_"] = EnumAdapter<Easing::Type>::ToString(endZoomEasing_);

    JsonAdapter::Save("SelectScene/selectStageDrawer.json", data);
}

void SelectStageDrawer::UpdateFocusAnim() {

    focusAnimTimer_.Update();
    if (focusAnimTimer_.IsFinished()) {

        //focusAnimForward_ = !focusAnimForward_;
        focusAnimTimer_.Reset();
    }
}

void SelectStageDrawer::DrawFocusAnim() {

    // 移動中は描画しない
    if (currentState_ == State::Move) {
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

void SelectStageDrawer::DrawEndZoom() {

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
        for (auto& spite : stages_[focusIndex_].objects) {

            spite.Draw();
        }
    }

    // 左ステージ処理
    if (focusIndex_ > 0) {

        // サイズ補間処理を行う
        float scale = 1.0f - t;
        Vector2 size = Vector2(outSize_.x * scale, outSize_.y * scale);
        ApplyPoseToStage(stages_[focusIndex_ - 1], leftTranslate_, size);
        if (0.0f < scale) {

            // 全ての描画処理
            stages_[focusIndex_ - 1].background.Draw();
            stages_[focusIndex_ - 1].frame.Draw();
            stages_[focusIndex_ - 1].stageIndexBack.Draw();
            stages_[focusIndex_ - 1].stageIndexText.Draw();
            for (auto& spite : stages_[focusIndex_ - 1].objects) {

                spite.Draw();
            }
        }
    }
    // 右
    if (focusIndex_ + 1 < stages_.size()) {

        float scale = 1.0f - t;
        Vector2 size = Vector2(outSize_.x * scale, outSize_.y * scale);
        ApplyPoseToStage(stages_[focusIndex_ + 1], rightTranslate_, size);
        if (0.0f < scale) {

            // 全ての描画処理
            stages_[focusIndex_ + 1].background.Draw();
            stages_[focusIndex_ + 1].frame.Draw();
            stages_[focusIndex_ + 1].stageIndexBack.Draw();
            stages_[focusIndex_ + 1].stageIndexText.Draw();
            for (auto& spite : stages_[focusIndex_ + 1].objects) {

                spite.Draw();
            }
        }
    }
}

void SelectStageDrawer::StartMoveToNext(uint32_t next) {

    next = (std::min)(next, static_cast<uint32_t>(stages_.size()) - 1);
    // 同じ場所に補間できないようにする
    if (next == focusIndex_) {
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

void SelectStageDrawer::ApplyPoseToStage(Stage& stage, const Vector2& center, const Vector2& size) {

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
    for (size_t k = 0; k < stage.objects.size(); ++k) {

        const Vector2  uv = stage.objectUVs[k];

        // uvをグリッドサイズに変換して左上オフセットを足す
        stage.objects[k].transform.translate = Vector2(left + uv.x * gridWidth, top + uv.y * gridHeight);
        stage.objects[k].size = cell;
    }
}

void SelectStageDrawer::PoseFromOffset(float offset, Vector2& outPos, Vector2& outSize) {

    // 処理が終了している時は目標値を返す
    if (offset <= -1.0f) {
        outPos = leftTranslate_;
        outSize = outSize_;
        return;
    }
    if (offset >= 1.0f) {
        outPos = rightTranslate_;
        outSize = outSize_;
        return;
    }

    // 左から真ん中へ補間
    if (offset < 0.0f) {

        float t = offset + 1.0f;
        outPos = MyMath::Lerp(leftTranslate_, centerTranslate_, t);
        outSize = MyMath::Lerp(outSize_, focusSize_, t);
    }
    // 真ん中から右へ補間する
    else {

        float t = offset;
        outPos = MyMath::Lerp(centerTranslate_, rightTranslate_, t);
        outSize = MyMath::Lerp(focusSize_, outSize_, t);
    }
}

void SelectStageDrawer::DrawActivate(float f) {

    // 移動処理以外で処理しない
    if (currentState_ != State::Move) {
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
    if (movingRight) {

        // 最後のステージのときは描画を何もしない
        if (focusIndex_ + 1 >= stages_.size()) {
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
        if (enterIndex >= stages_.size()) {
            return;
        }
    }
    // 左から補間させる
    else {

        // 最初のステージの時は何もしない
        if (focusIndex_ == 0) {
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
        if (static_cast<int>(enterIndex) < 0) {
            return;
        }
    }

    // 3ステージ描画中は処理しない
    float off = static_cast<float>(enterIndex) - f;
    if (off > -1.01f && off < 1.01f) {
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
    for (auto& sprite : stages_[enterIndex].objects) {

        // ステージ描画
        sprite.Draw();
    }
}

void SelectStageDrawer::BuildAllStage() {

    // 全てのステージをCSVファイルを基に構築する
    stages_.clear();

    // ファイル名をすべて取得する
    auto csvFiles = SelectStageBuilder::CollectStageCSV("Resources/Stage");

    // 1番左の表示
    float cursorX = leftTranslate_.x;
    maxStageCount_ = static_cast<uint32_t>(csvFiles.size());
    for (uint32_t index = 0; index < csvFiles.size(); ++index) {

        // CSVファイルを読み込む
        auto grid = SelectStageBuilder::LoadCSV(csvFiles[index]);
        if (grid.empty()) {
            continue;
        }

        uint32_t warpIndex = 0;
        const int rows = static_cast<int>(grid.size());
        const int cols = static_cast<int>(grid.front().size());

        // このステージ描画情報
        Stage stage;
        stage.index = index;
        stage.rows = rows;
        stage.cols = cols;
        // 全体のサイズ
        stage.size = outSize_;
        stage.translate = Vector2(cursorX + stage.size.x * 0.5f, leftTranslate_.y);

        // フレーム描画初期化
        // フレームは全体のサイズで設定する
        stage.frame = Sprite("Scene_Select/selectStageRect.png");
        stage.frame.size = stage.size;
        stage.frame.anchorPoint = 0.5f;
        stage.frame.transform.translate = stage.translate;
        stage.frame.color = frameColor_;
        stage.frame.isApplyViewMat = false;
        // 背景描画初期化
        // 背景は全体のサイズで設定する
        stage.background = Sprite("Scene_Select/bgCheckerboard.png");
        stage.background.size = stage.size;
        stage.background.anchorPoint = 0.5f;
        stage.background.transform.translate = stage.translate;
        stage.background.color = backgroundColor_;
        stage.background.uvTransform.scale = Vector2(80.0f, 45.0f);
        stage.background.isApplyViewMat = false;
        // ステージ番号背景
        stage.stageIndexBack = Sprite("Scene_Select/hexagonDesign.png");
        stage.stageIndexBack.size = stageIndexBackSize_;
        stage.stageIndexBack.anchorPoint = 0.5f;
        stage.stageIndexBack.color = stageIndexBackColor_;
        stage.stageIndexBack.isApplyViewMat = false;
        // ステージ番号
        stage.stageIndexText = TextBox2D(std::to_string(index + 1));
        stage.stageIndexText.SetFont("Game/x10y12pxDonguriDuel.ttf");
        stage.stageIndexText.fontSize = stageIndexTextSize_;
        stage.stageIndexText.anchorPos = Vector2(0.5f, 0.0f);
        stage.stageIndexText.glyphSpacing = 0.0f;
        stage.stageIndexText.color = stageIndexTextColor_;
        stage.stageIndexText.size = 256.0f;
        stage.stageIndexText.textBoxVisible = false;
        stage.stageIndexText.isApplyViewMat = false;
        for (int r = 0; r < rows; ++r) {

            const int colsThis = static_cast<int>(grid[r].size());
            for (int c = 0; c < colsThis; ++c) {

                // CSVのインデックスからスプライトを作成する
                const int id = grid[r][c];
                // Noneは処理しない
                if (static_cast<StageObjectType>(id) == StageObjectType::None) {
                    continue;
                }

                // ステージ位置を記録
                stage.objectIds.push_back(id);
                stage.objectUVs.push_back(Vector2((c + 0.5f) / static_cast<float>(cols),
                    (r + 0.5f) / static_cast<float>(rows)));
                stage.objects.emplace_back(CreateTileSprite(id, Vector2(0.0f), Vector2(1.0f), warpIndex));

                // インデックスを進める
                if (static_cast<StageObjectType>(id) == StageObjectType::Warp) {
                    ++warpIndex;
                }
            }
        }
        // 配列に追加
        stages_.push_back(std::move(stage));
    }
}

Sprite SelectStageDrawer::CreateTileSprite(uint32_t index,
    const Vector2& translate, const Vector2& size, uint32_t warpIndex) {

    // スプライトをパスから作成
    Sprite sprite = Sprite(GetFileNameFromIndex(index, warpIndex));
    // 設定
    sprite.anchorPoint = 0.5f;
    sprite.transform.translate = translate;
    sprite.size = size;
    sprite.isApplyViewMat = false;

    // 個別のサイズ設定
    if (static_cast<StageObjectType>(index) == StageObjectType::Player) {

        sprite.size *= 0.8f;
    }
    return sprite;
}

std::string SelectStageDrawer::GetFileNameFromIndex(uint32_t index, uint32_t warpIndex) const {

    // 番号からスプライト画像のパスを取得する
    switch (index) {
    case 1:  return "Scene_Select/stageBlockNormal.png";   // NormalBlock
    case 2:  return "Scene_Select/stageGoal.png";          // Goal
    case 3:  return "Scene_Select/stagePlayer.png";        // Player
    case 5:  return "Scene_Select/stageEmptyBlock.png";    // EmptyBlock
    case 6:  return "Scene_Select/stageLaserLauncher.png"; // LaserLauncher
    case 4: {

        // Warpはインデックスに応じて使用する画像を変更する
        std::string path = "Scene_Select/stageWarp" + std::to_string(warpIndex) + ".png";
        return path;
    }
    }
    return "";
}