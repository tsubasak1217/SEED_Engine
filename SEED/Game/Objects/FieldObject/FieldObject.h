#pragma once

// engine
#include <SEED/Lib/Structs/Model.h>
#include <Game/Objects/Base/BaseObject.h>

// lib
#include <SEED/Lib/Tensor/Vector3.h>

#include <imgui.h>


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
    virtual void DrawHud(){}
    virtual void ShowImGui(){};

    //--- getter / setter ---//
    Model* GetModel(){ return model_.get(); }
    uint32_t GetFieldObjectType()const{ return fieldObjectType_; }
    void SetFieldObjectType(uint32_t type){ fieldObjectType_ = type; }
    void SetPlayer(Player* player){ player_ = player; }
    bool GetRemoveFlag()const { return removeFlag_; }

    const std::string& GetGUID()const{ return guid_; }
    void SetGUID(const std::string& newGUID){ guid_ = newGUID; }

protected:
    uint32_t fieldObjectType_;
    bool removeFlag_ = false;
    std::string guid_;
    Player* player_ = nullptr;  // プレイヤーへのポインタ

    // カメラが来る位置
    Vector3 cameraPosition_ = {0.0f,0.0f,0.0f};
};

