#pragma once

//============================================================================
//	include
//============================================================================
#include <SEED/Source/Basic/Scene/State_Base.h>
#include <Game/Objects/Clear/ClearTextAnimation.h>
#include <Game/Objects/Clear/ClearSelectMenu.h>

//============================================================================
//	GameState_Clear class
//============================================================================
class GameState_Clear :
    public State_Base {
public:
    //========================================================================
    //	public Methods
    //========================================================================

    GameState_Clear(Scene_Base* pScene);
    GameState_Clear() = default;
    ~GameState_Clear() override;

    // 初期化処理
    void Initialize() override;
    void Finalize() override {}

    // 更新処理
    void Update() override;
    void Draw() override;

    void BeginFrame() override {}
    void EndFrame() override {}
    void HandOverColliders() override {}
    void ManageState() override;
private:
    //========================================================================
    //	private Methods
    //========================================================================

    //--------- structure ----------------------------------------------------
    //========================================================================
    //	private Methods
    //========================================================================

    //--------- structure ----------------------------------------------------

    // 状態
    enum class State {

        ZoomCamera, // カメラズーム処理
        InText,     // クリアテキスト登場
        OutText,    // クリアテキスト退場
        Select      // 入力処理
    };

    //--------- variables ----------------------------------------------------

    // 現在の状態
    State currentState_;

    // クリア文字
    std::unique_ptr<ClearTextAnimation> clearText_;
    Timer outTextTimer_ = Timer(0.64f);

    // メニュー
    std::unique_ptr<ClearSelectMenu> menu_;

    //--------- functions ----------------------------------------------------

    // エディター
    void Edit();

    // update
    /// ZoomCamera
    void UpdateZoomCamera();
    /// InText
    void UpdateInText();
    /// OutText
    void UpdateOutText();
    /// Select
    void UpdateSelect();
};