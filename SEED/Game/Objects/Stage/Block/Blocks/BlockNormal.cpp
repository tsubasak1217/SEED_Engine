#include "BlockNormal.h"

//============================================================================
//	BlockNormal classMethods
//============================================================================

void BlockNormal::Initialize(const std::string& filename) {

    // スプライトの初期化
    sprite_ = Sprite(filename);
}

void BlockNormal::Update() {

    // ブロックの更新を行う
}

void BlockNormal::Draw() {

    // スプライトの描画
    sprite_.Draw();
}