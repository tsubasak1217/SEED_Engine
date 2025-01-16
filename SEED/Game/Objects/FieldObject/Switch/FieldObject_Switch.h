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
public:
    FieldObject_Switch();
    FieldObject_Switch(const std::string& modelName);
    ~FieldObject_Switch() = default;

    void Initialize() override;
    void Update() override;

    void RegisterObserver(IObserver* observer) override;
    void UnregisterObserver(IObserver* observer) override;
    void Notify(const std::string& event, void* data = nullptr) override;

    void OnCollision(Collider* collider, ObjectType objectType);

    void Toggle();  // スイッチの状態をトグルする

    //--- getter / setter ---//
    // ドアへのポインタを設定・取得するメソッド
    void AddAssociatedDoor(FieldObject_Door* door);
    const std::vector<FieldObject_Door*>& GetAssociatedDoors() const;

private:
    std::vector<IObserver*> observers_;
    bool isActivated_ = false;  // スイッチの状態
    // LoadFromJson 内でスイッチの関連情報を一時保存するための構造
    std::vector<std::tuple<FieldObject_Switch*, std::vector<int>>> switchDoorAssociations;

    std::vector<FieldObject_Door*> associatedDoors_;    // 紐付けられたドアへのポインタ
};

