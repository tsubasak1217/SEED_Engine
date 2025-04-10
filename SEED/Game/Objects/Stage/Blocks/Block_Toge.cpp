#include "Block_Toge.h"

Block_Toge::Block_Toge(){
    Initialize();
}

void Block_Toge::Initialize(){
    blockType_ = BlockType::Toge;
    blockModel_ = std::make_unique<Model>("Assets/Toge.obj");
    AdjustSize();
}

void Block_Toge::SolveCollision(Block_Base* other){
    other;
}
