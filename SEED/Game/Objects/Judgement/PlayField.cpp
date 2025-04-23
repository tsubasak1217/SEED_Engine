#include "PlayField.h"
#include <Environment/Environment.h>
#include <SEED/Source/Manager/CameraManager/CameraManager.h>
#include <SEED/Source/SEED.h>

///////////////////////////////////////////////////////////////////////////////
// static変数の初期化
///////////////////////////////////////////////////////////////////////////////
float PlayField::kPlayFieldSizeX_ = kPlayFieldSizeY_ = float(kWindowSizeY) * 0.975f;// プレイフィールドの幅
float PlayField::kPlayFieldSizeY_ = kPlayFieldSizeY_;// プレイフィールドの高さ
float PlayField::kKeyWidth_ = kPlayFieldSizeX_ / kKeyCount_;// 鍵盤の幅

/////////////////////////////////////////////////////////////////////////
// コンストラクタ・デストラクタ
/////////////////////////////////////////////////////////////////////////
PlayField::PlayField(){
    Initialize();
}

PlayField::~PlayField(){
}

/////////////////////////////////////////////////////////////////////////
// 初期化
/////////////////////////////////////////////////////////////////////////
void PlayField::Initialize(){
    // ゲームカメラの取得
    BaseCamera* gameCamera_ = CameraManager::GetInstance()->GetCamera("gameCamera");

    // プレイフィールドの4点を求める
    Vector2 center = kWindowCenter;
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
    float keyWidth = std::fabsf(playFieldPointsWorld_[RIGHT].x - playFieldPointsWorld_[LEFT].x) / kKeyCount_;
    for(int i = 0; i < kKeyCount_ + 1; i++){
        keyboardBorderPoints_[i] = playFieldPointsWorld_[LEFT] +
            Vector3(keyWidth * i, 0.0f, 0.0f);
    }

    // レーン描画矩形の座標決定
    for(int i = 0; i < kKeyCount_; i++){

        // 矩形をの頂点を計算
        Vector3 v[2][3];
        v[0][0] = playFieldPointsWorld_[TOP];
        v[0][1] = keyboardBorderPoints_[i + 1];
        v[0][2] = keyboardBorderPoints_[i];
        v[1][0] = playFieldPointsWorld_[BOTTOM];
        v[1][1] = keyboardBorderPoints_[i];
        v[1][2] = keyboardBorderPoints_[i + 1];

        // 矩形の頂点を代入
        for(int j = 0; j < 3; j++){
            lane_[0][i].localVertex[j] = v[0][j];
            lane_[1][i].localVertex[j] = v[1][j];
            laneAnswer_[0][i].tri.localVertex[j] = v[0][j];
            laneAnswer_[1][i].tri.localVertex[j] = v[1][j];
            laneAnswer_[0][i].evalutionPolygon.localVertex[j] = v[0][j];
            laneAnswer_[1][i].evalutionPolygon.localVertex[j] = v[1][j];
            laneAnswer_[0][i].baseScale = 1.05f;// 押したときのリアクション用の矩形は少し大きくする
            laneAnswer_[1][i].baseScale = 1.05f;
        }

        // 色を設定
        lane_[0][i].color = { 1.0f, 1.0f, 1.0f, 0.1f };
        lane_[1][i].color = { 1.0f, 1.0f, 1.0f, 0.1f };
        laneAnswer_[0][i].tri.color = { 1.0f, 1.0f, 1.0f, 0.0f };
        laneAnswer_[1][i].tri.color = { 1.0f, 1.0f, 1.0f, 0.0f };
        laneAnswer_[0][i].evalutionPolygon.color = { 1.0f, 1.0f, 1.0f, 0.0f };
        laneAnswer_[1][i].evalutionPolygon.color = { 1.0f, 1.0f, 1.0f, 0.0f };

        // レーンの境界線の矩形の座標を決定
        laneBorderLine_[0][i] = lane_[0][i];
        laneBorderLine_[1][i] = lane_[1][i];
        laneBorderLine_[0][i].localVertex[1].x -= keyWidth * 0.5f;
        laneBorderLine_[0][i].localVertex[2].x -= keyWidth * 0.5f;
        laneBorderLine_[1][i].localVertex[1].x -= keyWidth * 0.5f;
        laneBorderLine_[1][i].localVertex[2].x -= keyWidth * 0.5f;


        // レーンの境界線の周りのオーラ的なやつの矩形の座標を決定
        laneBorderLineAura_[0][i] = laneBorderLine_[0][i];
        laneBorderLineAura_[1][i] = laneBorderLine_[1][i];

        // 画像を設定
        laneAnswer_[0][i].tri.GH = TextureManager::LoadTexture("PlayField/gradation.png");
        laneAnswer_[1][i].tri.GH = TextureManager::LoadTexture("PlayField/gradation.png");
        laneBorderLine_[0][i].GH = TextureManager::LoadTexture("PlayField/borderLine.png");
        laneBorderLine_[1][i].GH = TextureManager::LoadTexture("PlayField/borderLine.png");
        laneBorderLineAura_[0][i].GH = TextureManager::LoadTexture("PlayField/lineAura.png");
        laneBorderLineAura_[1][i].GH = TextureManager::LoadTexture("PlayField/lineAura.png");

        // blendModeを設定
        laneAnswer_[0][i].tri.blendMode = BlendMode::ADD;
        laneAnswer_[1][i].tri.blendMode = BlendMode::ADD;
        laneBorderLine_[0][i].blendMode = BlendMode::ADD;
        laneBorderLine_[1][i].blendMode = BlendMode::ADD;
        laneBorderLineAura_[0][i].blendMode = BlendMode::ADD;
        laneBorderLineAura_[1][i].blendMode = BlendMode::ADD;

        // 要素が1つだけ多いので右端だけ手動で調整
        if(i == kKeyCount_ - 1){
            laneBorderLine_[0][i + 1] = lane_[0][i];
            laneBorderLine_[1][i + 1] = lane_[1][i];
            laneBorderLine_[0][i + 1].localVertex[1].x += keyWidth * 0.5f;
            laneBorderLine_[0][i + 1].localVertex[2].x += keyWidth * 0.5f;
            laneBorderLine_[1][i + 1].localVertex[1].x += keyWidth * 0.5f;
            laneBorderLine_[1][i + 1].localVertex[2].x += keyWidth * 0.5f;
            laneBorderLineAura_[0][i + 1] = laneBorderLine_[0][i + 1];
            laneBorderLineAura_[1][i + 1] = laneBorderLine_[1][i + 1];
            laneBorderLine_[0][i + 1].GH = TextureManager::LoadTexture("PlayField/borderLine.png");
            laneBorderLine_[1][i + 1].GH = TextureManager::LoadTexture("PlayField/borderLine.png");
            laneBorderLineAura_[0][i + 1].GH = TextureManager::LoadTexture("PlayField/lineAura.png");
            laneBorderLineAura_[1][i + 1].GH = TextureManager::LoadTexture("PlayField/lineAura.png");
            laneBorderLine_[0][i + 1].blendMode = BlendMode::ADD;
            laneBorderLine_[1][i + 1].blendMode = BlendMode::ADD;
            laneBorderLineAura_[0][i + 1].blendMode = BlendMode::ADD;
            laneBorderLineAura_[1][i + 1].blendMode = BlendMode::ADD;
        }
    }
}


/////////////////////////////////////////////////////////////////////////
// 更新
/////////////////////////////////////////////////////////////////////////
void PlayField::Update(){

    for(auto answerQuadArray : laneAnswer_){
        for(auto answerQuad : answerQuadArray){
            answerQuad.Update();
        }
    }
}


/////////////////////////////////////////////////////////////////////////
// 描画
/////////////////////////////////////////////////////////////////////////
void PlayField::Draw(){

    // レーン
    for(int i = 0; i < 2; i++){
        for(int j = 0; j < lane_[i].size(); j++){
            SEED::DrawTriangle(lane_[i][j]);
        }
    }


    // レーンのリアクション
    for(int i = 0; i < 2; i++){
        for(int j = 0; j < laneAnswer_[i].size(); j++){
            laneAnswer_[i][j].Draw();
        }
    }

    // レーンの境界線
    for(int i = 0; i < 2; i++){
        for(int j = 0; j < laneBorderLine_[i].size(); j++){
            SEED::DrawTriangle(laneBorderLine_[i][j]);
        }
    }

    // レーンの境界線のオーラ
    for(int i = 0; i < 2; i++){
        for(int j = 0; j < laneBorderLineAura_[i].size(); j++){
            SEED::DrawTriangle(laneBorderLineAura_[i][j]);
        }
    }

}


/////////////////////////////////////////////////////////////////////////
// レーンの押下状態を設定
/////////////////////////////////////////////////////////////////////////
void PlayField::SetEvalution(LaneBit laneBit, UpDown layer, const Vector4& color){

    // ビットから押されているレーンを取得(5鍵の部分のみ)
    std::vector<int32_t> lanes;
    for(int i = 0; i < kKeyCount_; i++){
        if(laneBit & (1 << i)){
            lanes.push_back(i);
        }
    }

    // レーンの押下状態を設定
    for(auto& lane : lanes){
        // ノーツを拾っている場合
        if(color != Vector4(0.0f, 0.0f, 0.0f, 0.0f)){
            laneAnswer_[(int)layer][lane].evalutionPolygon.color = color;
            laneAnswer_[(int)layer][lane].isTapNote = true;
        }
    }
}

void PlayField::SetLanePressed(int32_t lane, const Vector4& color){
    // レーンの押下状態を設定
    laneAnswer_[0][lane].isTrigger = true;
    laneAnswer_[1][lane].isTrigger = true;
    // 押下状態を更新
    laneAnswer_[0][lane].isPress = true;
    laneAnswer_[1][lane].isPress = true;
    // 押下状態を解除
    laneAnswer_[0][lane].isRelease = false;
    laneAnswer_[1][lane].isRelease = false;
    // 色を設定
    laneAnswer_[0][lane].tri.color = color;
    laneAnswer_[1][lane].tri.color = color;
}

void PlayField::SetLaneReleased(int32_t lane){

    // レーンの押下状態を解除
    laneAnswer_[0][lane].isRelease = true;
    laneAnswer_[1][lane].isRelease = true;
    // 押下状態を更新
    laneAnswer_[0][lane].isPress = false;
    laneAnswer_[1][lane].isPress = false;
    // 押下状態を解除
    laneAnswer_[0][lane].isTrigger = false;
    laneAnswer_[1][lane].isTrigger = false;
}
