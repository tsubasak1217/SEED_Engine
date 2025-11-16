#include "PlayerInput.h"
#include <SEED/Lib/Functions/MyMath.h>
#include <SEED/Lib/Functions/MatrixFunc.h>
#include <Game/Objects/Judgement/PlayField.h>
#include <SEED/Lib/enums/Direction.h>
#include <Game/Objects/Judgement/MouseVectorCircle.h>
#include <Game/Config/PlaySettings.h>

/////////////////////////////////////////////////////////
// static変数の初期化
/////////////////////////////////////////////////////////
PlayerInput* PlayerInput::instance_ = nullptr;

/////////////////////////////////////////////////////////
// コンストラクタ
/////////////////////////////////////////////////////////
PlayerInput::PlayerInput(){

    ///////////////////////////////////////////////////////
    // タップの取得
    ///////////////////////////////////////////////////////
    {
        tap_.Value = []{
            return
                Input::IsPressKey(DIK_A) or Input::IsPressKey(DIK_S) or Input::IsPressKey(DIK_D) or
                Input::IsPressKey(DIK_F) or Input::IsPressKey(DIK_SPACE) or Input::IsPressMouse(MOUSE_BUTTON::LEFT);
        };
        tap_.Trigger = [&]{
            return tapLane_.size() > 0;
        };
        tap_.Release = [&]{
            return releaseLane_.size() > 0;
        };
        tap_.Press = [&]{ return tap_.Value(); };
    }

    ///////////////////////////////////////////////////////
    // 左右フリックの取得
    ///////////////////////////////////////////////////////
    {
        sideFlick_.Value = [&]{
            float horizontalVal = Input::GetMouseVector().x;
            if(horizontalVal > flickDeadZone_){ return LR::RIGHT; }
            if(horizontalVal < -flickDeadZone_){ return LR::LEFT; }
            return LR::NONE;
        };

        sideFlick_.PreValue = [&]{
            float horizontalVal = Input::GetMouseVector(INPUT_STATE::BEFORE).x;
            if(horizontalVal > flickDeadZone_){ return LR::RIGHT; }
            if(horizontalVal < -flickDeadZone_){ return LR::LEFT; }
            return LR::NONE;
        };

        sideFlick_.Trigger = [&]{
            return (sideFlick_.Value() != LR::NONE) && (sideFlick_.PreValue() == LR::NONE);
        };
    }

    ///////////////////////////////////////////////////////
    // 矩形フリックの取得
    ///////////////////////////////////////////////////////
    {
        rectFlick_.Value = [&]{
            // LT,RT,LB,RB方向のマウスフリック判定を取得
            Vector2 flickVec = Input::GetMouseVector();

            // 長さがデッドゾーンより小さければ無視
            if(MyMath::Length(flickVec) < flickDeadZone_){ return DIRECTION8::NONE; }

            // 方向を取得
            if(flickVec.x > 0.0f && flickVec.y <= 0.0f){ return DIRECTION8::RIGHTTOP; }
            if(flickVec.x > 0.0f && flickVec.y > 0.0f){ return DIRECTION8::RIGHTBOTTOM; }
            if(flickVec.x <= 0.0f && flickVec.y > 0.0f){ return DIRECTION8::LEFTBOTTOM; }
            if(flickVec.x <= 0.0f && flickVec.y <= 0.0f){ return DIRECTION8::LEFTTOP; }

            // どの方向にも当てはまらなければ無視
            return DIRECTION8::NONE;
        };

        rectFlick_.PreValue = [&]{
            // LT,RT,LB,RB方向のマウスフリック判定を取得
            Vector2 flickVec = Input::GetMouseVector(INPUT_STATE::BEFORE);

            // 長さがデッドゾーンより小さければ無視
            if(MyMath::Length(flickVec) < flickDeadZone_){ return DIRECTION8::NONE; }

            // 方向を取得
            if(flickVec.x > 0.0f && flickVec.y <= 0.0f){ return DIRECTION8::RIGHTTOP; }
            if(flickVec.x > 0.0f && flickVec.y > 0.0f){ return DIRECTION8::RIGHTBOTTOM; }
            if(flickVec.x <= 0.0f && flickVec.y > 0.0f){ return DIRECTION8::LEFTBOTTOM; }
            if(flickVec.x <= 0.0f && flickVec.y <= 0.0f){ return DIRECTION8::LEFTTOP; }

            // どの方向にも当てはまらなければ無視
            return DIRECTION8::NONE;
        };

        rectFlick_.Trigger = [&]{
            return (rectFlick_.Value() != DIRECTION8::NONE) && (rectFlick_.PreValue() == DIRECTION8::NONE);
        };
    }

    ///////////////////////////////////////////////////////
    // ホールドの取得
    ///////////////////////////////////////////////////////
    {
        hold_.Value = [&]{
            std::unordered_set<int32_t> holdLane;
            if(Input::IsPressKey(DIK_A)){ holdLane.insert(0); }
            if(Input::IsPressKey(DIK_S)){ holdLane.insert(1); }
            if(Input::IsPressKey(DIK_D)){ holdLane.insert(2); }
            if(Input::IsPressKey(DIK_F)){ holdLane.insert(3); }
            if(Input::IsPressKey(DIK_SPACE)){ holdLane.insert(4); }
            if(Input::IsPressMouse(MOUSE_BUTTON::LEFT)){ holdLane.insert(cursorLane_.Value()); }
            return holdLane;

        };
        hold_.Trigger = [&]{
            return tap_.Trigger();
        };
        hold_.Release = [&]{
            return tap_.Release();
        };
        hold_.Press = [&]{
            return tap_.Value();
        };
    }

    ///////////////////////////////////////////////////////
    // レーンの取得
    ///////////////////////////////////////////////////////
    {
        cursorLane_.Value = []{
            static float basePosX = kWindowCenter.x - PlayField::kPlayFieldSizeX_ * 0.5f;
            float cursorDif = instance_->cursorPos_ - basePosX;
            return std::clamp(int(cursorDif / PlayField::kKeyWidth_), 0, PlayField::kKeyCount_ - 1);
        };

        cursorLane_.PreValue = []{
            static float basePosX = kWindowCenter.x - PlayField::kPlayFieldSizeX_ * 0.5f;
            float cursorDif = instance_->preCursorPos_ - basePosX;
            return std::clamp(int(cursorDif / PlayField::kKeyWidth_), 0, PlayField::kKeyCount_ - 1);
        };
    }
    ///////////////////////////////////////////////////////
    // ホイールの取得
    ///////////////////////////////////////////////////////
    {
        wheelScroll_.PreValue = []{
            int32_t wheel = Input::GetMouseWheel(INPUT_STATE::BEFORE);
            if(wheel == 0){ return UpDown::NONE; }
            return (wheel > 0) ? UpDown::UP : UpDown::DOWN;
        };
        wheelScroll_.Value = []{
            int32_t wheel = Input::GetMouseWheel();
            if(wheel == 0){ return UpDown::NONE; }
            return (wheel > 0) ? UpDown::UP : UpDown::DOWN;
        };
        wheelScroll_.Trigger = [&]{
            if(wheelScroll_.PreValue() != UpDown::NONE){ return false; }
            return wheelScroll_.Value() != UpDown::NONE;
        };
    }
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////
// インスタンスの取得
/////////////////////////////////////////////////////////////////////////////////////////////////////////
PlayerInput* PlayerInput::GetInstance(){
    if(!instance_){
        instance_ = new PlayerInput();
    }
    return instance_;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////
// デストラクタ
/////////////////////////////////////////////////////////////////////////////////////////////////////////
PlayerInput::~PlayerInput(){
}


///////////////////////////////////////////////////////////////////////////////////////////////
// インスタンスの削除
///////////////////////////////////////////////////////////////////////////////////////////////
void PlayerInput::Initialize(){
    // カーソルの初期位置をウィンドウの中心に設定
    cursorPos_ = kWindowCenter.x;
    preCursorPos_ = kWindowCenter.x;
    // カーソルの端の位置を設定
    edgePos_[(int)LR::LEFT] = cursorPos_ - PlayField::kPlayFieldSizeX_ * 0.5f;
    edgePos_[(int)LR::RIGHT] = cursorPos_ + PlayField::kPlayFieldSizeX_ * 0.5f;

    for(int i = 0; i < 2; i++){
        cursor_[i] = Triangle();
        cursor_[i].GH = TextureManager::LoadTexture("PlayField/cursor_half.png");
        cursor_[i].litingType = LIGHTINGTYPE_NONE;
        //cursor_[i].blendMode = BlendMode::ADD;

        cursor2D_[i] = Triangle2D();
        cursor2D_[i].GH = TextureManager::LoadTexture("PlayField/cursor_half.png");
        //cursor2D_[i].color = { 1.0f,0.0f,1.0f,1.0f };

        cursorLine_[i] = Quad();
        cursorLine_[i].GH = TextureManager::LoadTexture("PlayField/lineAura.png");
        cursorLine_[i].color = { 1.0f,1.0f,0.0f,1.0f };
    }

    // パラメータの初期化
    flickDeadZone_ = 30.0f;// フリックのデッドゾーン

    // マウスベクトル表示UIの初期化
    mouseVectorCircle_ = std::make_unique<MouseVectorCircle>();
    mouseVectorCircle_->Initialize();
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////
// 更新
/////////////////////////////////////////////////////////////////////////////////////////////////////////
void PlayerInput::Update(){

    // 必要な頂点の計算
    Vector3 corsorWorldPos = PlayField::GetInstance()->GetCursorWorldPos(cursorPos_);
    Vector3 farPos[2] = {
        PlayField::GetInstance()->GetPlayFieldPointWorld(0),
        PlayField::GetInstance()->GetPlayFieldPointWorld(2)
    };
    float dist = MyMath::Length(corsorWorldPos - farPos[0]);
    float keyWidth = PlayField::GetInstance()->GetKeyWidthWorld();
    float zRate = 0.015f;
    float keyWidthRate = 0.35f;

    // カーソル矩形の位置を更新
    cursor_[0].localVertex[0] = corsorWorldPos + MyMath::Normalize(farPos[0] - corsorWorldPos) * dist * zRate;
    cursor_[0].localVertex[1] = corsorWorldPos + Vector3(keyWidth * keyWidthRate,0.0f,0.0f);
    cursor_[0].localVertex[2] = corsorWorldPos - Vector3(keyWidth * keyWidthRate,0.0f,0.0f);
    cursor_[1].localVertex[0] = corsorWorldPos + MyMath::Normalize(farPos[1] - corsorWorldPos) * dist * zRate;
    cursor_[1].localVertex[1] = corsorWorldPos - Vector3(keyWidth * keyWidthRate, 0.0f, 0.0f);
    cursor_[1].localVertex[2] = corsorWorldPos + Vector3(keyWidth * keyWidthRate, 0.0f, 0.0f);

    // zファイティング防止
    for(int i = 0; i < 3; i++){
        cursor_[0].localVertex[i].z -= 0.1f;
        cursor_[1].localVertex[i].z -= 0.1f;
    }

    float lineWidthRate = 0.5f;
    cursorLine_[0].localVertex[0] = farPos[0];
    cursorLine_[0].localVertex[1] = farPos[0];
    cursorLine_[0].localVertex[2] = corsorWorldPos - Vector3(keyWidth * lineWidthRate, 0.0f, 0.0f);
    cursorLine_[0].localVertex[3] = corsorWorldPos + Vector3(keyWidth * lineWidthRate, 0.0f, 0.0f);
    cursorLine_[1].localVertex[0] = farPos[1];
    cursorLine_[1].localVertex[1] = farPos[1];
    cursorLine_[1].localVertex[2] = corsorWorldPos + Vector3(keyWidth * lineWidthRate, 0.0f, 0.0f);
    cursorLine_[1].localVertex[3] = corsorWorldPos - Vector3(keyWidth * lineWidthRate, 0.0f, 0.0f);

    // zファイティング防止
    for(int i = 0; i < 4; i++){
        cursorLine_[0].localVertex[i].z -= 0.1f;
        cursorLine_[1].localVertex[i].z -= 0.1f;
    }


    BaseCamera* camera = SEED::GetMainCamera();
    for(int i = 0; i < 3; i++){
        cursor2D_[0].localVertex[i] = camera->ToScreenPosition(cursor_[0].localVertex[i]);
        cursor2D_[1].localVertex[i] = camera->ToScreenPosition(cursor_[1].localVertex[i]);
    }

    // マウスベクトル表示UIの更新
    mouseVectorCircle_->Update();

}

/////////////////////////////////////////////////////////////////////////////////////////////////////////
// 描画
/////////////////////////////////////////////////////////////////////////////////////////////////////////
void PlayerInput::Draw(){
    // カーソルの描画
    for(int i = 0; i < 2; i++){
        //SEED::DrawTriangle(cursor_[i]);
        //SEED::DrawTriangle2D(cursor2D_[i]);
        SEED::DrawQuad(cursorLine_[i]);
    }

#ifdef _DEBUG
    // 入力情報の表示
    DisplayInputInfo();
#endif // _DEBUG
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////
// フレーム開始時の処理
/////////////////////////////////////////////////////////////////////////////////////////////////////////
void PlayerInput::BeginFrame(){
    // 前フレームでの情報を保存
    preCursorPos_ = cursorPos_;

    // カーソルの移動量を取得
    Vector2 mouseVal = Input::GetMouseVector();

    // カーソルの移動量を加算
    cursorPos_ += mouseVal.x * PlaySettings::GetInstance()->GetCursorSenstivity();

    // 入力情報の更新
    DecideLaneInput();
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////
// フレーム終了時の処理
/////////////////////////////////////////////////////////////////////////////////////////////////////////
void PlayerInput::EndFrame(){
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////
// 指定したレーンをタップしたか取得
/////////////////////////////////////////////////////////////////////////////////////////////////////////
bool PlayerInput::GetIsTap(int32_t key){
    // keyが範囲外ならfalse
    if(key < 0 || key >= PlayField::kKeyCount_){
        return false;
    }
    // tapLaneに含まれているかどうか
    if(tapLane_.find(key) != tapLane_.end()){
        return true;
    }
    return false;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////
// 指定したレーンを押しているかどうか
/////////////////////////////////////////////////////////////////////////////////////////////////////////
bool PlayerInput::GetIsPress(int32_t key){

    // keyが範囲外ならfalse
    if(key < 0 || key >= PlayField::kKeyCount_){
        return false;
    }

    // tapLaneに含まれているかどうか
    if(holdLane_.find(key) != holdLane_.end()){
        return true;
    }

    return false;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////
// 指定したレーンを離したかどうか
/////////////////////////////////////////////////////////////////////////////////////////////////////////
bool PlayerInput::GetIsRelease(int32_t key){
    // keyが範囲外ならfalse
    if(key < 0 || key >= PlayField::kKeyCount_){
        return false;
    }
    // tapLaneに含まれているかどうか
    if(releaseLane_.find(key) != releaseLane_.end()){
        return true;
    }
    return false;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////
// 押していないレーンを取得
/////////////////////////////////////////////////////////////////////////////////////////////////////////
const std::unordered_set<int32_t>& PlayerInput::GetUnTapLane(){
    static std::unordered_set<int32_t> unTapLane;

    // いったん全部入れる
    for(int i = 0; i < PlayField::kKeyCount_; ++i){
        unTapLane.insert(i);
    }

    // 押しているレーンを削除
    auto& holdLane = GetHoldLane();
    for(auto& lane : holdLane){
        unTapLane.erase(lane);
    }

    return unTapLane;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////
// タップしたレーンを一括取得
/////////////////////////////////////////////////////////////////////////////////////////////////////////
std::unordered_set<int32_t> PlayerInput::SystemGetTapLane(){

    std::unordered_set<int32_t> tapLane;

    // タップしたレーンを取得
    if(Input::IsTriggerKey(DIK_A)){ tapLane.insert(0); }
    if(Input::IsTriggerKey(DIK_S)){ tapLane.insert(1); }
    if(Input::IsTriggerKey(DIK_D)){ tapLane.insert(2); }
    if(Input::IsTriggerKey(DIK_F)){ tapLane.insert(3); }
    if(Input::IsTriggerKey(DIK_SPACE)){ tapLane.insert(4); }

    // マウスをトリガーしたとき
    if(Input::IsTriggerMouse(MOUSE_BUTTON::LEFT)){
        tapLane.insert(cursorLane_.Value());

    } else{
        // マウスを押しているとき
        if(Input::IsPressMouse(MOUSE_BUTTON::LEFT)){
            // レーンが切り替わったら
            int preLane = cursorLane_.PreValue();
            int curLane = cursorLane_.Value();
            int kLaneCount = PlayField::kKeyCount_;
            float horizontalVal = Input::GetMouseVector().x * PlaySettings::GetInstance()->GetCursorSenstivity();

            // 間のレーンをタップしたことにする(左⇔右のカーソルがループすることも考慮)
            if(curLane != preLane){
                // 右側に動かしたとき
                if(horizontalVal > 0.0f){
                    for(int i = preLane; i != curLane; i = (i + 1) % kLaneCount){
                        if(i != preLane){
                            tapLane.insert(i);
                        }
                    }

                } else{// 左側に動かしたとき
                    for(int i = preLane; i != curLane; i = (i - 1 + kLaneCount) % kLaneCount){
                        if(i != preLane){
                            tapLane.insert(i);
                        }
                    }
                }

                tapLane.insert(curLane);

            } else{
                // マウスの移動量がkeyWidthより大きいとき、全部押している
                if(fabsf(horizontalVal) > PlayField::kKeyWidth_ && isLooped_){
                    for(int i = 0; i < kLaneCount; ++i){
                        tapLane.insert(i);
                    }
                }
            }
        }
    }

    return tapLane;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////
// リリースしたレーンを取得
/////////////////////////////////////////////////////////////////////////////////////////////////////////
std::unordered_set<int32_t> PlayerInput::SystemGetReleaseLane(){
    std::unordered_set<int32_t> releaseLane;

    // リリースしたレーンを取得
    if(Input::IsReleaseKey(DIK_A)){ releaseLane.insert(0); }
    if(Input::IsReleaseKey(DIK_S)){ releaseLane.insert(1); }
    if(Input::IsReleaseKey(DIK_D)){ releaseLane.insert(2); }
    if(Input::IsReleaseKey(DIK_F)){ releaseLane.insert(3); }
    if(Input::IsReleaseKey(DIK_SPACE)){ releaseLane.insert(4); }
    if(Input::IsReleaseMouse(MOUSE_BUTTON::LEFT)){
        releaseLane.insert(cursorLane_.Value());
    } else{
        // マウスを押しているとき
        if(Input::IsPressMouse(MOUSE_BUTTON::LEFT)){
            // レーンが切り替わったら
            int preLane = cursorLane_.PreValue();
            int curLane = cursorLane_.Value();
            int kLaneCount = PlayField::kKeyCount_;
            float horizontalVal = Input::GetMouseVector().x;

            // 間のレーンをタップしたことにする(左⇔右のカーソルがループすることも考慮)
            if(curLane != preLane){
                // 右側に動かしたとき
                if(horizontalVal > 0.0f){
                    for(int i = preLane; i != curLane; i = (i + 1) % kLaneCount){
                        if(i != curLane){
                            releaseLane.insert(i);
                        }
                    }
                } else{// 左側に動かしたとき
                    for(int i = preLane; i != curLane; i = (i - 1 + kLaneCount) % kLaneCount){
                        if(i != curLane){
                            releaseLane.insert(i);
                        }
                    }
                }

            } else{
                // マウスの移動量がkeyWidthより大きいとき、全部押している
                if(fabsf(horizontalVal) > PlayField::kKeyWidth_ && isLooped_){
                    for(int i = 0; i < kLaneCount; ++i){
                        releaseLane.insert(i);
                    }
                }
            }
        }
    }
    return releaseLane;
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////
// 譜面データのセット
/////////////////////////////////////////////////////////////////////////////////////////////////////////
void PlayerInput::SetNotesData(const NotesData* pNotesData){
    mouseVectorCircle_->SetNotesData(pNotesData);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////
// レーンのInput情報を決定する関数
/////////////////////////////////////////////////////////////////////////////////////////////////////////
void PlayerInput::DecideLaneInput(){

    // マウス右ボタン押していればループ、押していなければクランプ
    if(Input::IsPressMouse(MOUSE_BUTTON::RIGHT)){
        isLooped_ = cursorPos_ < edgePos_[(int)LR::LEFT] || cursorPos_ > edgePos_[(int)LR::RIGHT];
        cursorPos_ = MyFunc::Spiral(cursorPos_, edgePos_[(int)LR::LEFT], edgePos_[(int)LR::RIGHT]);
    } else{
        cursorPos_ = std::clamp(cursorPos_, edgePos_[(int)LR::LEFT], edgePos_[(int)LR::RIGHT]);
    }

    // ホールドしているレーンを取得
    holdLane_ = hold_.Value();
    // タップしたレーンを取得
    tapLane_ = SystemGetTapLane();
    // リリースしたレーンを取得
    releaseLane_ = SystemGetReleaseLane();
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////
// 入力情報を表示する関数
/////////////////////////////////////////////////////////////////////////////////////////////////////////
void PlayerInput::DisplayInputInfo(){
    /*--------------------------*/
// プレイヤーの入力を取得し表示
/*--------------------------*/
#ifdef _DEBUG
    auto& tpLane = GetTapLane();
    auto& hlLane = GetHoldLane();
    auto& relLane = GetReleaseLane();

    ImFunc::CustomBegin("input", MoveOnly_TitleBar);

    // タップ判定
    {
        ImGui::Text("タップ判定:{ ");
        ImGui::SameLine();
        for(auto& lane : tpLane){
            ImGui::Text("%d,", lane);
            ImGui::SameLine();
        }
        ImGui::Text("}");
        ImGui::Text("マウスを押しているか:%d", Input::IsPressMouse(MOUSE_BUTTON::LEFT));
        ImGui::Separator();
    }

    // ホールド判定
    {
        ImGui::Text("ホールド判定:{ ");
        ImGui::SameLine();
        for(auto& lane : hlLane){
            ImGui::Text("%d,", lane);
            ImGui::SameLine();
        }
        ImGui::Text("}");
        ImGui::Separator();
    }

    // リリース判定
    {
        ImGui::Text("離した判定:{ ");
        ImGui::SameLine();
        for(auto& lane : relLane){
            ImGui::Text("%d,", lane);
            ImGui::SameLine();
        }
        ImGui::Text("}");
        ImGui::Separator();
    }

    // カーソル
    {
        ImGui::Text("カーソルのレーン: %d", GetCursorLane());
        ImGui::Text("前のカーソルのレーン: %d", GetPreCursorLane());
        Vector2 mouseVal = Input::GetMouseVector() * PlaySettings::GetInstance()->GetCursorSenstivity();
        ImGui::Text("マウスの移動量: %.2f", mouseVal.x);
        ImGui::Separator();
    }

    // フリック
    {
        LR sideFlick = GetSideFlickDirection();
        ImGui::Text("SideFlick: %d | %d", sideFlick == LR::LEFT, sideFlick == LR::RIGHT);
        ImGui::Separator();

        DIRECTION8 rectFlick = GetRectFlickDirection();
        ImGui::Text("RectFlick");
        ImGui::Text("%d,%d", rectFlick == DIRECTION8::LEFTTOP, rectFlick == DIRECTION8::RIGHTTOP);
        ImGui::Text("%d,%d", rectFlick == DIRECTION8::LEFTBOTTOM, rectFlick == DIRECTION8::RIGHTBOTTOM);
        ImGui::Separator();
    }

    // ホイールスクロール
    {
        UpDown wheelScroll = GetWheelScrollDirection();
        ImGui::Text("wheel↑ %d", wheelScroll == UpDown::UP);
        ImGui::Text("wheel↓ %d", wheelScroll == UpDown::DOWN);
        ImGui::Text("Trigger %d", GetIsWheelTrigger());
        int32_t scrollVal = Input::GetMouseWheel();
        ImGui::Text("ホイールの移動量: %f", scrollVal);

        ImGui::Separator();
    }

    ImGui::End();
#endif // _DEBUG

}
