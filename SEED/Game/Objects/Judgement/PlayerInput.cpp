#include "PlayerInput.h"
#include <SEED/Lib/Functions/MyFunc/MyMath.h>
#include <SEED/Lib/Functions/MyFunc/MatrixFunc.h>
#include <Game/Objects/Judgement/PlayField.h>
#include <SEED/Lib/enums/Direction.h>

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
            if(Input::IsTriggerMouse(MOUSE_BUTTON::LEFT)){
                return true;
            } else{
                if(cursorLane_.Value() != cursorLane_.PreValue()){
                    if(Input::IsPressMouse(MOUSE_BUTTON::LEFT)){
                        return true;
                    }
                } else{
                    return 
                        Input::IsTriggerKey(DIK_A) or Input::IsTriggerKey(DIK_S) or Input::IsTriggerKey(DIK_D) or 
                        Input::IsTriggerKey(DIK_F) or Input::IsTriggerKey(DIK_SPACE) or Input::IsTriggerMouse(MOUSE_BUTTON::LEFT);
                }
            }
            return false;
        };
        tap_.Release = []{
            return 
                Input::IsReleaseKey(DIK_A) or Input::IsReleaseKey(DIK_S) or Input::IsReleaseKey(DIK_D) or 
                Input::IsReleaseKey(DIK_F) or Input::IsReleaseKey(DIK_SPACE) or Input::IsReleaseMouse(MOUSE_BUTTON::LEFT);
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
    // カーソルの矩形を設定
    cursorQuad_ = MakeEqualQuad2D(30.0f, { 1.0f,0.0f,0.0f,1.0f });
    cursorQuad_.translate = { cursorPos_, kWindowCenter.y };
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////
// 更新
/////////////////////////////////////////////////////////////////////////////////////////////////////////
void PlayerInput::Update(){
    // カーソルの移動量を取得
    Vector2 mouseVal = Input::GetMouseVector();

    // カーソルの移動量を加算
    cursorPos_ += mouseVal.x * cursorSenstivity_;

    // マウス右ボタン押していればループ、押していなければクランプ
    if(Input::IsPressMouse(MOUSE_BUTTON::RIGHT)){
        cursorPos_ = MyFunc::Spiral(cursorPos_, edgePos_[(int)LR::LEFT], edgePos_[(int)LR::RIGHT]);
    } else{
        cursorPos_ = std::clamp(cursorPos_, edgePos_[(int)LR::LEFT], edgePos_[(int)LR::RIGHT]);
    }

    // カーソル矩形の位置を更新
    cursorQuad_.translate = { cursorPos_, kWindowCenter.y };
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////
// 描画
/////////////////////////////////////////////////////////////////////////////////////////////////////////
void PlayerInput::Draw(){
    // カーソルの描画
    SEED::DrawQuad2D(cursorQuad_);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////
// フレーム開始時の処理
/////////////////////////////////////////////////////////////////////////////////////////////////////////
void PlayerInput::BeginFrame(){
    // 前フレームでの情報を保存
    preCursorPos_ = cursorPos_;

    // 入力情報の更新
    DecideLaneInput();
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////
// フレーム終了時の処理
/////////////////////////////////////////////////////////////////////////////////////////////////////////
void PlayerInput::EndFrame(){
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
// タップしたレーンを取得
/////////////////////////////////////////////////////////////////////////////////////////////////////////
std::unordered_set<int32_t> PlayerInput::SystemGetTapLane(){

    std::unordered_set<int32_t> tapLane;
    if(!tap_.Trigger()){
        return tapLane;
    }
    
    // タップしたレーンを取得
    if(Input::IsTriggerKey(DIK_A)){ tapLane.insert(0); }
    if(Input::IsTriggerKey(DIK_S)){ tapLane.insert(1); }
    if(Input::IsTriggerKey(DIK_D)){ tapLane.insert(2); }
    if(Input::IsTriggerKey(DIK_F)){ tapLane.insert(3); }
    if(Input::IsTriggerKey(DIK_SPACE)){ tapLane.insert(4); }
    if(Input::IsTriggerMouse(MOUSE_BUTTON::LEFT)){
        tapLane.insert(cursorLane_.Value());
    }
    if(cursorLane_.Value() != cursorLane_.PreValue()){
        if(Input::IsPressMouse(MOUSE_BUTTON::LEFT)){
            tapLane.insert(cursorLane_.Value());
        }
    }

    return tapLane;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////
// リリースしたレーンを取得
/////////////////////////////////////////////////////////////////////////////////////////////////////////
std::unordered_set<int32_t> PlayerInput::SystemGetReleaseLane(){
    std::unordered_set<int32_t> releaseLane;
    if(!hold_.Release()){
        return releaseLane;
    }
    // リリースしたレーンを取得
    if(Input::IsReleaseKey(DIK_A)){ releaseLane.insert(0); }
    if(Input::IsReleaseKey(DIK_S)){ releaseLane.insert(1); }
    if(Input::IsReleaseKey(DIK_D)){ releaseLane.insert(2); }
    if(Input::IsReleaseKey(DIK_F)){ releaseLane.insert(3); }
    if(Input::IsReleaseKey(DIK_SPACE)){ releaseLane.insert(4); }
    if(Input::IsReleaseMouse(MOUSE_BUTTON::LEFT)){
        releaseLane.insert(cursorLane_.Value());
    }
    if(cursorLane_.Value() != cursorLane_.PreValue()){
        if(Input::IsPressMouse(MOUSE_BUTTON::LEFT)){
            releaseLane.insert(cursorLane_.PreValue());
        }
    }
    return releaseLane;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////
// レーンのInput情報を決定する関数
/////////////////////////////////////////////////////////////////////////////////////////////////////////
void PlayerInput::DecideLaneInput(){
    // ホールドしているレーンを取得
    holdLane_ = hold_.Value();
    // タップしたレーンを取得
    tapLane_ = SystemGetTapLane();
    // リリースしたレーンを取得
    releaseLane_ = SystemGetReleaseLane();
}