#pragma once

//============================================================================
//	include
//============================================================================
#include "../IPostProcess.h"

//============================================================================
//	Bloom class
//============================================================================
class Bloom :
    public IPostProcess {
public:
    //========================================================================
    //	public Methods
    //========================================================================

    Bloom();
    ~Bloom() = default;

    void Initialize() override;

    void Update() override;

    void Apply() override;
    void Release() override;

    void StartTransition() override {};
    void EndTransition() override {};

    void Edit() override;

    nlohmann::json ToJson() override;
    void FromJson(const nlohmann::json& json) override;

    //--------- accessor -----------------------------------------------------

private:
    //========================================================================
    //	private Methods
    //========================================================================

    //--------- variables ----------------------------------------------------

    float threshold_ = 1.0f; // 閾値
    int radius_ = 2;         // ブラー半径
    float sigma_ = 64.0f;    // 強度
};