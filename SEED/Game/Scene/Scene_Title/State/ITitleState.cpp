#include "ITitleState.h"

// host
#include "Scene_Title.h"

ITitleState::ITitleState(Scene_Title* host):
    host_(host){}

ITitleState::~ITitleState(){}

void ITitleState::EndFrame(){
    ManageState();
}
