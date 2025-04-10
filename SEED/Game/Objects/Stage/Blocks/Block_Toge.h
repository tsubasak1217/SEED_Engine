#pragma once
#include <Game/Objects/Stage/Blocks/Block_Base.h>

class Block_Toge : public Block_Base{
public:
    Block_Toge();
    ~Block_Toge()override = default;

public:
    void Initialize() override;
    void SolveCollision(Block_Base* other) override;
};