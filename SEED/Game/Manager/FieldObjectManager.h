#pragma once

// engine

//local
#include "../FieldObject/FieldObject.h"
// lib
#include "../lib/patterns/ISubject.h"
#include <vector>
#include <memory>

class FieldObjectManager{
public:
    FieldObjectManager(ISubject& subject) : subject_(subject){}

    void Update();
    void Draw();

    // すべてのオブジェクトをクリア
    void ClearAllFieldObjects();

    // 新しいオブジェクトを追加
    void AddFieldObject(std::unique_ptr<FieldObject> obj);

    std::vector<std::unique_ptr<FieldObject>>& GetObjects(){ return fieldObjects_; }

private:
    std::vector<std::unique_ptr<FieldObject>> fieldObjects_;
    ISubject& subject_;
};
