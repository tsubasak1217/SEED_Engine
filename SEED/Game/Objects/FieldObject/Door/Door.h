#pragma once
#include "../FieldObject.h"
#include "State/DoorState.h"
#include "../lib/patterns/IObserver.h"
#include <memory>
#include <string>

class Door 
    : public FieldObject, public IObserver{
public:
    Door(const std::string& modelName);
    virtual ~Door();

    void Initialize() override;
    void Update() override;
    void Draw() override;

    // IObserver の実装
    void OnNotify(const std::string& event, void* data = nullptr) override;

    //--- getter / setter ---//
    bool GetIsOpened() const{ return isOpened_; }
    void SetIsOpened(bool isOpened);

    // 状態変更用メソッド
    void ChangeState(DoorState* newState);

    // 状態クラスからアクセスするためのメソッド
    Model* GetModel(){ return model_.get(); }
    float GetOpenSpeed() const{ return openSpeed_; }
    float GetMaxOpenHeight() const{ return kMaxOpenHeight_; }

private:
    bool isOpened_ = false;  // 開閉状態のフラグ

    std::unique_ptr<DoorState> currentState_;

    float openSpeed_ = 1.0f;            // 開閉速度
    const float kMaxOpenHeight_ = 3.0f; // 最大開く高さ
};
