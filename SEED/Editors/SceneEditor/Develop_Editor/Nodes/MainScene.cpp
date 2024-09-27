#include "MainScene.h"

MainScene::~MainScene(){

    delete inScene_;
    delete outScene_;
    inScene_ = nullptr;
    outScene_ = nullptr;

    for(auto& eventScene : eventScenes_){
        delete eventScene;
        eventScene = nullptr;
    }

    eventScenes_.clear();
}
