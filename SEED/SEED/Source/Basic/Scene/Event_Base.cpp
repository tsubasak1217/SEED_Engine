#include "Event_Base.h"

Event_Base::Event_Base(Scene_Base* parentScene){
    pScene_ = parentScene;
}

Event_Base::~Event_Base(){
}

void Event_Base::Initialize(){
}

void Event_Base::Update(){
}

void Event_Base::Draw(){
}
