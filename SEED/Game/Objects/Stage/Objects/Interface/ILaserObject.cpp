#include "ILaserObject.h"

//============================================================================
//	ILaserObject classMethods
//============================================================================

void ILaserObject::SetSize(const Vector2& size) {

    // 初期化時のみサイズを設定
    if (!initSizeY_.has_value()) {

        initSizeY_ = size.y;
    }
    sprite_.size = size;
}