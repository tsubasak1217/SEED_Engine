#pragma once
#include "../Activator/FieldObject_Activator.h"

/**
 * @brief レバーオブジェクト
 *
 * 重さ判定は無視し、触れている（=プレイヤー等と衝突している）状態のときに
 * ゲームパッドボタン（例：Aボタン）を押すとトグル動作を行う。
 */
class FieldObject_Lever 
    : public FieldObject_Activator{
public:
    FieldObject_Lever();
    virtual ~FieldObject_Lever() = default;

    // フレーム開始時
    virtual void BeginFrame() override;
    // フレーム終了時
    virtual void EndFrame() override;
    // 衝突時の処理（重さは扱わない）
    virtual void OnCollision(const BaseObject* other, ObjectType objectType) override;

private:
    // レバーをトグルする関数
    void Toggle();

    // レバーに触れているかどうか
    bool isTouched_ = false;
};