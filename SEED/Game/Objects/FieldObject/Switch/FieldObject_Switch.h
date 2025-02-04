#pragma once
#include "../Activator/FieldObject_Activator.h"

class FieldObject_Switch : public FieldObject_Activator{
public:
    FieldObject_Switch();
    virtual ~FieldObject_Switch() = default;
    void Draw() override;

    // フレーム開始時に重量判定を行い、起動／停止を決定する
    virtual void BeginFrame() override;
    void EndFrame() override;
    
    // 衝突時の処理
    void OnCollision(const BaseObject* other, ObjectType objectType) override;
};