#pragma once
#include "../Activator/FieldObject_Activator.h"

class FieldObject_Switch : public FieldObject_Activator{
public:
    FieldObject_Switch();
    virtual ~FieldObject_Switch() = default;

    // フレーム開始時に重量判定を行い、起動／停止を決定する
    void Update() override;
    // Observerのリストを取得
};