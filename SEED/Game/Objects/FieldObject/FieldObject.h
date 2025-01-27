#pragma once

// engine
#include "../SEED/lib/structs/Model.h"
#include "Base/BaseObject.h"

// lib
#include "../SEED/lib/tensor/Vector3.h"

#include "imgui.h"


// c++
#include <memory>
#include <string>
#include <cstdint>

class Player;

class FieldObject : public BaseObject{
public:
    FieldObject();
    FieldObject(const std::string& modelName);
    ~FieldObject()override = default;

    void Initialize()override;
    void Update()override;
    void Draw()override;

    virtual void ShowImGui(){};

    //--- getter / setter ---//
    Model* GetModel(){ return model_.get(); }
    uint32_t GetFieldObjectType()const{ return fieldObjectType_; }
    uint32_t GetFieldObjectID()const{ return fieldObjectID_; }
    void SetFieldObjectType(uint32_t type){ fieldObjectType_ = type; }
    void SetFieldObjectID(uint32_t id){ fieldObjectID_ = id; }
    void SetPlayer(Player* player){ player_ = player; }

protected:
    uint32_t fieldObjectType_;
    uint32_t fieldObjectID_ = 0;

    Player* player_ = nullptr;  // プレイヤーへのポインタ

};

