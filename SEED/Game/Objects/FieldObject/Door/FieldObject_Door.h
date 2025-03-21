#pragma once
#include "../FieldObject.h"
#include "State/DoorState.h"
#include "../lib/patterns/IObserver.h"
#include <memory>
#include <string>

// 前方宣言
class FieldObject_Activator;

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
    void SetOpenSpeed(float speed){ openSpeed_ = speed; }
    float GetMaxOpenHeight() const{ return closedPosY_ + kMaxOpenHeight_; }
    float GetOpenHeight() const{ return kMaxOpenHeight_; }
    void SetOpenHeight(float height){ kMaxOpenHeight_ = height; }

    void SetActivator(FieldObject_Activator* pActivator);
    void RemoveActivator(FieldObject_Activator* pActivator);
    bool GetHasActivator() const{ return hasActivator_; }

    void SetClosedPosY(float posY){ closedPosY_ = posY; }
    float GetClosedPosY() const{ return closedPosY_; }

    bool GetShouldPerformCameraView() const{ return shouldPerformCameraView_; }
    void SetShouldPerformCameraView(bool shouldPerform){ shouldPerformCameraView_ = shouldPerform; }

    bool GetCameraViewActive() const{ return cameraViewActive_; }
    void SetCameraViewActive(bool isActive){ cameraViewActive_ = isActive; }

private:

private:
    bool shouldPerformCameraView_ = false;      //< カメラ演出を行うかどうか
    bool cameraViewActive_ = false;             //< カメラ演出中かどうか
    bool isOpened_ = false;                     //< 開閉状態のフラグ
    bool hasActivator_ = false;                 //< スイッチを持っているかどうか
    std::unique_ptr<DoorState> currentState_;   //< 現在の状態
    float openSpeed_ = 8.0f;                    //< 開閉速度
    float kMaxOpenHeight_ =5.0f;                //< 最大開く高さ
    float closedPosY_ = 0.0f;                   //< 閉じた後のy座標
    int stageNum_ = 0;                          //< ステージ番号      
    std::unique_ptr<BaseObject> cameraTarget_ = nullptr;        //< カメラ移動ターゲット
};
