#pragma once
#include "FieldObject/FieldObject.h"
#include <functional>
#include "Scene_Base.h"

class FieldObject_EventArea : public FieldObject {
public:
    FieldObject_EventArea();
    FieldObject_EventArea(const std::string& modelName);
    ~FieldObject_EventArea() = default;

public:
    void Initialize()override;
    void Draw()override;
    void BeginFrame()override;
    void SetEvent(std::function<void(Scene_Base*)> event) { event_ = event; }
    void OnCollision(const BaseObject* other, ObjectType objectType)override;

public:
    const std::string& GetEventName()const{ return eventName_; }
    void SetEventName(const std::string& eventName){ eventName_ = eventName; }

private:
    std::string eventName_;
    std::function<void(Scene_Base*)> event_;
    bool isCollidePlayer_ = false;
    bool preIsCollidePlayer_ = false;

public:
    bool isOnceEvent_ = false;
    static uint32_t nextFieldObjectID_;

};