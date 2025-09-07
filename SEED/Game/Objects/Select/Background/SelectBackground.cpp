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
    drawer_.EnableAmbientPurpleFade(8, 320.0f, 4.0f, screenRange_);
}

void SelectBackground::Update() {

    // 更新処理
    drawer_.Update(kWindowCenter, 0.08f);
}

void SelectBackground::Draw() {

    // 六角形sを描画
    drawer_.Draw(false);
}