#pragma once

//============================================================================
//	include
//============================================================================
#include <SEED/Lib/enums/Direction.h>
#include <Game/Objects/Select/Drawer/SelectStageDrawer.h>

//============================================================================
//	SelectStage class
//============================================================================
class SelectStage {
public:
	//========================================================================
	//	public Methods
	//========================================================================

	SelectStage() = default;
	~SelectStage() = default;

    // 初期化
    void Initialize(uint32_t firstFocusStage);

    // 更新処理
    void Update();

    // 描画処理
    void Draw();

    // エディター
    void Edit();

	//--------- accessor -----------------------------------------------------

private:
	//========================================================================
	//	private Methods
	//========================================================================

	//--------- variables ----------------------------------------------------

    // 入力状態
    LR stickDirection_; // スティック入力
    bool stickLatched_; // スティック入力暴発防止

    // 入力管理
    std::unique_ptr<InputMapper<SelectInputEnum>> inputMapper_;

    // ステージ描画を初期化
    std::unique_ptr<SelectStageDrawer> stageDrawer_;

	//--------- functions ----------------------------------------------------

    // update
    void UpdateSelectInput();
};