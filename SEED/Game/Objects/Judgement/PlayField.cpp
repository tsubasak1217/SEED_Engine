#include "PlayField.h"
#include <Environment/Environment.h>
#include <SEED/Source/Manager/CameraManager/CameraManager.h>
#include <SEED/Source/SEED.h>


/////////////////////////////////////////////////////////////////////////
// コンストラクタ・デストラクタ
/////////////////////////////////////////////////////////////////////////
PlayField::PlayField(){
    // ゲームカメラの取得
    BaseCamera* gameCamera_ = CameraManager::GetInstance()->GetCamera("gameCamera");

    // プレイフィールドの4点を求める
    Vector2 center = kWindowCenter;
    kPlayFieldSizeY_ = float(kWindowSizeY) * 0.9f;
    kPlayFieldSizeX_ = kPlayFieldSizeY_;
    Vector2 size = { kPlayFieldSizeX_, kPlayFieldSizeY_ };

    // スクリーン上の四点を求める
    playFieldPointsScreen_[TOP] = center + Vector2(0.0f, -size.y * 0.5f);
    playFieldPointsScreen_[RIGHT] = center + Vector2(size.x * 0.5f, 0.0f);
    playFieldPointsScreen_[BOTTOM] = center + Vector2(0.0f, size.y * 0.5f);
    playFieldPointsScreen_[LEFT] = center + Vector2(-size.x * 0.5f, 0.0f);

    // ワールド上の四点を求める
    playFieldPointsWorld_[TOP] = gameCamera_->ToWorldPosition(playFieldPointsScreen_[TOP], farZ_);
    playFieldPointsWorld_[RIGHT] = gameCamera_->ToWorldPosition(playFieldPointsScreen_[RIGHT], nearZ_);
    playFieldPointsWorld_[BOTTOM] = gameCamera_->ToWorldPosition(playFieldPointsScreen_[BOTTOM], farZ_);
    playFieldPointsWorld_[LEFT] = gameCamera_->ToWorldPosition(playFieldPointsScreen_[LEFT], nearZ_);

    // 鍵盤の境界線の座標を求める
    for(int i = 0; i < kKeyCount_ + 1; i++){
        static float keyWidth = kPlayFieldSizeX_ / kKeyCount_;
        keyboardBorderPoints_[i] = playFieldPointsWorld_[RIGHT] +
            Vector3(keyWidth * i,0.0f,0.0f);
    }
}

PlayField::~PlayField(){
}

/////////////////////////////////////////////////////////////////////////
// インスタンスの取得
/////////////////////////////////////////////////////////////////////////
PlayField* PlayField::GetInstance(){
    if(!instance_){
        instance_ = new PlayField();
    }
    return instance_;
}

/////////////////////////////////////////////////////////////////////////
// 更新
/////////////////////////////////////////////////////////////////////////
void PlayField::Update(){
}


/////////////////////////////////////////////////////////////////////////
// 描画
/////////////////////////////////////////////////////////////////////////
void PlayField::Draw(){

    // 
}
