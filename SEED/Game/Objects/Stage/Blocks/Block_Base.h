#pragma once

// stl
#include <memory>
#include <cstdint>
#include <json.hpp>

// Engine
#include <SEED/Lib/Structs/Model.h>
#include <SEED/Lib/Tensor/Vector2i.h>
#include <SEED/Lib/Functions/MyFunc/MyMath.h>
#include <SEED/Lib/Functions/MyFunc/MyFunc.h>

// enum
#include <Game/Objects/Stage/Blocks/BlockType.h>


class Block_Base{
    friend class Stage;
public:
    Block_Base() = default;
    virtual ~Block_Base() = default;

public:
    virtual void Initialize();
    virtual void BeginFrame();
    virtual void Update();
    virtual void Draw();

public:
    void UpdateMatrix();
    void SetAddress(const Vector2i& address){ address_ = address; }
    void SetFirstAddress(const Vector2i& address){ firstAddress_ = address; }
    void AddAddress(const Vector2i& addValue){ address_ += addValue; }
    void ResetAddress(){ address_ = firstAddress_; }
    virtual void SolveCollision(Block_Base* other) = 0;

protected:
    void AdjustSize();
    virtual void Load(const nlohmann::json& json);
    virtual void Output(nlohmann::json& json);

public:
    Vector3 GetRotate()const{ return blockModel_->rotate_; }
    void SetRotate(const Vector3& rotate){ blockModel_->rotate_ = rotate; }
    void SetFirstPos(const Vector3& pos){ firstPos_ = pos; }
    Vector3 GetTranslate()const{ return blockModel_->translate_; }
    bool GetIsAlive(){ return isAlive_; }
    BlockType GetBlockType(){ return blockType_; }
    void SetIsAlive(bool isAlive){ isAlive_ = isAlive; }

protected:
    static uint32_t nextBlockID_;
    uint32_t blockID_;
    BlockType blockType_;
    Vector3 firstPos_;
    Vector2i firstAddress_;// 初期座標
    Vector2i address_;
    Vector2i preAddress_;
    static float baseBlockSize_;
    std::unique_ptr<Model> blockModel_;
    bool isAlive_ = true;
};