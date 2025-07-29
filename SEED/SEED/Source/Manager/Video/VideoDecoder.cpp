#include "VideoDecoder.h"

VideoManager::~VideoManager(){
}

void VideoManager::Initialize() {
    GetInstance();
}

const VideoManager* VideoManager::GetInstance() {
    if(instance_ == nullptr){
        instance_ = new VideoManager();
    }
    return instance_;
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// ビデオの読み込み
//////////////////////////////////////////////////////////////////////////////////////////////////
void VideoManager::LoadVideo(const std::string& filename){

    // 既に読み込まれていれば何もしない
    auto& self = *instance_;
    if(self.videoDatas_.find(filename) != self.videoDatas_.end()){
        return;
    }
}
