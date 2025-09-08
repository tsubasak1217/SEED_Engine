#pragma once

//============================================================================
//	include
//============================================================================
#include <Game/Objects/Stage/Objects/Interface/IStageObject.h>
#include <Game/Objects/Stage/Objects/Player/Entity/Player.h>

//============================================================================
//	Goal class
//============================================================================
class Goal :
    public IStageObject {
public:
	//========================================================================
	//	public Methods
	//========================================================================

    Goal(GameObject2D* owner) : IStageObject(owner){}
    Goal() = default;
	~Goal() = default;

    void Initialize() override;

    void Update() override;

    void Draw() override;

    void Edit() override {};

    //--------- collision ----------------------------------------------------
    void OnCollisionEnter(GameObject2D* other) override;
    void OnCollisionStay(GameObject2D* other) override;
    void OnCollisionExit(GameObject2D* other) override;

	//--------- accessor -----------------------------------------------------
    void SetTargetPos(Vector2 targetPos) { targetPos_ = targetPos; }

private:
	//========================================================================
	//	private Methods
	//========================================================================

	//--------- variables ----------------------------------------------------

    static inline std::unordered_map<std::string, std::string> imageMap_{};

    // 王冠を取得する際の動きづくりに使用する変数
    Player* pPlayer_ = nullptr;
    Vector2 targetPos_;// 目標地点
    Vector2 startPos_;
    Vector2 savedPos_;
    float motionTimer_;

	//--------- functions ----------------------------------------------------
    void CrownMotion();
};