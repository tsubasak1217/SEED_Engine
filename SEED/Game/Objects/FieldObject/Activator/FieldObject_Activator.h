#pragma once
#include "../FieldObject.h"
#include "../../lib/patterns/ISubject.h"
#include "../../lib/patterns/IObserver.h"
#include <vector>
#include <string>
#include <algorithm>

// 前方宣言
class FieldObject_Door;
class FieldObject_MoveFloor;

/**
 * @brief アクティベータの基底クラス
 *
 * スイッチやレバー等の、何らかの手段で起動／停止を行うオブジェクトの基底クラス。
 * 重量判定や Observer 通知などの共通機能を持つ。
 */
class FieldObject_Activator : public FieldObject, public ISubject{
public:
    FieldObject_Activator() = default;
    virtual ~FieldObject_Activator() = default;

    //--- FieldObject の基本関数 --------------------------------------//

    // Initialize（初期化）
    virtual void Initialize() override;
    // Update（更新）
    virtual void Update() override;
    // Draw（描画）
    virtual void Draw() override;
    // BeginFrame（フレーム開始）
    virtual void BeginFrame() override;
    // EndFrame（フレーム終了）
    virtual void EndFrame() override;
    // ShowImGui（ImGui表示）
    virtual void ShowImGui() override;

    //--- ISubject インターフェース ------------------------------------//

    // Observer を登録
    virtual void RegisterObserver(IObserver* observer) override;
    // Observer の登録を解除
    virtual void UnregisterObserver(IObserver* observer) override;
    // イベント通知
    virtual void Notify(const std::string& event, void* data = nullptr) override;

    std::vector<IObserver*>& GetObservers(){ return observers_; }

    //--- 衝突検知 ---------------------------------------------------//

    // 衝突したオブジェクトを受け取り、必要なら重量を加算する処理を行う
    virtual void OnCollision( BaseObject* other, ObjectType objectType) override;

    //--- ドア／移動床との関連付け -----------------------------------//

    // ドアを関連付ける
    void AddAssociatedDoor(FieldObject_Door* door);
    // ドアの関連付けを解除
    void RemoveAssociatedDoor(FieldObject_Door* door);
    // 関連付けられているドアのリストを取得
    std::vector<FieldObject_Door*>& GetAssociatedDoors();

    // 移動床を関連付ける
    void AddAssociatedMoveFloor(FieldObject_MoveFloor* moveFloor);
    // 移動床の関連付けを解除
    void RemoveAssociatedMoveFloor(FieldObject_MoveFloor* moveFloor);
    // 関連付けられている移動床のリストを取得
    std::vector<FieldObject_MoveFloor*>& GetAssociatedMoveFloors();

    //--- 重量判定に関するアクセサ ------------------------------------//

    // 必要な重量をセット
    void SetRequiredWeight(int weight){ requiredWeight_ = weight; }
    // 必要な重量を取得
    int GetRequiredWeight() const{ return requiredWeight_; }
    // ImGui等で操作しやすいようポインタを取得
    int* GetRequiredWeightPtr(){ return &requiredWeight_; }

protected:
    // Observer リスト
    std::vector<IObserver*> observers_;

    // アクティブ状態かどうか
    bool isActivated_ = false;

    // 紐付けられたドア／移動床へのポインタ
    std::vector<FieldObject_Door*> associatedDoors_;
    std::vector<FieldObject_MoveFloor*> associatedMoveFloors_;

    // 衝突状態を追跡するフラグ
    bool isColliding_ = false;            // 現在衝突しているかどうか
    bool wasCollidingLastFrame_ = false;  // 前のフレームで衝突していたかどうか

    // 重量関連
    int requiredWeight_ = 10;    // 押すのに必要な重量（例として10）
    float currentWeight_ = 0.0f; // 今フレームで衝突しているオブジェクトの総重量
};
