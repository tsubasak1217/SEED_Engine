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

    // タイトル戻す
    bool IsReturnScene() const { return inputMapper_->IsTriggered(SelectInputEnum::Return); }
    bool IsDecideStage() const { return stageDrawer_->IsDecideStage(); }

    // 選択ステージ情報
    const SelectStageInfo& GetStageInfo() const { return stageDrawer_->GetStageInfo(); }

    // 入力を取得
    const InputMapper<SelectInputEnum>& GetInput() const { return *inputMapper_.get(); }
private:
	//========================================================================
	//	private Methods
	//========================================================================

	//--------- variables ----------------------------------------------------

    // 入力管理
    std::unique_ptr<InputMapper<SelectInputEnum>> inputMapper_;

    // ステージ描画を初期化
    std::unique_ptr<SelectStageDrawer> stageDrawer_;

    // 連続入力用
    LR holdDirection_ = LR::NONE;
    bool isRepeating_ = false;
    Timer holdTimer_;
    Timer repeatTimer_;

	//--------- functions ----------------------------------------------------

    // update
    void UpdateSelectInput();
};