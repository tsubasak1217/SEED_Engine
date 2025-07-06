#include "SongInfo.h"
#include <json.hpp>
#include <fstream>
#include <array>
#include <SEED/Source/SEED.h>

/////////////////////////////////////////////////////////////////////////////
// 曲名でSongInfoを初期化する関数
/////////////////////////////////////////////////////////////////////////////
void SongInfo::Initialize(const std::string& _songName){

    this->songName = _songName;
    std::string jsonNames[diffcultySize] = { "Basic.json", "Expert.json", "Master.json", "Parallel.json" };
    std::string directoryPath = "Resources/NoteDatas/" + _songName + "/";

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
    }


    // 難易度ごとのデータを設定
    for(int i = 0; i < diffcultySize; i++){
        if(noteDatas[i].empty()){ continue; }

        // JSONからデータを取得
        difficulty[i] = noteDatas[i]["difficulty"];
        notesDesignerName[i] = noteDatas[i]["notesDesigner"];
    }

    // スコア情報を初期化
    directoryPath = "Resources/Jsons/ScoreDatas/" + _songName + "/";
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
    kDrawSize = { 500.0f,120.0f };

    // 背景画像の読み込み
    backSprite = std::make_unique<Sprite>("DefaultAssets/white1x1.png");
    backSprite->anchorPoint = { 0.5f,0.5f };
    backSprite->size = kDrawSize;

    jacketSprite = std::make_unique<Sprite>("DefaultAssets/white1x1.png");
    jacketSprite->size = { kDrawSize.y * 0.7f,kDrawSize.y * 0.7f };
}

/////////////////////////////////////////////////////////////////////////////
// 楽曲情報を描画する関数
/////////////////////////////////////////////////////////////////////////////
void SongInfoDrawer::Draw(const SongInfo& songInfo, const Transform2D& transform, bool isSelected){
    // 背景部分を描画
    if(isSelected){
        backSprite->color = { 1.0f,1.0f,0.0f,1.0f };
    } else{
        backSprite->color = { 1.0f,1.0f,1.0f,1.0f };
    }

    backSprite->translate = transform.translate;
    backSprite->scale = transform.scale;
    //backSprite->Draw();
    songInfo;
    
    static Vector2 leftTopPos{};
    leftTopPos = transform.translate - (kDrawSize * transform.scale) * 0.5f;

    // ジャケットのオフセット
    Vector2 jacketOffset = { kDrawSize.y * 0.15f * transform.scale.x,kDrawSize.y * 0.15f * transform.scale.y };
    jacketSprite->GH = TextureManager::LoadTexture("Jackets/" + songInfo.songName + ".png");
    jacketSprite->translate = leftTopPos + jacketOffset;
    jacketSprite->scale = backSprite->scale;
    // ジャケット画像を描画
    //jacketSprite->Draw();
}

/////////////////////////////////////////////////////////////////////////////
// 編集を行う関数
/////////////////////////////////////////////////////////////////////////////
void SongInfoDrawer::Edit(){
#ifdef _DEBUG

    ImGui::DragFloat2("DrawSize", &kDrawSize.x, 1.0f, 100.0f, 1000.0f);

#endif // _DEBUG
}
