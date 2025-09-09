#include "ISceneTransition.h"

void ISceneTransition::StartTransition(float sceneOutTime, float sceneInTime){
    sceneInTimer_ = Timer(sceneInTime);
    sceneOutTimer_ = Timer(sceneOutTime);
    isStart_ = true;
}
