#pragma once
#include <Game/Objects/Stage/Blocks/Block_Base.h>

class Block_Wall : public Block_Base{
public:
    Block_Wall();
    ~Block_Wall()override = default;

public:
    void Initialize() override;
};