#pragma once

//============================================================================
//	include
//============================================================================
#include <SEED/Lib/Structs/Sprite.h>
#include <Game/Objects/Player/State/PlayerStateController.h>

//============================================================================
//	Player class
//============================================================================
class Player {
public:
	//========================================================================
	//	public Methods
	//========================================================================

	Player() = default;
	~Player() = default;

    // 初期化処理
    void Initialize();

    // 更新処理
    void Update();

    // 描画処理
    void Draw();

    // エディター
    void Edit();

	//--------- accessor -----------------------------------------------------

    void SetTranslate(const Vector2& translate) { sprite_.translate = translate; }

    const Sprite& GetSprite() const { return sprite_; }
private:
	//========================================================================
	//	private Methods
	//========================================================================

	//--------- variables ----------------------------------------------------

     // jsonパス
    const std::string kJsonPath_ = "Player/playerParameter.json";

    // 描画情報
    Sprite sprite_;

    // 状態管理
    std::unique_ptr<PlayerStateController> stateController_;

	//--------- functions ----------------------------------------------------

    // json
    void ApplyJson();
    void SaveJson();
};