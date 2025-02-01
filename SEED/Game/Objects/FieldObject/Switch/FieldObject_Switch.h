#pragma once
#include "../FieldObject.h"

#include "../../lib/patterns/ISubject.h"
#include "../../lib/patterns/IObserver.h"

// lib
#include <vector>
#include <tuple>

// 前方宣言
class FieldObject_Door;
class FieldObject_MoveFloor;

class FieldObject_Switch :
    public FieldObject, public ISubject{

    // スイッチのタイプを定義
    enum SwitchType{
        TYPE_TRIGGER,   // 押した瞬間のみ反応
        TYPE_HELD,      // 押し続けている間反応
    };

public:
    // コンストラクタ
    FieldObject_Switch();
    FieldObject_Switch(const std::string& modelName, SwitchType type); // SwitchTypeを指定するコンストラクタ
    ~FieldObject_Switch() = default;

    // 初期化関数
    void Initialize() override;

    // 更新関数
    void Update() override;
    // 描画関数
    void Draw() override;

    // 開始時処理
    void BeginFrame() override;
    // 終了時処理
    void EndFrame() override;

    // ImGui表示関数
    void ShowImGui() override;

    // Observer パターン用関数
    void RegisterObserver(IObserver* observer) override;
    void UnregisterObserver(IObserver* observer) override;
    void Notify(const std::string& event, void* data = nullptr) override;

    // 衝突検知関数
    void OnCollision(const BaseObject* other, ObjectType objectType) override;

    // スイッチの状態をトグルする関数
    void Toggle();

    //--- Getter / Setter ---//

    // ドアへのポインタを追加
    void AddAssociatedDoor(FieldObject_Door* door);

    // ドアへのポインタを削除
    void RemoveAssociatedDoor(FieldObject_Door* door);

    // 紐付けられたドアのポインタを取得
    std::vector<FieldObject_Door*>& GetAssociatedDoors();

    // 移動床へのポインタを追加
    void AddAssociatedMoveFloor(FieldObject_MoveFloor* moveFloor);

    // 移動床へのポインタを削除
    void RemoveAssociatedMoveFloor(FieldObject_MoveFloor* moveFloor);

    // 紐付けられた移動床のポインタを取得
    std::vector<FieldObject_MoveFloor*>& GetAssociatedMoveFloors();

    // SwitchTypeの設定
    void SetSwitchType(SwitchType type){ switchType_ = type; }

    // SwitchTypeの取得
    SwitchType GetSwitchType() const{ return switchType_; }

    // Observerのリストを取得
    std::vector<IObserver*>& GetObservers(){ return observers_; }

    // 必要な重量
    void SetRequiredWeight(int weight){ requiredWeight_ = weight; }
    int GetRequiredWeight()const{ return requiredWeight_; }    
    int* GetRequiredWeightPtr(){ return &requiredWeight_; }


private:
    // Observerリスト
    std::vector<IObserver*> observers_;

    // スイッチの状態
    bool isActivated_ = false;

    // LoadFromJson内でスイッチとドアの関連情報を一時保存
    std::vector<std::tuple<FieldObject_Switch*, std::vector<int>>> switchDoorAssociations;

    // 紐付けられたドアへのポインタ
    std::vector<FieldObject_Door*> associatedDoors_;

    // 紐付けられた移動床へのポインタ
    std::vector<FieldObject_MoveFloor*> associatedMoveFloors_;

    // スイッチタイプ（デフォルトはTrigger）
    SwitchType switchType_ = SwitchType::TYPE_TRIGGER;

    // 衝突状態を追跡する変数
    bool isColliding_ = false;            // 現在衝突しているか
    bool wasCollidingLastFrame_ = false;  // 前フレームで衝突していたか

    // 押すのに必要な重量
    int requiredWeight_;// 管理しやすくするためにint型で管理
    float currentWeight_;// 触れているオブジェクトの総重量はfloat型で管理
};
