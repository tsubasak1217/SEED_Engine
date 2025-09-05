#include "Laser.h"

//============================================================================
//	include
//============================================================================
#include <Game/Objects/Stage/Objects/Laser/Methods/LaserHelper.h>

//============================================================================
//	Laser classMethods
//============================================================================

void Laser::Initialize(const std::string& filename) {

    // スプライトの初期化
    sprite_ = Sprite("DefaultAssets/white1x1.png");
    sprite_.anchorPoint = Vector2(0.5f);
}

void Laser::SetDirection(DIRECTION4 direction) {

    // 向きの設定
    direction_ = direction;
    // 向きで回転を設定
    sprite_.rotate = LaserHelper::GetRotateFromDirection(direction);
}

void Laser::Update() {


}

void Laser::Draw() {

    // スプライトの描画
    sprite_.Draw();
}