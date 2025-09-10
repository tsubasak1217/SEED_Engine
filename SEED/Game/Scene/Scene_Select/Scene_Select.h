#pragma once

//============================================================================
//	include
//============================================================================
#include <SEED/Source/Basic/Scene/Scene_Base.h>

// c++
#include <memory>

// objects
#include <Game/Objects/Select/Background/SelectBackground.h>
#include <Game/Objects/Select/SelectStage.h>

// transition
#include <SEED/Source/Manager/SceneTransitionDrawer/SceneTransitionDrawer.h>
#include <SEED/Source/Basic/SceneTransition/BlockSlideTransition.h>
#include <SEED/Source/Basic/SceneTransition/HexagonTransition.h>

//============================================================================
//	Scene_Select class
//============================================================================
class Scene_Select :
    public Scene_Base {
public:
	//========================================================================
	//	public Methods
	//========================================================================

    Scene_Select();
	~Scene_Select();

    void Initialize() override;
    void Finalize() override {}

    void Update() override;

    void Draw() override;

    void BeginFrame() override;
    void EndFrame() override;

    void HandOverColliders() override {}
private:
	//========================================================================
	//	private Methods
	//========================================================================

	//--------- variables ----------------------------------------------------

    // 背景描画
    std::unique_ptr<SelectBackground> background_;
    // ステージ選択
    std::unique_ptr<SelectStage> selectStage_;

    // Audios
    const float kBGMVolume_ = 0.2f;
    AudioHandle bgmHandle_;

    // transition
    Timer backToTitleTimer_ = Timer(1.0f);
    Timer toGameSceneTimer_ = Timer(1.0f);
    bool isTransitionToGame_ = false;
    bool isBackToTitle_ = false;
    float titleSceneStartTime_ = 1.0f;
    float gameSceneStartTime_ = 0.5f;
    float hexagonSize_ = 32.0f;
    std::vector<Vector4> quadColors_;

	//--------- functions ----------------------------------------------------

};