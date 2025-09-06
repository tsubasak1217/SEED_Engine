#pragma once

//============================================================================
//	include
//============================================================================
#include <SEED/Lib/enums/Direction.h>
#include <Game/Objects/Stage/Objects/Interface/IStageObject.h>
#include <Game/Objects/Stage/Objects/Laser/Laser.h>

//============================================================================
//	LaserLauncher class
//============================================================================
class LaserLauncher :
    public IStageObject {
public:
    //========================================================================
    //	public Methods
    //========================================================================

    LaserLauncher(GameObject2D* owner) : IStageObject(owner) {}
    LaserLauncher() = default;
    ~LaserLauncher();

    void Initialize() override;
    void InitializeLaunchSprites();
    void InitializeLasers();

    void Update() override;

    void Draw() override;

    void Edit() override;

    // ワープレーザー処理
    void WarpLaserFromController(const Vector2& translate, const GameObject2D* sourceLaserObject);
    // ワープ後にできたオブジェクトを破棄
    void RemoveWarpLasers();
    // 所持ているレーザーに対してアクティブを設定する
    void SetIsActive(bool isActive);

    //--------- accessor -----------------------------------------------------

    void SetTranslate(const Vector2& translate) override;
    void SetSize(const Vector2& size) override;
    void SetIsLaserActive(bool isActive);

    // 発射方向を設定
    void SetLaunchDirections(uint8_t directions);

    const Vector2& GetTranslate() const override { return translate_; }

    // 方向をビット値で取得
    uint8_t GetBitDirection() const { return bitDirection_; }
    // レーザーを取得する
    const std::list<GameObject2D*> GetLasers() const { return lasers_; }
private:
    //========================================================================
    //	private Methods
    //========================================================================

    //--------- variables ----------------------------------------------------

    // 値保持用
    std::string fileName_; // スプライト
    Vector2 laserSize_;    // レーザーのサイズ
    Vector2 translate_;    // 発射台の座標

    // 発射台スプライト
    std::vector<Sprite> launchSprites_;
    // 発射方向
    uint8_t bitDirection_;
    std::vector<DIRECTION4> launchDirections_;

    // レーザー本体
    std::list<GameObject2D*> lasers_;
    std::list<GameObject2D*> warpedLasers_;

    //--------- functions ----------------------------------------------------


};