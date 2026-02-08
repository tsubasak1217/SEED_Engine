#pragma once
#include <unordered_set>
#include <SEED/Source/Manager/InputManager/InputManager.h>
#include <SEED/Source/Manager/InputManager/InputHandler.h>
#include <Game/Objects/Judgement/PlayField.h>
#include <SEED/Source/SEED.h>

class NotesData;
class MouseVectorCircle;

/// <summary>
/// 音ゲーのプレイヤー入力を格納するクラス
/// </summary>
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
    bool GetIsTap(int32_t key);
    bool GetIsPress(int32_t key);
    bool GetIsRelease(int32_t key);
    SEED::GeneralEnum::LR GetSideFlickDirection(){ return sideFlick_.Value(); }
    bool GetIsSideFlick(){ return sideFlick_.Trigger(); }
    SEED::GeneralEnum::DIRECTION8 GetRectFlickDirection(){ return rectFlick_.Value(); }
    bool GetIsRectFlick(){ return rectFlick_.Trigger(); }
    bool GetIsHold(){ return hold_.Press(); }
    int32_t GetCursorLane(){ return cursorLane_.Value(); }
    int32_t GetPreCursorLane(){ return cursorLane_.PreValue(); }
    const std::unordered_set<int32_t>& GetHoldLane(){ return holdLane_; }
    const std::unordered_set<int32_t>& GetTapLane(){ return tapLane_; }
    const std::unordered_set<int32_t>& GetReleaseLane(){ return releaseLane_; }
    const std::unordered_set<int32_t>& GetUnTapLane();
    bool GetIsWheelTrigger(){ return wheelScroll_.Trigger(); }
    SEED::GeneralEnum::UpDown GetWheelScrollDirection(){ return wheelScroll_.Value(); }
    float GetFlickDeadZone(){ return flickDeadZone_; }

public:// setter
    void SetNotesData(const NotesData* pNotesData);

private:
    const std::unordered_set<int32_t>& SystemGetHoldLane(){ return hold_.Value(); }
    std::unordered_set<int32_t> SystemGetTapLane();
    std::unordered_set<int32_t> SystemGetReleaseLane();

private:// 内部用
    void DecideLaneInput();
    void DisplayInputInfo();

private:
    float flickDeadZone_;// フリックのデッドゾーン
    float cursorPos_;// カーソルの位置(X軸移動のみ)
    float preCursorPos_;// 1フレーム前のカーソルの位置
    float edgePos_[2];// カーソルの端の位置
    bool isLooped_;// カーソルがループしたかどうか

private:
    // 入力を格納するハンドラ群
    SEED::InputHandler<bool> tap_;
    SEED::InputHandler<std::unordered_set<int32_t>> hold_;
    SEED::InputHandler<SEED::GeneralEnum::LR> sideFlick_;
    SEED::InputHandler<SEED::GeneralEnum::DIRECTION8> rectFlick_;
    SEED::InputHandler<int32_t>cursorLane_;
    SEED::InputHandler<SEED::GeneralEnum::UpDown> wheelScroll_;

    // 入力のUI表示用
    SEED::Topology::Triangle cursor_[2];
    SEED::Topology::Triangle2D cursor2D_[2];
    SEED::Topology::Quad cursorLine_[2];
    std::unique_ptr<MouseVectorCircle> mouseVectorCircle_;// マウスの動きを表すサークル

    // 判定のたびに無駄な計算を行わないように毎フレーム最初にこいつらに情報を入れる
    std::unordered_set<int32_t> holdLane_;
    std::unordered_set<int32_t> tapLane_;
    std::unordered_set<int32_t> releaseLane_;

    // カーソル位置に表示するアイコンオブジェクト
    SEED::GameObject2D* cursorIconObj_ = nullptr;
};