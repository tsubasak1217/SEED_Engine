#include "SongInfo.h"
#include <json.hpp>
#include <fstream>
#include <array>
#include <SEED/Source/SEED.h>

/////////////////////////////////////////////////////////////////////////////
// 曲名でSongInfoを初期化する関数
/////////////////////////////////////////////////////////////////////////////
void SongInfo::Initialize(const std::string& _folderName){

    folderName = _folderName;
    std::string jsonNames[diffcultySize] = { "Basic.json", "Expert.json", "Master.json", "Parallel.json" };
    std::string directoryPath = "Resources/NoteDatas/" + _folderName + "/";

    // jsonファイルの存在を確認し、読み込む
    for(int i = 0; i < diffcultySize; i++){
        std::string filePath = directoryPath + jsonNames[i];
        // あるか確認
        std::ifstream file(filePath);
        // ファイルが存在しない場合はスキップ
        if(!file.is_open()){
            file.close();
            continue;
        }

        // JSONファイルを読み込む
        nlohmann::json noteData = nlohmann::json::parse(std::ifstream(filePath));
        noteDatas[i] = noteData;

        // アーティスト名を取得
        if(artistName.empty()){
            artistName = noteData["artist"];
        }

        // BPMを取得
        if(bpm == 0.0f){
            bpm = noteData["bpm"];
        }

        // ジャンルを取得
        if(genre == std::nullopt){
            genre = (SongGenre)noteData["genre"];
        }

        // 楽曲名を取得
        if(songName.empty()){
            songName = noteData["songName"];
        }

        // 音声ファイルのパスを設定
        if(audioFilePath.empty()){
            audioFilePath = noteData["audioPath"];
        }
    }


    // 難易度ごとのデータを設定
    for(int i = 0; i < diffcultySize; i++){
        if(noteDatas[i].empty()){ continue; }

        // JSONからデータを取得
        difficulty[i] = noteDatas[i]["difficulty"];
        notesDesignerName[i] = noteDatas[i]["notesDesigner"];
    }

    // スコア情報を初期化
    directoryPath = "Resources/NoteDatas/" + _folderName + "/" + "scoreData.json";
    // jsonファイルの存在を確認し、読み込む
    for(int i = 0; i < diffcultySize; i++){
        std::string filePath = directoryPath + jsonNames[i];
        // あるか確認
        std::ifstream file(filePath);
        // ファイルが存在しない場合はスキップ
        if(!file.is_open()){
            file.close();
            continue;
        }

        // JSONファイルを読み込む
        nlohmann::json scoreData = nlohmann::json::parse(std::ifstream(filePath));
        score[i] = scoreData["score"];
        ranks[i] = ScoreRankUtils::GetScoreRank(score[i]);
        clearIcons[i] = (ClearIcon)scoreData["clearIcon"];
    }
}


//////////////////////////////////////////////////////////////////////////////
// 曲描画の初期化関数
//////////////////////////////////////////////////////////////////////////////
void SongInfoDrawer::Initialize(){


    // 背景画像の読み込み
    backSprite = std::make_unique<Sprite>("DefaultAssets/white1x1.png");
    backSprite->anchorPoint = { 0.5f,0.5f };
    backSprite->size = kDrawSize;

    jacketSprite = std::make_unique<Sprite>("DefaultAssets/white1x1.png");
    jacketSprite->size = { kDrawSize.y * 0.7f,kDrawSize.y * 0.7f };

    // テキストの初期化
    for(int i = 0; i < textBoxKeys.size(); i++){
        textBox[textBoxKeys[i]] = std::make_unique<TextBox2D>();
        textBox[textBoxKeys[i]]->textBoxVisible = false;
        textBox[textBoxKeys[i]]->SetFont("DefaultAssets/M_PLUS_Rounded_1c/MPLUSRounded1c-Bold.ttf");
    }

    // セッティングの読み込み
    LoadSettings();
}

/////////////////////////////////////////////////////////////////////////////
// 楽曲情報を描画する関数
/////////////////////////////////////////////////////////////////////////////
void SongInfoDrawer::Draw(const SongInfo& songInfo, const Transform2D& transform, TrackDifficulty difficulty, bool isSelected, float alpha){
    // 背景部分を描画
    if(isSelected){
        backSprite->color = backColors[(int)difficulty];
        backSprite->color.value.w = alpha;
    } else{
        backSprite->color = backColors[(int)difficulty] * 0.8f;
        backSprite->color.value.w = alpha;
    }

    backSprite->size = kDrawSize;
    backSprite->transform.translate = transform.translate;
    backSprite->transform.scale = transform.scale;
    backSprite->Draw();

    static Vector2 leftTopPos{};
    leftTopPos = transform.translate - (kDrawSize * transform.scale) * 0.5f;

    // ジャケットのオフセット
    Vector2 jacketOffset = { kDrawSize.y * 0.15f * transform.scale.x,kDrawSize.y * 0.15f * transform.scale.y };
    jacketSprite->size = { kDrawSize.y * 0.7f,kDrawSize.y * 0.7f };
    jacketSprite->GH = TextureManager::LoadTexture("../../Resources/NoteDatas/" + songInfo.folderName + "/" + songInfo.folderName + ".png");
    jacketSprite->transform.translate = leftTopPos + jacketOffset;
    jacketSprite->transform.scale = backSprite->transform.scale;
    // ジャケット画像を描画
    jacketSprite->color.value.w = alpha; // 透明度を適用
    jacketSprite->Draw();

    for(auto& key : textBoxKeys){
        if(key == "GroupName"){
            continue;
        }


        // テキストの描画
        int difficultyIndex = static_cast<int>(difficulty);
        if(key == "SongName"){
            textBox[key]->text = songInfo.songName;

        } else if(key == "ArtistName"){
            textBox[key]->text = songInfo.artistName;

        } else if(key == "BPM"){
            int bpm = static_cast<int>(songInfo.bpm);
            textBox[key]->text = "BPM" + std::to_string(bpm);

        } else if(key == "Difficulty"){
            textBox[key]->text = std::to_string(songInfo.difficulty[difficultyIndex]);

        } else if(key == "NotesDesigner"){
            textBox[key]->text = songInfo.notesDesignerName[difficultyIndex];

        } else if(key == "Score"){
            if(songInfo.score[difficultyIndex] == 0){
                continue; // スコアが0の場合は描画しない(未プレイ)
            }
            textBox[key]->text = std::to_string(songInfo.score[difficultyIndex]);
        }

        textBox[key]->transform.scale = transform.scale;
        textBox[key]->transform.translate = leftTopPos + textBoxRelativePos[key] * transform.scale;
        textBox[key]->color.value.w = alpha; // 透明度を適用
        textBox[key]->layer = backSprite->layer + 1;
        textBox[key]->Draw();
    }
}

/////////////////////////////////////////////////////////////////////////////
// 楽曲グループ情報を描画する関数
/////////////////////////////////////////////////////////////////////////////
void SongInfoDrawer::Draw(const SongGroup& groupInfo, const Transform2D& transform, TrackDifficulty difficulty, bool isSelected, float alpha){


    // 背景部分を描画
    if(isSelected){
        backSprite->color = backColors[(int)difficulty];
        backSprite->color.value.w = alpha; // 選択時は透明度を適用
    } else{
        backSprite->color = backColors[(int)difficulty] * 0.8f;
        backSprite->color.value.w = alpha; // 選択時は不透明
    }

    backSprite->size = kDrawSize;
    backSprite->transform.translate = transform.translate;
    backSprite->transform.scale = transform.scale;
    backSprite->Draw();

    static Vector2 leftTopPos{};
    leftTopPos = transform.translate - (kDrawSize * transform.scale) * 0.5f;

    // グループ名を描画
    textBox["GroupName"]->text = groupInfo.groupName;
    textBox["GroupName"]->transform.scale = transform.scale;
    textBox["GroupName"]->transform.translate = leftTopPos + textBoxRelativePos["GroupName"] * transform.scale;
    textBox["GroupName"]->color.value.w = alpha; // 透明度を適用
    textBox["GroupName"]->layer = backSprite->layer + 1;
    textBox["GroupName"]->Draw();

    difficulty;
}


/////////////////////////////////////////////////////////////////////////////
// 編集を行う関数
/////////////////////////////////////////////////////////////////////////////
void SongInfoDrawer::Edit(){
#ifdef _DEBUG

    ImGui::DragFloat2("DrawSize", &kDrawSize.x, 1.0f, 10.0f, 1000.0f);

    for(auto& key : textBoxKeys){
        if(ImGui::CollapsingHeader(key.c_str())){
            ImGui::Indent();
            ImGui::DragFloat2("相対座標", &textBoxRelativePos[key].x, 1.0f, -1000.0f, 1000.0f);
            textBox[key]->Edit();
            ImGui::Unindent();
        }
    }

    // 背景色の設定
    ImGui::Separator();
    for(int i = 0; i < (int)TrackDifficulty::kMaxDifficulty; i++){
        ImGui::ColorEdit4(("背景色 " + std::to_string(i)).c_str(), &backColors[i].x);
    }

    if(ImGui::Button("セッティングの保存")){
        SaveSettings();
    }

#endif // _DEBUG
}

///////////////////////////////////////////////////////////////////////
// 保存と読み込みの関数
///////////////////////////////////////////////////////////////////////
void SongInfoDrawer::SaveSettings(){
    // 設定を保存するためのJSONデータを作成
    nlohmann::json jsonData = ToJson();
    // ファイルに書き込む
    std::string filePath = "Resources/Jsons/Settings/SongInfoDrawerSettings.json";
    std::ofstream file(filePath);
    if(file.is_open()){
        file << jsonData.dump(4); // 4はインデントのスペース数
        file.close();
    }
}

void SongInfoDrawer::LoadSettings(){

    std::string filePath = "Resources/Jsons/Settings/SongInfoDrawerSettings.json";
    std::ifstream file(filePath);

    if(file.is_open()){
        nlohmann::json settingsJson;
        file >> settingsJson;
        file.close();

        // 設定を読み込む
        if(settingsJson.contains("kDrawSize")){
            kDrawSize = settingsJson["kDrawSize"];
        }

        for(auto key : textBoxKeys){
            if(settingsJson.contains(key)){
                if(settingsJson[key].contains("textBox")){
                    textBox[key]->LoadFromJson(settingsJson[key]["textBox"]);
                }
                if(settingsJson[key].contains("RelativePos")){
                    textBoxRelativePos[key] = settingsJson[key]["RelativePos"];
                }
            }
        }

        // 背景色の読み込み
        if(settingsJson.contains("backColors")){
            for(int i = 0; i < (int)TrackDifficulty::kMaxDifficulty; i++){
                backColors[i] = settingsJson["backColors"][i];
            }
        }
    }
}

nlohmann::json SongInfoDrawer::ToJson(){
    nlohmann::json jsonData;
    jsonData["kDrawSize"] = kDrawSize;

    for(auto key : textBoxKeys){
        jsonData[key]["textBox"] = textBox[key]->GetJsonData();
        jsonData[key]["RelativePos"] = textBoxRelativePos[key];
    }

    // 背景色の保存
    for(int i = 0; i < (int)TrackDifficulty::kMaxDifficulty; i++){
        jsonData["backColors"][i] = backColors[i];
    }

    return jsonData;
}
