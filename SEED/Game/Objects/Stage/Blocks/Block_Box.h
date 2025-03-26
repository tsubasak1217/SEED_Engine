#pragma once
#include <Game/Objects/Stage/Blocks/Block_Base.h>

class Block_Box : public Block_Base{
public:
    Block_Box();
    ~Block_Box()override = default;

public:
    void Initialize() override;
};