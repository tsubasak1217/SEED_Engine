#pragma once
#include "../Activator/FieldObject_Activator.h"
#include "Sprite.h"

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
    void DrawHud() override;
private:
    // レバーをトグルする関数
    void Toggle();

    void UpdateHud();

    // レバーに触れているかどうか
    bool isTouched_ = false;

    std::unique_ptr<Sprite> hud_;
};