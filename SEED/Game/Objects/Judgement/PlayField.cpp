#include "PlayField.h"
#include <Environment/Environment.h>
#include <SEED/Source/Manager/CameraManager/CameraManager.h>
#include <Game/Objects/Notes/NotesData.h>
#include <Game/Objects/Judgement/Judgement.h>
#include <SEED/Lib/Functions/MyFunc/Easing.h>

///////////////////////////////////////////////////////////////////////////////
// static変数の初期化
///////////////////////////////////////////////////////////////////////////////
PlayField* PlayField::instance_ = nullptr;
float PlayField::kPlayFieldSizeX_ = kPlayFieldSizeY_ = float(kWindowSizeY) * 0.975f;// プレイフィールドの幅
float PlayField::kPlayFieldSizeY_ = kPlayFieldSizeY_;// プレイフィールドの高さ
float PlayField::kKeyWidth_ = kPlayFieldSizeX_ / kKeyCount_;// 鍵盤の幅

/////////////////////////////////////////////////////////////////////////
// コンストラクタ・デストラクタ・インスタンス取得
/////////////////////////////////////////////////////////////////////////
PlayField::PlayField(){
    Initialize();
}

PlayField* PlayField::GetInstance(){
    if(!instance_){
        instance_ = new PlayField();
    }
    return instance_;
}

PlayField::~PlayField(){
}

/////////////////////////////////////////////////////////////////////////
// 初期化
/////////////////////////////////////////////////////////////////////////
void PlayField::Initialize(){
    // ゲームカメラの取得
    BaseCamera* gameCamera_ = SEED::GetCamera("gameCamera");

    farZ_ = 600.0f;// プレイフィールドの奥行き
    nearZ_ = 50.0f;// プレイフィールドの手前

    // プレイフィールドの4点を求める
    Vector2 center = kWindowCenter;
    Vector2 size = { kPlayFieldSizeX_, kPlayFieldSizeY_ };

    // 
    static Vector3 layerOffset = { 0.0f,0.0f,-0.001f };


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
    keyWidthWorld_ = std::fabsf(playFieldPointsWorld_[RIGHT].x - playFieldPointsWorld_[LEFT].x) / kKeyCount_;
    for(int i = 0; i < kKeyCount_ + 1; i++){
        keyboardBorderPoints_[i] = playFieldPointsWorld_[LEFT] +
            Vector3(keyWidthWorld_ * i, 0.0f, 0.0f);
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
        lane_[0][i].color = { 0.0f, 0.0f, 0.0f, 0.8f };
        lane_[1][i].color = { 0.0f, 0.0f, 0.0f, 0.8f };
        laneAnswer_[0][i].tri.color = { 1.0f, 1.0f, 1.0f, 0.0f };
        laneAnswer_[1][i].tri.color = { 1.0f, 1.0f, 1.0f, 0.0f };
        laneAnswer_[0][i].evalutionPolygon.color = { 1.0f, 1.0f, 1.0f, 0.0f };
        laneAnswer_[1][i].evalutionPolygon.color = { 1.0f, 1.0f, 1.0f, 0.0f };

        // レーンの境界線の矩形の座標を決定
        laneBorderLine_[0][i] = lane_[0][i];
        laneBorderLine_[1][i] = lane_[1][i];
        laneBorderLine_[0][i].color = { 1.0f,1.0f,1.0f,0.5f };
        laneBorderLine_[1][i].color = { 1.0f,1.0f,1.0f,0.5f };
        laneBorderLine_[0][i].localVertex[1].x -= keyWidthWorld_ * 0.5f;
        laneBorderLine_[0][i].localVertex[2].x -= keyWidthWorld_ * 0.5f;
        laneBorderLine_[1][i].localVertex[1].x -= keyWidthWorld_ * 0.5f;
        laneBorderLine_[1][i].localVertex[2].x -= keyWidthWorld_ * 0.5f;


        // レーンの境界線の周りのオーラ的なやつの矩形の座標を決定
        laneBorderLineAura_[0][i] = laneBorderLine_[0][i];
        laneBorderLineAura_[1][i] = laneBorderLine_[1][i];

        // 画像を設定
        laneAnswer_[0][i].tri.GH = TextureManager::LoadTexture("PlayField/gradation.png");
        laneAnswer_[1][i].tri.GH = TextureManager::LoadTexture("PlayField/gradation.png");
        laneAnswer_[0][i].evalutionPolygon.GH = TextureManager::LoadTexture("PlayField/gradation.png");
        laneAnswer_[1][i].evalutionPolygon.GH = TextureManager::LoadTexture("PlayField/gradation.png");
        laneBorderLine_[0][i].GH = TextureManager::LoadTexture("PlayField/borderLine.png");
        laneBorderLine_[1][i].GH = TextureManager::LoadTexture("PlayField/borderLine.png");
        laneBorderLineAura_[0][i].GH = TextureManager::LoadTexture("PlayField/lineAura.png");
        laneBorderLineAura_[1][i].GH = TextureManager::LoadTexture("PlayField/lineAura.png");

        // blendModeを設定
        laneAnswer_[0][i].tri.blendMode = BlendMode::ADD;
        laneAnswer_[1][i].tri.blendMode = BlendMode::ADD;
        laneAnswer_[0][i].evalutionPolygon.blendMode = BlendMode::ADD;
        laneAnswer_[1][i].evalutionPolygon.blendMode = BlendMode::ADD;
        laneBorderLine_[0][i].blendMode = BlendMode::ADD;
        laneBorderLine_[1][i].blendMode = BlendMode::ADD;
        laneBorderLineAura_[0][i].blendMode = BlendMode::ADD;
        laneBorderLineAura_[1][i].blendMode = BlendMode::ADD;

        // 要素が1つだけ多いので右端だけ手動で調整
        if(i == kKeyCount_ - 1){
            laneBorderLine_[0][i + 1] = lane_[0][i];
            laneBorderLine_[1][i + 1] = lane_[1][i];
            laneBorderLine_[0][i + 1].color = { 1.0f,1.0f,1.0f,0.5f };
            laneBorderLine_[1][i + 1].color = { 1.0f,1.0f,1.0f,0.5f };
            laneBorderLine_[0][i + 1].localVertex[1].x += keyWidthWorld_ * 0.5f;
            laneBorderLine_[0][i + 1].localVertex[2].x += keyWidthWorld_ * 0.5f;
            laneBorderLine_[1][i + 1].localVertex[1].x += keyWidthWorld_ * 0.5f;
            laneBorderLine_[1][i + 1].localVertex[2].x += keyWidthWorld_ * 0.5f;
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

    // Zファイティングを防ぐために、Z座標を少しずらす
    for(int i = 0; i < kKeyCount_; i++){
        for(int j = 0; j < 4; j++){
            laneAnswer_[0][i].tri.localVertex[j].z += layerOffset.z;
            laneAnswer_[1][i].tri.localVertex[j].z += layerOffset.z;
            laneAnswer_[0][i].evalutionPolygon.localVertex[j].z += layerOffset.z * 2;
            laneAnswer_[1][i].evalutionPolygon.localVertex[j].z += layerOffset.z * 2;
        }
    }

    for(int i = 0; i < kKeyCount_ + 1; i++){
        for(int j = 0; j < 3; j++){
            laneBorderLineAura_[0][i].localVertex[j].z += layerOffset.z * 3;
            laneBorderLineAura_[1][i].localVertex[j].z += layerOffset.z * 3;
            laneBorderLine_[0][i].localVertex[j].z += layerOffset.z * 4;
            laneBorderLine_[1][i].localVertex[j].z += layerOffset.z * 4;
        }
    }

    // エフェクトの発生位置を計算
    CalcEffectEmitPoints();

    // エフェクトの初期化
    EffectSystem::DeleteAll();// 既存のエフェクトを削除
    EffectSystem::AddEffectEndless("stageBack.json", SEED::GetMainCamera()->GetTranslation(), nullptr);

    // 背景の初期化
    backImage_ = Sprite("PlayField/tempBack.png");
    backImage_.drawLocation = DrawLocation::Back;
    backImage_.isStaticDraw = false;
    backImage_.size = kWindowSize;

    //objects2D_ = GameSystem::GetScene()->GetHierarchy()->LoadFromJson("Resources/Jsons/Prefabs/PlayFieldItems.json", false).objects2D_;

    SEED::windowBackColor_ = 0x26554EFF;
}


/////////////////////////////////////////////////////////////////////////
// 更新
/////////////////////////////////////////////////////////////////////////
void PlayField::Update(){

    // 押されたら反応するレーンの描画
    for(auto& answerQuadArray : laneAnswer_){
        for(auto& answerQuad : answerQuadArray){
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

    // 背景の描画
    //backImage_.Draw();
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
    laneAnswer_[0][lane].isPress ? laneAnswer_[0][lane].isRelease = true : laneAnswer_[0][lane].isRelease = false;
    laneAnswer_[1][lane].isPress ? laneAnswer_[1][lane].isRelease = true : laneAnswer_[1][lane].isRelease = false;
    // 押下状態を更新
    laneAnswer_[0][lane].isPress = false;
    laneAnswer_[1][lane].isPress = false;
    // 押下状態を解除
    laneAnswer_[0][lane].isTrigger = false;
    laneAnswer_[1][lane].isTrigger = false;
}

/////////////////////////////////////////////////////////////////////////
// 流れてくるノーツ描画に使う頂点情報を取得
/////////////////////////////////////////////////////////////////////////
Quad PlayField::GetNoteQuad(float timeRatio, int32_t lane, UpDown layer, float ratioWidth){
    Quad result;
    Triangle laneTriangle = lane_[(int)layer][lane];

    // ノーツの奥行きの計算
    float farZ = std::clamp(timeRatio + ratioWidth * 0.5f, 0.0f, 1.0f);
    float nearZ = std::clamp(timeRatio - ratioWidth * 0.5f, 0.0f, 1.0f);

    // ノーツの矩形の頂点を計算
    result.localVertex[0] = MyMath::Lerp(laneTriangle.localVertex[0], laneTriangle.localVertex[2], 1.0f - farZ);
    result.localVertex[1] = MyMath::Lerp(laneTriangle.localVertex[0], laneTriangle.localVertex[1], 1.0f - farZ);
    result.localVertex[2] = MyMath::Lerp(laneTriangle.localVertex[0], laneTriangle.localVertex[2], 1.0f - nearZ);
    result.localVertex[3] = MyMath::Lerp(laneTriangle.localVertex[0], laneTriangle.localVertex[1], 1.0f - nearZ);

    return result;
}

Quad PlayField::GetWheelFloorQuad(float timeRatio, UpDown layer, float ratioWidth){
    Quad result;

    // ノーツの奥行きの計算
    float farZ = std::clamp(timeRatio - ratioWidth * 0.5f, 0.0f, 1.0f);
    float nearZ = std::clamp(timeRatio + ratioWidth * 0.5f, 0.0f, 1.0f);

    // ノーツの矩形の頂点を計算
    if(layer == UpDown::UP){
        result.localVertex[0] = MyMath::Lerp(playFieldPointsWorld_[TOP], playFieldPointsWorld_[LEFT], farZ);
        result.localVertex[1] = MyMath::Lerp(playFieldPointsWorld_[TOP], playFieldPointsWorld_[RIGHT], farZ);
        result.localVertex[2] = MyMath::Lerp(playFieldPointsWorld_[TOP], playFieldPointsWorld_[LEFT], nearZ);
        result.localVertex[3] = MyMath::Lerp(playFieldPointsWorld_[TOP], playFieldPointsWorld_[RIGHT], nearZ);
    } else{
        result.localVertex[0] = MyMath::Lerp(playFieldPointsWorld_[BOTTOM], playFieldPointsWorld_[RIGHT], farZ);
        result.localVertex[1] = MyMath::Lerp(playFieldPointsWorld_[BOTTOM], playFieldPointsWorld_[LEFT], farZ);
        result.localVertex[2] = MyMath::Lerp(playFieldPointsWorld_[BOTTOM], playFieldPointsWorld_[RIGHT], nearZ);
        result.localVertex[3] = MyMath::Lerp(playFieldPointsWorld_[BOTTOM], playFieldPointsWorld_[LEFT], nearZ);
    }
    return result;
}

Quad PlayField::GetWheelDirectionQuad(float timeRatio, UpDown layer){
    Quad result;

    // ノーツの奥行きの計算
    timeRatio = std::clamp(timeRatio, 0.0f, 1.0f);
    float height = timeRatio == 1.0f ? 0.0f : 4.0f;

    // ノーツの矩形の頂点を計算
    if(layer == UpDown::UP){
        result.localVertex[2] = MyMath::Lerp(playFieldPointsWorld_[TOP], playFieldPointsWorld_[LEFT], timeRatio);
        result.localVertex[3] = MyMath::Lerp(playFieldPointsWorld_[TOP], playFieldPointsWorld_[RIGHT], timeRatio);
        result.localVertex[0] = result.localVertex[2] + Vector3(0.0f, height, 0.0f);
        result.localVertex[1] = result.localVertex[3] + Vector3(0.0f, height, 0.0f);
    } else{
        result.localVertex[2] = MyMath::Lerp(playFieldPointsWorld_[BOTTOM], playFieldPointsWorld_[RIGHT], timeRatio);
        result.localVertex[3] = MyMath::Lerp(playFieldPointsWorld_[BOTTOM], playFieldPointsWorld_[LEFT], timeRatio);
        result.localVertex[0] = result.localVertex[2] + Vector3(0.0f, -height, 0.0f);
        result.localVertex[1] = result.localVertex[3] + Vector3(0.0f, -height, 0.0f);
    }
    return result;
}

// RectFlickの矩形を取得
Quad2D PlayField::GetRectFlickQuad(float timeRatio, DIRECTION8 dir, float ratioWidth){
    Quad2D result;
    Vector2 points[2];

    switch(dir){
    case DIRECTION8::LEFTTOP:
        points[0] = playFieldPointsScreen_[LEFT];
        points[1] = playFieldPointsScreen_[TOP];
        break;

    case DIRECTION8::RIGHTTOP:
        points[0] = playFieldPointsScreen_[TOP];
        points[1] = playFieldPointsScreen_[RIGHT];
        break;

    case DIRECTION8::RIGHTBOTTOM:
        points[0] = playFieldPointsScreen_[RIGHT];
        points[1] = playFieldPointsScreen_[BOTTOM];
        break;

    case DIRECTION8::LEFTBOTTOM:
        points[0] = playFieldPointsScreen_[BOTTOM];
        points[1] = playFieldPointsScreen_[LEFT];
        break;

    default:
        return result; // 無効な方向の場合は空のQuadを返す
    }

    // ノーツの奥行きの計算
    float timeratio2 = std::clamp(timeRatio, 0.3f, FLT_MAX);
    float farZ = std::clamp(timeratio2 + ratioWidth * 0.5f, 0.0f, FLT_MAX);
    float nearZ = std::clamp(timeratio2 - ratioWidth * 0.5f, 0.0f, FLT_MAX);

    // ノーツの矩形の頂点を計算
    result.localVertex[0] = MyMath::Lerp(kWindowCenter, points[0], farZ);
    result.localVertex[1] = MyMath::Lerp(kWindowCenter, points[1], farZ);
    result.localVertex[2] = MyMath::Lerp(kWindowCenter, points[0], nearZ);
    result.localVertex[3] = MyMath::Lerp(kWindowCenter, points[1], nearZ);

    return result;
}



void PlayField::CalcEffectEmitPoints(){

    // レーン部分
    for(int i = 0; i < kKeyCount_; i++){
        Vector3 point = (keyboardBorderPoints_[i] + keyboardBorderPoints_[i + 1]) * 0.5f;
        effectEmitPoints_[LaneBit(LANE_1 << i)] = point;
    }

    // 左右のサイドフリック
    effectEmitPoints_[SIDEFLICK_LEFT] = playFieldPointsWorld_[LEFT];
    effectEmitPoints_[SIDEFLICK_RIGHT] = playFieldPointsWorld_[RIGHT];
    // ホイールノーツ
    effectEmitPoints_[WHEEL_DOWN] = (playFieldPointsWorld_[LEFT] + playFieldPointsWorld_[RIGHT]) * 0.5f;
    effectEmitPoints_[WHEEL_UP] = effectEmitPoints_[WHEEL_DOWN];

    // レクトフリックのエフェクト発生位置(wheelノーツと同じ)
    effectEmitPoints_[RECTFLICK_LT] = effectEmitPoints_[WHEEL_DOWN];
    effectEmitPoints_[RECTFLICK_RT] = effectEmitPoints_[WHEEL_DOWN];
    effectEmitPoints_[RECTFLICK_LB] = effectEmitPoints_[WHEEL_DOWN];
    effectEmitPoints_[RECTFLICK_RB] = effectEmitPoints_[WHEEL_DOWN];
    effectEmitPoints_[RECTFLICK_ALL] = effectEmitPoints_[WHEEL_DOWN];

}

int PlayField::GetLaneBitIndex(uint32_t laneBit){
    assert(laneBit != 0 && (laneBit & (laneBit - 1)) == 0); // 単一ビットのみ有効かチェック
    int index = 0;
    while((laneBit >>= 1) != 0){
        ++index;
    }
    return index;
}

/////////////////////////////////////////////////////////////
// レーンのエフェクトを発生させる関数
/////////////////////////////////////////////////////////////
void PlayField::LaneEffect(int evalution, LaneBit laneBit){
    switch(evalution){
    case Judgement::Evaluation::PERFECT:
        EffectSystem::AddEffectOnce("hitEffect_perfect.json", effectEmitPoints_[laneBit]);
        break;

    case Judgement::Evaluation::GREAT:
        EffectSystem::AddEffectOnce("hitEffect_great.json", effectEmitPoints_[laneBit]);
        break;

    case Judgement::Evaluation::GOOD:
        EffectSystem::AddEffectOnce("hitEffect_good.json", effectEmitPoints_[laneBit]);
        break;

    default:
        // MISSのときは何もしない
        return;
    }
}

/////////////////////////////////////////////////////////////
// wheelノーツのエフェクトを発生させる関数
/////////////////////////////////////////////////////////////
void PlayField::WheelEffect(int evalution, LaneBit laneBit){

    std::string filename;
    if(laneBit & LaneBit::WHEEL_UP){
        filename = "wheel_up.json";
    } else{
        filename = "wheel_down.json";
    }

    switch(evalution){
    case Judgement::Evaluation::PERFECT:
        EffectSystem::AddEffectOnce(filename, effectEmitPoints_[laneBit]);
        break;

    case Judgement::Evaluation::GREAT:
        EffectSystem::AddEffectOnce(filename, effectEmitPoints_[laneBit]);
        break;

    case Judgement::Evaluation::GOOD:
        EffectSystem::AddEffectOnce(filename, effectEmitPoints_[laneBit]);
        break;

    default:
        // MISSのときは何もしない
        return;
    }
}


/////////////////////////////////////////////////////////////
// rectFlickのエフェクトを発生させる関数
/////////////////////////////////////////////////////////////
void PlayField::RectFlickEffect(int evalution, LaneBit laneBit){

    evalution;
    if(laneBit & LaneBit::RECTFLICK_LT){
        EffectSystem::AddEffectOnce("rectFlick_LT.json", effectEmitPoints_[LaneBit::RECTFLICK_ALL]);
    }
    if(laneBit & LaneBit::RECTFLICK_RT){
        EffectSystem::AddEffectOnce("rectFlick_RT.json", effectEmitPoints_[LaneBit::RECTFLICK_ALL]);
    }
    if(laneBit & LaneBit::RECTFLICK_LB){
        EffectSystem::AddEffectOnce("rectFlick_LB.json", effectEmitPoints_[LaneBit::RECTFLICK_ALL]);
    }
    if(laneBit & LaneBit::RECTFLICK_RB){
        EffectSystem::AddEffectOnce("rectFlick_RB.json", effectEmitPoints_[LaneBit::RECTFLICK_ALL]);
    }
}

/////////////////////////////////////////////////////////////
// エフェクトを発生させる関数
/////////////////////////////////////////////////////////////
void PlayField::EmitEffect(LaneBit laneBit, UpDown layer, int evalution){

    layer;

    //===================================
    // レーンに属するノーツの場合(tap,hold)
    //===================================
    for(int i = 0; i < kKeyCount_; i++){
        if(laneBit & (1 << i)){
            // レーンのエフェクトを発生させる
            LaneEffect(evalution, LaneBit(LANE_1 << i));
        }
    }

    //===================================
    // ホイールノーツの場合
    //===================================
    if(laneBit & LaneBit::WHEEL){
        WheelEffect(evalution, laneBit);
    }

    //===================================
    // レクトフリックの場合
    //===================================
    if(laneBit & LaneBit::RECTFLICK_ALL){
        // レクトフリックのエフェクトを発生させる
        RectFlickEffect(evalution, laneBit);
    }

}

// スクリーンのカーソル座標をワールドに
Vector3 PlayField::GetCursorWorldPos(float cursorX){
    float t = (cursorX - playFieldPointsScreen_[LEFT].x) / 
        (playFieldPointsScreen_[RIGHT].x - playFieldPointsScreen_[LEFT].x);
    Vector3 result = MyMath::Lerp(playFieldPointsWorld_[LEFT], playFieldPointsWorld_[RIGHT], t);
    return result;
}
