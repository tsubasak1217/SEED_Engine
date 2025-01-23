#include "PlayerCorpse.h"

PlayerCorpse::PlayerCorpse(){}

PlayerCorpse::~PlayerCorpse(){}

void PlayerCorpse::Initialize(){
    model_ = std::make_unique<Model>("dinosaur_corpse.obj");
    model_->UpdateMatrix();
}

void PlayerCorpse::Update(){
    BaseObject::Update();
}