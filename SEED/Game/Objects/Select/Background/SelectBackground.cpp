#include "SelectBackground.h"

//============================================================================
//	include
//============================================================================
#include <Environment/Environment.h>

//============================================================================
//	SelectBackground classMethods
//============================================================================

void SelectBackground::Initialize() {

    // 画面サイズを設定
    screenRange_ = Range2D(Vector2(0.0f), kWindowSize);
    // 初期設定
    drawer_.SetActive(true);
    drawer_.SetDefaultColorAlpha(0.08f);
    drawer_.SetAnimaionMode(HexagonAnimaionMode::Radial);
    drawer_.SetBorder(screenRange_.min.x, LR::RIGHT, screenRange_);

    // ドット背景
    ceckerSprite_ = Sprite("Scene_Select/bgCheckerboard.png");
    ceckerSprite_.anchorPoint = 0.5f;
    ceckerSprite_.blendMode = BlendMode::ADD;
    // 画面全体に埋める
    ceckerSprite_.translate = kWindowCenter;
    ceckerSprite_.size = kWindowSize;
    ceckerSprite_.uvTransform = AffineMatrix(Vector3(160.0f, 144.0f, 1.0f), Vector3(0.0f), Vector3(0.0f));
    // 最初は描画しない
    ceckerSprite_.color.w = 0.0f;
    // タイマー初期化
    checkerAlphaTimer_.duration = 1.0f;
}

void SelectBackground::Update() {

    // 更新処理
    drawer_.Update(kWindowCenter, 0.08f);

    // ドット背景を更新
    if (!checkerAlphaTimer_.IsFinished()) {

        checkerAlphaTimer_.Update();
        ceckerSprite_.color.w = std::lerp(0.0f, 0.01f, checkerAlphaTimer_.GetProgress());
    }
}

void SelectBackground::Draw() {

    // ドット背景を描画
    ceckerSprite_.Draw();

    // 六角形sを描画
    drawer_.Draw();
}