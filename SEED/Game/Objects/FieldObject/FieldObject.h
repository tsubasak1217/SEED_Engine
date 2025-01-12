#pragma once

// engine
#include "../SEED/lib/structs/Model.h"

// lib
#include "../SEED/lib/tensor/Vector3.h"

#include "imgui.h"

// c++
#include <memory>
#include <string>
#include <cstdint>

class FieldObject{
public:
    FieldObject(const std::string& modelName);
    virtual ~FieldObject() = default;

    virtual void Initialize(){};
    virtual void Update();
    virtual void Draw();

    //--- getter / setter ---//
    Model* GetModel(){ return model_.get(); }

    void SetPosition(const Vector3& pos){ model_->translate_ = pos; }
    void SetScale(const Vector3& scale){ model_->scale_ = scale; }
    void SetRotation(const Vector3& rotation){ model_->rotate_ = rotation; }
    void SetColor(const Vector4& color){ model_->color_ = color; }

    const std::string& GetName()const{ return name_; }

protected:
    //表示用モデル
    std::unique_ptr<Model> model_;

    std::string name_;
};

