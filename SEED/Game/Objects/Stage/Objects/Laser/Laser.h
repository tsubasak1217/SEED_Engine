#pragma once

//============================================================================
//	include
//============================================================================
#include <Game/Objects/Stage/Objects/Interface/ILaserObject.h>

//============================================================================
//	Laser class
//============================================================================
class Laser :
    public ILaserObject {
public:
    //========================================================================
    //	public Methods
    //========================================================================

    Laser(GameObject2D* owner) : ILaserObject(owner) {}
    Laser() = default;
    ~Laser() = default;

    void Initialize() override;

    void Update() override;

    void Draw() override;

    void Edit() override {};

    //--------- accessor -----------------------------------------------------

    void SetDirection(DIRECTION4 direction) override;

    // 状態の設定
    // 伸び始める
    void ReExtend() override;
    // 伸びるのを終了
    void StopExtend() override;

    // ヒット先のワープの情報を設定する
    void SetHitWarpParam(const WarpLaserParam& param) override;
private:
    //========================================================================
    //	private Methods
    //========================================================================

    //--------- variables ----------------------------------------------------

    //--------- functions ----------------------------------------------------

    // update
    void UpdateExtend();
    void UpdateStop();
};