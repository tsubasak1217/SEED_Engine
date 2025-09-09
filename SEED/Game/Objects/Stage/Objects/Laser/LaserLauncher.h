#pragma once

//============================================================================
//	include
//============================================================================
#include <SEED/Lib/enums/Direction.h>
#include <SEED/Lib/Functions/myFunc/Easing.h>
#include <SEED/Lib/Structs/Timer.h>
#include <Game/Objects/Stage/Objects/Interface/IStageObject.h>
#include <Game/Objects/Stage/Objects/Laser/Laser.h>

// c++
#include <span>

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

    //--------- accessor -----------------------------------------------------

    void SetTranslate(const Vector2& translate) override;
    void SetSize(const Vector2& size) override;
    void SetScale(float scale) override;
    void SetCommonState(StageObjectCommonState state) override;
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

    //--------- structure ----------------------------------------------------

    // 発射状態
    enum class State {

        Emit,            // 発射中...
        EmitNotPossible, // 発生不可(真隣になにかあってレーザーが進めない)
    };

    // 発生位置
    struct LaserUnit {

        State currentState;                  // 現在の状態
        DIRECTION4 direction;                // 方向
        std::vector<Sprite> laserUnitSprite; // 表示スプライト
        std::vector<Timer> timers;           // 時間管理
        int spriteCount;                     // 使用スプライト数
        float phaseSpacing;                  // 発生間隔
        float gapFromBlock;                  // 発射台との距離
        float moveDistance;                  // 各方向の移動量

        float duration;               // 補間にかける時間
        Easing::Type scalineEasing;   // スケーリング処理を行う際のイージング
        Easing::Type translateEasing; // 座標移動処理を行う際のイージング

        // 補間に使用する値
        // スケール
        float startScale;
        float targetScale;
    };

    //--------- variables ----------------------------------------------------

    // 値保持用
    Vector2 laserSize_;    // レーザーのサイズ
    Vector2 translate_;    // 発射台の座標

    // 発射台スプライト
    Sprite frameSprite_;

    // 中心の星
    Sprite centerStarSprite_;
    Timer centerStarTimer_; // 時間経過
    bool isStarGrowing_;    // 拡大中か

    // 発射方向
    uint8_t bitDirection_;
    std::vector<DIRECTION4> launchDirections_;

    // レーザー本体
    std::list<GameObject2D*> lasers_;
    std::list<GameObject2D*> warpedLasers_;

    // ユニット
    // 方向ごとに表示する
    LaserUnit commonUnit_;         // 共通パラメータ
    std::vector<LaserUnit> units_; // commonUnit_で調節した値で動かす

    //--------- functions ----------------------------------------------------

    // update
    void UpdateCenterStar();
    void UpdateUnits();
    void UpdateEmit(LaserUnit& unit);
    void UpdateEmitNotPossible(LaserUnit& unit);

    // helper
    bool IsDirectionReadyForUnit(const LaserUnit& unit) const;
};