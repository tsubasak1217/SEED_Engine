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

    // すべてのステージを構築する
    BuildAllStage();
    // 初期化値設定
    // 最初のフォーカス先を設定
    focusIndex_ = std::min<uint32_t>(firstFocusStage, static_cast<uint32_t>(stages_.size()) - 1);
    animFrom_ = static_cast<float>(focusIndex_);
    animTo_ = static_cast<float>(focusIndex_);
    moveTimer_.Reset();
    currentState_ = State::Select;

    // json適応
    ApplyJson();
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

void SelectStageDrawer::Update() {

    if (currentState_ == State::Move) {

        // 補間中にのみタイマーを進める
        moveTimer_.Update();
        // 補間終了後選択可能にする
        if (moveTimer_.IsFinished()) {

            currentState_ = State::Select;
            animFrom_ = animTo_;
        }
    }
}

void SelectStageDrawer::Draw() {

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

        // フレーム描画
        stages_[i].frame.Draw();
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

        EnumAdapter<Easing::Type>::Combo("moveEasing", &moveEasing_);
    }
}

void SelectStageDrawer::ApplyJson() {

    nlohmann::json data;
    if (!JsonAdapter::LoadCheck("SelectScene/selectStageDrawer.json", data)) {
        return;
    }

    moveTimer_.duration = data.value("moveTimer_.duration", 0.32f);

    from_json(data["centerTranslate_"], centerTranslate_);
    from_json(data["leftTranslate_"], leftTranslate_);
    from_json(data["rightTranslate_"], rightTranslate_);
    from_json(data["focusSize_"], focusSize_);
    from_json(data["outSize_"], outSize_);

    moveEasing_ = EnumAdapter<Easing::Type>::FromString(data["moveEasing_"]).value();
}

void SelectStageDrawer::SaveJson() {

    nlohmann::json data;

    data["moveTimer_.duration"] = moveTimer_.duration;

    to_json(data["centerTranslate_"], centerTranslate_);
    to_json(data["leftTranslate_"], leftTranslate_);
    to_json(data["rightTranslate_"], rightTranslate_);
    to_json(data["focusSize_"], focusSize_);
    to_json(data["outSize_"], outSize_);

    data["moveEasing_"] = EnumAdapter<Easing::Type>::ToString(moveEasing_);

    JsonAdapter::Save("SelectScene/selectStageDrawer.json", data);
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
    stage.frame.translate = center;
    stage.frame.size = size;

    // タイル位置からサイズを求める
    Vector2 tileSize = Vector2(size.x / static_cast<float>(stage.cols),
        size.y / static_cast<float>(stage.rows));
    float left = center.x - size.x * 0.5f;
    float top = center.y - size.y * 0.5f;
    for (size_t k = 0; k < stage.objects.size(); ++k) {

        const uint32_t id = stage.objectIds[k];
        const Vector2 uv = stage.objectUVs[k];

        // 現在のフレームサイズから今のフレーム内の位置を設定する
        Vector2 cpos = Vector2(left + uv.x * size.x, top + uv.y * size.y);
        stage.objects[k].translate = cpos;
        stage.objects[k].size = tileSize;

        // 個別のサイズ設定
        if (static_cast<StageObjectType>(id) == StageObjectType::Player) {

            stage.objects[k].size = stage.objects[k].size * 0.8f;
        }
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
    // フレーム描画
    stages_[enterIndex].frame.Draw();
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
        stage.frame.translate = stage.translate;
        for (int r = 0; r < rows; ++r) {

            const int colsThis = static_cast<int>(grid[r].size());
            for (int c = 0; c < colsThis; ++c) {

                // CSVのインデックスからスプライトを作成する
                const int id = grid[r][c];
                // Noneは処理しない
                if (id == 0) {
                    continue;
                }

                // ステージ位置を記録
                stage.objectIds.push_back(id);
                stage.objectUVs.push_back(Vector2((c + 0.5f) / (float)cols, (r + 0.5f) / (float)rows));
                stage.objects.emplace_back(CreateTileSprite(id, Vector2(0.0f), Vector2(1.0f)));
            }
        }
        // 配列に追加
        stages_.push_back(std::move(stage));
    }
}

Sprite SelectStageDrawer::CreateTileSprite(uint32_t index,
    const Vector2& translate, const Vector2& size) {

    // スプライトをパスから作成
    Sprite sprite = Sprite(GetFileNameFromIndex(index));
    // 設定
    sprite.anchorPoint = 0.5f;
    sprite.translate = translate;
    sprite.size = size;

    // 個別のサイズ設定
    if (static_cast<StageObjectType>(index) == StageObjectType::Player) {

        sprite.size *= 0.8f;
    }
    return sprite;
}

std::string SelectStageDrawer::GetFileNameFromIndex(uint32_t index) const {

    // 番号からスプライト画像のパスを取得する
    switch (index) {
    case 1:  return "Scene_Game/StageObject/normalBlock.png";       // NormalBlock
    case 2:  return "Scene_Game/StageObject/crown.png";             // Goal
    case 3:  return "Scene_Game/StageObject/Player/PlayerBody.png"; // Player
    case 4:  return "Scene_Game/StageObject/frameRect.png";         // Warp
    case 5:  return "Scene_Game/StageObject/dottedLine.png";        // EmptyBlock
    case 6:  return "Scene_Game/StageObject/normalBlock.png";       // LaserLauncher
    }
    return "";
}