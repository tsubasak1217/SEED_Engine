#include "ISceneTransition.h"

void ISceneTransition::StartTransition(float curSceneExitTime, float newSceneEnterTime){
    curSceneExitTimer_ = Timer(curSceneExitTime);
    newSceneEnterTimer_ = Timer(newSceneEnterTime);
    isStart_ = true;
}
