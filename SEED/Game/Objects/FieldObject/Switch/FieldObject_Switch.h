#pragma once
#include "../FieldObject.h"

#include "../../lib/patterns/ISubject.h"
#include "../../lib/patterns/IObserver.h"

// lib
#include <vector>


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

    void Toggle();  // スイッチの状態をトグルする

private:
    std::vector<IObserver*> observers_;
    bool isActivated_ = false;  // スイッチの状態
};

