#include "EventScene.h"

EventScene::~EventScene(){

    delete inScene_;
    delete outScene_;
    inScene_ = nullptr;
    outScene_ = nullptr;
}
