#pragma once

//============================================================================
//	include
//============================================================================
#include <SEED/Lib/Structs/Sprite.h>

//============================================================================
//	BorderLine class
//============================================================================
class BorderLine {
public:
	//========================================================================
	//	public Methods
	//========================================================================

	BorderLine() = default;
	~BorderLine() = default;

    // 初期化処理
    void Initialize();

    // 更新処理
    void Update();

    // 描画処理
    void Draw();

    // エディター
    void Edit();

    // json
    void FromJson(const nlohmann::json& data);
    void ToJson(nlohmann::json& data);

	//--------- accessor -----------------------------------------------------

    void SetTranslate(const Vector2& translate) { sprite_.translate = translate; }

    bool IsActive() const { return currentState_ == State::Active; }
private:
	//========================================================================
	//	private Methods
	//========================================================================

	//--------- structure ----------------------------------------------------

    // 現在の状態
    enum class State {

        None,   // 表示なし
        Active, // アクティブ状態
    };

	//--------- variables ----------------------------------------------------

    // 現在の状態
    State currentState_;

    // 描画情報
    Sprite sprite_;

	//--------- functions ----------------------------------------------------

};