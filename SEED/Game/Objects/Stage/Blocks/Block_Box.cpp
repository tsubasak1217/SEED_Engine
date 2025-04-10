#include "Block_Box.h"

Block_Box::Block_Box(){
    Initialize();
}

void Block_Box::Initialize(){
    blockType_ = BlockType::Box;
    blockModel_ = std::make_unique<Model>("Assets/Box.obj");
    AdjustSize();
}

void Block_Box::SolveCollision(Block_Base* other){
    other;
}
