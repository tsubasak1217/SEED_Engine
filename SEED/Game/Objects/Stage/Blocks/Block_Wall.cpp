#include "Block_Wall.h"

Block_Wall::Block_Wall(){
    Initialize();
}

void Block_Wall::Initialize(){
    blockType_ = BlockType::Wall;
    blockModel_ = std::make_unique<Model>("Assets/Tile.obj");
    AdjustSize();
}

void Block_Wall::SolveCollision(Block_Base* other){
    other;
}
