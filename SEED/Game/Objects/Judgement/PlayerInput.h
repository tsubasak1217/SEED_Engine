#pragma once
#include <unordered_set>
#include <SEED/Source/Manager/InputManager/InputManager.h>
#include <SEED/Source/Manager/InputManager/InputHandler.h>
#include <Game/Objects/Judgement/PlayField.h>
#include <SEED/Source/SEED.h>

class PlayerInput{// プレイヤーの入力状態を格納するクラス
private:
    PlayerInput();
    PlayerInput(const PlayerInput&) = delete;
    void operator=(const PlayerInput&) = delete;

public:
    static PlayerInput* GetInstance();
    ~PlayerInput();
    void Initialize();
    void Update();
    void Draw();
    void BeginFrame();
    void EndFrame();

public:// 入力情報を取得する関数
    bool GetIsTap(){ return tap_.Trigger(); }
    LR GetSideFlickDirection(){ return sideFlick_.Value(); }
    bool GetIsSideFlick(){ return sideFlick_.Trigger(); }
    DIRECTION8 GetRectFlickDirection(){ return rectFlick_.Value(); }
    bool GetIsRectFlick(){ return rectFlick_.Trigger(); }
    bool GetIsHold(){ return hold_.Press(); }
    const std::unordered_set<int32_t>& GetHoldLane(){ return hold_.Value(); }
    const std::unordered_set<int32_t>& GetTapLane();
    const std::unordered_set<int32_t>& GetReleaseLane();
    int32_t GetLane(){ return lane_.Value(); }

private:
    static PlayerInput* instance_;

private:
    float flickDeadZone_ = 10.0f;// フリックのデッドゾーン
    float cursorPos_ = 0.0f;// カーソルの位置(X軸移動のみ)
    float preCursorPos_ = 0.0f;// 1フレーム前のカーソルの位置
    float cursorSenstivity_ = 1.0f;// カーソルの感度
    float edgePos_[2] = { 0.0f,0.0f };// カーソルの端の位置

private:
    InputHandler<bool> tap_;
    InputHandler<std::unordered_set<int32_t>> hold_;
    InputHandler<LR> sideFlick_;
    InputHandler<DIRECTION8> rectFlick_;
    InputHandler<int32_t>lane_;
    Quad2D cursorQuad_;// カーソル描画用の四角形

};