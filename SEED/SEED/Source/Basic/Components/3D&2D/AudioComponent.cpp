#include "AudioComponent.h"
#include <SEED/Source/Basic/Object/GameObject.h>
#include <SEED/Source/Basic/Object/GameObject2D.h>

/////////////////////////////////////////////////////////////////////////////////////////////////////
// コンストラクタ
/////////////////////////////////////////////////////////////////////////////////////////////////////
AudioComponent::AudioComponent(std::variant<GameObject*, GameObject2D*> pOwner, const std::string& tagName)
    : IComponent(pOwner, tagName){
    if(tagName == ""){
        componentTag_ = "Audio_ID:" + std::to_string(componentID_);
    }
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
// 初期化
/////////////////////////////////////////////////////////////////////////////////////////////////////
void AudioComponent::Initialize(){
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
// フレーム開始時処理
/////////////////////////////////////////////////////////////////////////////////////////////////////
void AudioComponent::BeginFrame(){
}


/////////////////////////////////////////////////////////////////////////////////////////////////////
// 更新処理
/////////////////////////////////////////////////////////////////////////////////////////////////////
void AudioComponent::Update(){

    for(int32_t i = 0; i < audioList_.size(); i++){

        if(audioList_[i].filePath.empty()){
            continue;
        }

        // 再生判断
        if(!audioList_[i].isPlayed){
            audioList_[i].playDelayTimer.Update();

            // 音源の再生開始
            if(audioList_[i].playDelayTimer.duration == 0.0f or audioList_[i].playDelayTimer.IsFinishedNow()){
                audioList_[i].handle = AudioManager::PlayAudio(
                    audioList_[i].filePath,
                    audioList_[i].isLoop,
                    audioList_[i].volume,
                    audioList_[i].startTime
                );

                // 再生タイマーの初期化
                audioList_[i].playTimer.Initialize(
                    AudioManager::GetDuration(audioList_[i].filePath),
                    audioList_[i].startTime
                );

                // 再生済みにする
                audioList_[i].isPlayed = true;
            }

        } else{
            // 再生タイマーの更新
            audioList_[i].playTimer.Update(1.0f, audioList_[i].isLoop);
        }
    }
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
// 描画処理
/////////////////////////////////////////////////////////////////////////////////////////////////////
void AudioComponent::Draw(){
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
// フレーム終了時処理
/////////////////////////////////////////////////////////////////////////////////////////////////////
void AudioComponent::EndFrame(){
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
// 終了処理
/////////////////////////////////////////////////////////////////////////////////////////////////////
void AudioComponent::Finalize(){
}



/////////////////////////////////////////////////////////////////////////////////////////////////////
// 編集処理
/////////////////////////////////////////////////////////////////////////////////////////////////////
void AudioComponent::EditGUI(){
#ifdef _DEBUG
    ImGui::Indent();
    static std::string hash = "";

    for(int32_t i = 0; i < audioList_.size(); i++){

        // 一意なハッシュ値を生成
        hash = MyFunc::PtrToStr(&audioList_[i]);

        // 削除処理
        if(ImGui::Button("削除##" + hash)){
            audioList_.erase(audioList_.begin() + i);
            break;
        }

        // 音声項目の編集ヘッダー
        ImGui::SameLine();
        if(ImFunc::CollapsingHeader("音声" + std::to_string(i) + "##" + hash)){
            ImGui::Indent();

            // フォルダビューで音声ファイルを選択
            static const std::filesystem::path defaultPath = "Resources/Audios/";
            static std::filesystem::path currentPath = defaultPath;
            std::string selected = ImFunc::FolderView("音声ファイル選択##" + hash, currentPath, { ".wav", ".mp3" }, defaultPath);

            // ファイルが選択されたら音声をセット
            if(!selected.empty()){
                audioList_[i].filePath = selected;
                audioList_[i].handle = AudioManager::PlayAudio(
                    audioList_[i].filePath,
                    audioList_[i].isLoop,
                    audioList_[i].volume,
                    audioList_[i].startTime
                );

                audioList_[i].filePath = selected;
                audioList_[i].playTimer.Initialize(
                    AudioManager::GetDuration(audioList_[i].filePath),
                    audioList_[i].startTime
                );
            }

            if(ImFunc::PlayBar("再生バー##" + hash, audioList_[i].playTimer)){

                // 再生位置を範囲内に収める
                audioList_[i].playTimer.currentTime = std::clamp(
                    audioList_[i].playTimer.currentTime,
                    0.0f,
                    audioList_[i].playTimer.duration
                );

                // 停止中なら再生終了させる
                if(audioList_[i].playTimer.isStop){
                    AudioManager::EndAudio(audioList_[i].handle);

                } else{
                    // 再生位置を合わせて再生
                    audioList_[i].handle = AudioManager::PlayAudio(
                        audioList_[i].filePath,
                        audioList_[i].isLoop,
                        audioList_[i].volume,
                        audioList_[i].playTimer.currentTime
                    );
                }
            }

            if(audioList_[i].filePath.empty()){
                ImGui::Text("音声ファイルを選択してください");
            } else{


                ImGui::Text(("音源: " + audioList_[i].filePath).c_str());
                ImFunc::InputText("名前##" + hash, audioList_[i].name);

                // 音量が変更されたら反映
                if(ImGui::DragFloat("音量##" + hash, &audioList_[i].volume, 0.01f, 0.0f, 3.0f)){
                    AudioManager::SetAudioVolume(audioList_[i].handle, audioList_[i].volume);
                }

                // ループフラグが変更されたら再生をやり直す
                if(ImGui::Checkbox("ループ再生##" + hash, &audioList_[i].isLoop)){
                    AudioManager::EndAudio(audioList_[i].handle);
                    audioList_[i].handle = AudioManager::PlayAudio(
                        audioList_[i].filePath,
                        audioList_[i].isLoop,
                        audioList_[i].volume,
                        audioList_[i].playTimer.currentTime
                    );
                }

                // 再生開始時間
                ImGui::DragFloat("開始時間##" + hash, &audioList_[i].startTime, 0.01f, 0.0f, audioList_[i].playTimer.duration);
                audioList_[i].startTime = std::clamp(audioList_[i].startTime, 0.0f, audioList_[i].playTimer.duration);

                // 再生遅延時間
                ImGui::DragFloat("再生遅延時間##" + hash, &audioList_[i].playOffsetTime, 0.01f, 0.0f, FLT_MAX);
            }

            ImGui::Unindent();
        }

        ImGui::Separator();
    }

    hash = MyFunc::PtrToStr(this);
    if(ImGui::Button("音声の追加##" + hash)){
        audioList_.emplace_back();
    }


    ImGui::Unindent();
#endif // _DEBUG
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
// json関連
/////////////////////////////////////////////////////////////////////////////////////////////////////
nlohmann::json AudioComponent::GetJsonData() const{
    nlohmann::json jsonData;

    // 基本情報
    jsonData["componentType"] = "Audio";
    jsonData.update(IComponent::GetJsonData());

    // 音声情報の書き出し
    for(const auto& audioItem : audioList_){
        nlohmann::json audioJson;
        audioJson["filePath"] = audioItem.filePath;
        audioJson["name"] = audioItem.name;
        audioJson["volume"] = audioItem.volume;
        audioJson["isLoop"] = audioItem.isLoop;
        audioJson["startTime"] = audioItem.startTime;
        audioJson["playOffsetTime"] = audioItem.playOffsetTime;
        audioJson["playDelayTime"] = audioItem.playDelayTimer.duration;
        jsonData["audioList"].push_back(audioJson);
    }

    return jsonData;
}

void AudioComponent::LoadFromJson(const nlohmann::json& jsonData){
    IComponent::LoadFromJson(jsonData);

    // 音声情報の読み込み
    audioList_.clear();
    for(const auto& audioJson : jsonData["audioList"]){
        AudioItem audioItem;
        audioItem.filePath = audioJson.value("filePath", "");
        audioItem.name = audioJson.value("name", "");
        audioItem.volume = audioJson.value("volume", 0.5f);
        audioItem.isLoop = audioJson.value("isLoop", false);
        audioItem.startTime = audioJson.value("startTime", 0.0f);
        audioItem.playOffsetTime = audioJson.value("playOffsetTime", 0.0f);
        audioItem.playDelayTimer.Initialize(audioJson.value("playDelayTime", 0.0f));
        audioList_.emplace_back(audioItem);
    }

}
