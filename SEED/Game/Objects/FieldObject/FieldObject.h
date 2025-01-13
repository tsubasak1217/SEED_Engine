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

class FieldObject : public BaseObject{
public:
    FieldObject();
    FieldObject(const std::string& modelName);
    ~FieldObject() = default;

    void Initialize()override;
    void Update()override;
    void Draw()override;

    //--- getter / setter ---//
    Model* GetModel(){ return model_.get(); }
    uint32_t GetFieldObjectType()const{ return fieldObjectType_; }
    void SetFieldObjectType(uint32_t type){ fieldObjectType_ = type; }

protected:
    uint32_t fieldObjectType_;

};

