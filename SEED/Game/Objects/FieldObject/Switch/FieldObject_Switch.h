#pragma once
#include "../FieldObject.h"

#include "../../lib/patterns/ISubject.h"
#include "../../lib/patterns/IObserver.h"

// lib
#include <vector>
#include <tuple>

class FieldObject_Door;

class FieldObject_Switch :
    public FieldObject,public ISubject{

    enum SwitchType{
        TYPE_TRIGGER,   //< 押した瞬間
        TYPE_HELD,      //< 押し続けている間
    };

public:
    // コンストラクタ
    FieldObject_Switch();
    FieldObject_Switch(const std::string& modelName, SwitchType type); // SwitchType を受け取るコンストラクタ
    ~FieldObject_Switch() = default;

    void Initialize() override;
    void Update() override;
    void ShowImGui() override;

    void RegisterObserver(IObserver* observer) override;
    void UnregisterObserver(IObserver* observer) override;
    void Notify(const std::string& event, void* data = nullptr) override;

    void OnCollision(const BaseObject* other, ObjectType objectType) override;

    void Toggle();  // スイッチの状態をトグルする

    //--- getter / setter ---//
    // ドアへのポインタを設定・取得するメソッド
    void AddAssociatedDoor(FieldObject_Door* door);
    void RemoveAssociatedDoor(FieldObject_Door* door);
    std::vector<FieldObject_Door*>& GetAssociatedDoors();

    // SwitchType の設定・取得
    void SetSwitchType(SwitchType type){ switchType_ = type; }
    SwitchType GetSwitchType() const{ return switchType_; }

    std::vector<IObserver*>& GetObservers(){ return observers_; }

private:
    std::vector<IObserver*> observers_;

    bool isActivated_ = false;  // スイッチの状態
    // LoadFromJson 内でスイッチの関連情報を一時保存するための構造
    std::vector<std::tuple<FieldObject_Switch*, std::vector<int>>> switchDoorAssociations;

    std::vector<FieldObject_Door*> associatedDoors_;    // 紐付けられたドアへのポインタ

    SwitchType switchType_ = SwitchType::TYPE_TRIGGER; // デフォルトを Trigger に設定

    // 衝突状態を追跡するための変数
    bool isColliding_ = false;            // 現在衝突しているか
    bool wasCollidingLastFrame_ = false;  // 前フレームで衝突していたか

public:
    static uint32_t nextFieldObjectID_;  // 次のID
};