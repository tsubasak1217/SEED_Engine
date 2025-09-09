#include "SceneTransitionDrawer.h"

void SceneTransitionDrawer::Update(){

    if(!instance_){
        instance_ = new SceneTransitionDrawer();
    }

    if(instance_->transition_){
        if(instance_->transition_->isStart_){
            instance_->transition_->Update();
        }

        if(instance_->transition_->sceneInTimer_.IsFinished()){
            instance_->transition_.reset();
        }
    }
}

void SceneTransitionDrawer::Draw(){

    if(!instance_){
        instance_ = new SceneTransitionDrawer();
    }

    if(instance_->transition_){
        if(instance_->transition_->isStart_){
            instance_->transition_->Draw();
        }
    }

}