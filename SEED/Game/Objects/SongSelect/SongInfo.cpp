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
    static std::string difficultyName[diffcultySize] = { "Basic", "Expert", "Master", "Parallel" };
    std::string directoryPath = "Resources/NoteDatas/" + _folderName + "/";

    // jsonファイルの存在を確認し、読み込む
    for(int i = 0; i < diffcultySize; i++){

        // JSONファイルを読み込む
        nlohmann::json noteData = MyFunc::GetJson(directoryPath + difficultyName[i] + ".json",true);
        noteDatas[i] = noteData;

        if(noteData.empty()){
            artistName = "";
            songName = "";
            bpm = 0.0f;
            genre = (SongGenre)0;
            audioFilePath = "";
            continue;
        }

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

    // jsonファイルの存在を確認し、読み込む
    directoryPath = "Resources/ScoreDatas/" + _folderName + "/scoreData.json";
    nlohmann::json scoreData = MyFunc::GetJson(directoryPath);

    // スコア情報を初期化
    if(!scoreData.empty()){
        for(int i = 0; i < diffcultySize; i++){
            // スコアの取得
            if(scoreData.contains("score")){
                nlohmann::json scoreJson = scoreData["score"];
                if(scoreJson[difficultyName[i]].is_number()){
                    score[i] = scoreJson[difficultyName[i]].get<float>();
                }
                ranks[i] = ScoreRankUtils::GetScoreRank(score[i]);
            }

            // クリアアイコンの取得
            if(scoreData.contains("clearIcon")){
                nlohmann::json clearIconData = scoreData["clearIcon"];
                clearIcons[i] = (ClearIcon)clearIconData.value(difficultyName[i], 0);
            }
        }
    }
}