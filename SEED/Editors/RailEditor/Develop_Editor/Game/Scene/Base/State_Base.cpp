#include <State_Base.h>
#include <Scene_Base.h>
#include <SEED.h>

void State_Base::ShiftState(State_Base* nextState){
    pScene_->ChangeState(nextState);
}
