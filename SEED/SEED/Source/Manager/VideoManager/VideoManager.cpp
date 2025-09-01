#include "VideoManager.h"
#include <SEED/Lib/Structs/VideoPlayer.h>
#include <SEED/Source/Manager/DxManager/ViewManager.h>

////////////////////////////////////////////////////////////
// インスタンスの取得
////////////////////////////////////////////////////////////
VideoManager* VideoManager::GetInstance(){
    if(!instance_){
        instance_ = new VideoManager();
    }
    return instance_;
}

////////////////////////////////////////////////////////////
// 更新処理
////////////////////////////////////////////////////////////
void VideoManager::Update(){
    // 各VideoPlayerの更新
    for(auto& videoPlayer : videoItems_){
        if(videoPlayer.first){
            // スキップ条件
            if(videoPlayer.second.removeOrder){ continue; }
            if(!videoPlayer.first->context_.isPlaying){ continue; }

            // 必要であれば音声再生の開始
            if(!videoPlayer.first->context_.isAudioStarted && videoPlayer.first->context_.isUseAudio){
                videoPlayer.first->StartAudio(); // 音声の開始
            }

            // フレームの更新
            videoPlayer.first->Update();
        }
    }
    // フレーム終了処理
    EndFrame();
}

////////////////////////////////////////////////////////////
// 解放
////////////////////////////////////////////////////////////
void VideoManager::Release(){

    for(auto& videoPlayer : videoItems_){
        if(videoPlayer.first){
            // viewの解放
            if(videoPlayer.second.frameTextureY_.GetSRVIndex() != -1){
                ViewManager::UnloadView(HEAP_TYPE::SRV_CBV_UAV, videoPlayer.second.frameTextureY_.GetSRVIndex());
            }
            if(videoPlayer.second.frameTextureUV_.GetSRVIndex() != -1){
                ViewManager::UnloadView(HEAP_TYPE::SRV_CBV_UAV, videoPlayer.second.frameTextureUV_.GetSRVIndex());
            }
            if(videoPlayer.second.frameTextureRGBA_.GetUAVIndex() != -1){
                ViewManager::UnloadView(HEAP_TYPE::SRV_CBV_UAV, videoPlayer.second.frameTextureRGBA_.GetUAVIndex());
            }
            if(videoPlayer.second.audioHandle.has_value()){
                AudioManager::EndAudio(videoPlayer.second.audioHandle.value()); // 音声の終了
            }
        }
    }

    // 全ての動画アイテムを削除
    videoItems_.clear();

    if(instance_){
        delete instance_;
        instance_ = nullptr;
    }
}

////////////////////////////////////////////////////////////
// フレーム終了処理
////////////////////////////////////////////////////////////
void VideoManager::EndFrame(){
    // 削除フラグが立っているものを削除
    for(auto it = videoItems_.begin(); it != videoItems_.end();){
        if(it->second.removeOrder){
            // viewの解放
            if(it->second.frameTextureY_.GetSRVIndex() != -1){
                ViewManager::UnloadView(HEAP_TYPE::SRV_CBV_UAV, it->second.frameTextureY_.GetSRVIndex());
            }
            if(it->second.frameTextureUV_.GetSRVIndex() != -1){
                ViewManager::UnloadView(HEAP_TYPE::SRV_CBV_UAV, it->second.frameTextureUV_.GetSRVIndex());
            }
            if(it->second.frameTextureRGBA_.GetUAVIndex() != -1){
                ViewManager::UnloadView(HEAP_TYPE::SRV_CBV_UAV, it->second.frameTextureRGBA_.GetUAVIndex());
            }

            it = videoItems_.erase(it); // 削除後は次の要素へ
        } else{
            ++it; // 削除しない場合は次の要素へ
        }
    }
}


////////////////////////////////////////////////////////////
// Videoの登録関数
////////////////////////////////////////////////////////////
void VideoManager::Register(VideoPlayer* video){
    // あるかどうか確認
    if(videoItems_.find(video) != videoItems_.end()){
        return; // 既に登録されている場合は何もしない
    }

    // 新しいVideoItemsを作成
    VideoItems videoItem;
    videoItems_.insert({ video, videoItem });
}