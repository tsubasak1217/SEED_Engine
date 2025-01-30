#pragma once
#include "../FieldObject.h"
#include "State/DoorState.h"
#include "../lib/patterns/IObserver.h"
#include <memory>
#include <string>

// 前方宣言
class FieldObject_Switch;

class FieldObject_Door
    : public FieldObject, public IObserver{

    enum Door_State{
        DOORSTATE_OPENING,
        DOORSTATE_CLOSING,
        DOORSTATE_OPENED,
        DOORSTATE_CLOSED
    };

public:
    FieldObject_Door();
    FieldObject_Door(const std::string& modelName);
    ~FieldObject_Door()override;

    void Initialize() override;
    void Update() override;
    void Draw() override;

    void ShowImGui() override;

    // IObserver の実装
    void OnNotify(const std::string& event, void* data = nullptr) override;

    // 状態変更用メソッド
    void ChangeState(DoorState* newState);

    // --- getter / setter --- //
    bool GetIsOpened() const{ return isOpened_; }
    void SetIsOpened(bool isOpened);
    float GetOpenSpeed() const{ return openSpeed_; }
    float GetMaxOpenHeight() const{ return closedPosY_ + kMaxOpenHeight_; }

    void SetSwitch(FieldObject_Switch* pSwitch);
    void RemoveSwitch(FieldObject_Switch* pSwitch);
    bool GetHasSwitch() const{ return hasSwitch_; }

    void SetClosedPosY(float posY){ closedPosY_ = posY; }
    float GetClosedPosY() const{ return closedPosY_; }

private:
    bool isOpened_ = false;                     // 開閉状態のフラグ
    bool hasSwitch_ = false;                    // スイッチを持っているかどうか
    std::unique_ptr<DoorState> currentState_;   // 現在の状態
    float openSpeed_ = 8.0f;                    // 開閉速度
    const float kMaxOpenHeight_ =5.0f;          // 最大開く高さ

    float closedPosY_ = 0.0f;                   // 閉じた後のy座標
};
