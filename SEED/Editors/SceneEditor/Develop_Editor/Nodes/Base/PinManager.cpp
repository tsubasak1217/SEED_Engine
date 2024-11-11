#include "PinManager.h"
#include <cassert>

// Nodes
#include <BaseNode.h>
#include <MainScene.h>
#include <InScene.h>
#include <OutScene.h>
#include <EventScene.h>

// static Initialize
uint32_t PinManager::pinID_next_ = 1;
std::unordered_map<uint32_t, Pin> PinManager::pins_;

/////////////////////////////////////////////////////////
//       接続
/////////////////////////////////////////////////////////
bool PinManager::Connect(uint32_t startPin, uint32_t endPin){

    // 指定ピンが無ければアサート
    if(pins_.find(startPin) == pins_.end()){ assert(false); }
    if(pins_.find(endPin) == pins_.end()){ assert(false); }

    if(pins_[startPin].nodeType == NodeType::MAIN_SCENE){
        return Link_MainScene(startPin,endPin);

    } else if(pins_[startPin].nodeType == NodeType::IN_SCENE){
        return Link_InScene(startPin,endPin);

    } else if(pins_[startPin].nodeType == NodeType::OUT_SCENE){
        return Link_OutScene(startPin,endPin);

    } else if(pins_[startPin].nodeType == NodeType::EVENT_SCENE){
        return Link_EventScene(startPin,endPin);

    }

    return false;
}


/////////////////////////////////////////////////////////
//       それぞれの場合の処理
/////////////////////////////////////////////////////////

bool PinManager::Link_MainScene(uint32_t startPin,uint32_t endPin){

    // 今のシーンポインタを取得
    MainScene* pCurrentScene = dynamic_cast<MainScene*>(pins_[startPin].pNode);

    // 接続先のピンに応じた処理
    if(pins_[endPin].nodeType == NodeType::MAIN_SCENE){// メインシーン---------------

        MainScene* pNextScene = dynamic_cast<MainScene*>(pins_[endPin].pNode);
        pCurrentScene->pNextScenes_.push_back(pNextScene);
        pNextScene->pPrevScenes_.push_back(pCurrentScene);
        return true;

    } else if(pins_[endPin].nodeType == NodeType::IN_SCENE){// 開始シーン---------------

        InScene* pInScene = dynamic_cast<InScene*>(pins_[endPin].pNode);
        pCurrentScene->inScene_ = pInScene;
        return true;

    } else if(pins_[endPin].nodeType == NodeType::OUT_SCENE){// 終了シーン---------------

        OutScene* pOutScene = dynamic_cast<OutScene*>(pins_[endPin].pNode);
        pCurrentScene->outScene_ = pOutScene;
        return true;

    } else if(pins_[endPin].nodeType == NodeType::EVENT_SCENE){// イベントシーン---------------

        EventScene* pEventScene = dynamic_cast<EventScene*>(pins_[endPin].pNode);
        pCurrentScene->eventScenes_.push_back(pEventScene);
        pEventScene->pPrevScene_ = pCurrentScene;
        return true;
    }

    return false;
}

bool PinManager::Link_InScene(uint32_t startPin,uint32_t endPin){

    // 今のシーンポインタを取得
    InScene* pCurrentScene = dynamic_cast<InScene*>(pins_[startPin].pNode);

    if(pins_[endPin].nodeType == NodeType::MAIN_SCENE){

        MainScene* pMainScene = dynamic_cast<MainScene*>(pins_[endPin].pNode);
        pCurrentScene->pAttachScene_ = pMainScene;
        pMainScene->inScene_ = pCurrentScene;
        return true;

    } else if(pins_[endPin].nodeType == NodeType::IN_SCENE){
        return false;
    } else if(pins_[endPin].nodeType == NodeType::OUT_SCENE){
        return false;
    } else if(pins_[endPin].nodeType == NodeType::EVENT_SCENE){

        EventScene* pEventScene = dynamic_cast<EventScene*>(pins_[endPin].pNode);
        pCurrentScene->pAttachScene_ = pEventScene;
        pEventScene->inScene_ = pCurrentScene;
        return true;
    }

    return false;
}

bool PinManager::Link_OutScene(uint32_t startPin,uint32_t endPin){

    // 今のシーンポインタを取得
    OutScene* pCurrentScene = dynamic_cast<OutScene*>(pins_[startPin].pNode);

    if(pins_[endPin].nodeType == NodeType::MAIN_SCENE){

        MainScene* pMainScene = dynamic_cast<MainScene*>(pins_[endPin].pNode);
        pCurrentScene->pAttachScene_ = pMainScene;
        pMainScene->outScene_ = pCurrentScene;
        return true;

    } else if(pins_[endPin].nodeType == NodeType::IN_SCENE){
        return false;
    } else if(pins_[endPin].nodeType == NodeType::OUT_SCENE){
        return false;
    } else if(pins_[endPin].nodeType == NodeType::EVENT_SCENE){

        EventScene* pEventScene = dynamic_cast<EventScene*>(pins_[endPin].pNode);
        pCurrentScene->pAttachScene_ = pEventScene;
        pEventScene->outScene_ = pCurrentScene;
        return true;
    }

    return false;
}

bool PinManager::Link_EventScene(uint32_t startPin,uint32_t endPin){

    // 今のシーンポインタを取得
    EventScene* pCurrentScene = dynamic_cast<EventScene*>(pins_[startPin].pNode);

    if(pins_[endPin].nodeType == NodeType::MAIN_SCENE){

        MainScene* pNextScene = dynamic_cast<MainScene*>(pins_[endPin].pNode);
        pCurrentScene->pNextScenes_.push_back(pNextScene);
        pNextScene->pPrevScenes_.push_back(pCurrentScene);
        return true;

    } else if(pins_[endPin].nodeType == NodeType::IN_SCENE){// 開始シーン---------------

        InScene* pInScene = dynamic_cast<InScene*>(pins_[endPin].pNode);
        pCurrentScene->inScene_ = pInScene;
        return true;

    } else if(pins_[endPin].nodeType == NodeType::OUT_SCENE){// 終了シーン---------------

        OutScene* pOutScene = dynamic_cast<OutScene*>(pins_[endPin].pNode);
        pCurrentScene->outScene_ = pOutScene;
        return true;

    } else if(pins_[endPin].nodeType == NodeType::EVENT_SCENE){
        return false;
    }

    return false;
}

/////////////////////////////////////////////////////////
//       解除
/////////////////////////////////////////////////////////

void PinManager::DisConnect(uint32_t pin1, uint32_t pin2){
    
    // 指定ピンが無ければアサート
    if(pins_.find(pin1) == pins_.end()){assert(false);}
    if(pins_.find(pin2) == pins_.end()){assert(false);}

    if(pins_[pin1].nodeType == NodeType::MAIN_SCENE){

    } else if(pins_[pin1].nodeType == NodeType::IN_SCENE){

    } else if(pins_[pin1].nodeType == NodeType::OUT_SCENE){

    } else if(pins_[pin1].nodeType == NodeType::EVENT_SCENE){

    } else if(pins_[pin1].nodeType == NodeType::BUTTON_EVENT){

    } else{
    }

}
