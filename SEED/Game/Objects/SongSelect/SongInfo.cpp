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

    // 楽曲情報jsonの存在を確認
    nlohmann::json generalData = SEED::Methods::File::GetJson(directoryPath + "songInfo.json", true);

    if(!generalData.empty()){
        // 共通情報を取得
        artistName = generalData.value("artist","");
        songName = generalData.value("songName","");
        bpm = generalData.value("bpm",0.0f);
        genre = (SongGenre)generalData.value("genre",0);
        songOffsetTime = generalData.value("offsetTime", 0.0f);
        songPreviewRange = generalData.value("previewRange", SEED::Range1D());
        songVolume = generalData.value("songVolume", 0.5f);
    
    } else{
        // 共通情報を初期化
        artistName = "";
        songName = "";
        bpm = 0.0f;
        genre = (SongGenre)0;
        songOffsetTime = 0.0f;
        songPreviewRange = SEED::Range1D();
        songVolume = 0.5f;
    }

    // audioファイルパスを設定
    audioFilePath = directoryPath + SEED::Methods::File::GetFileList(
        directoryPath,{ ".wav", ".mp3", ".m4a" }
    ).front().string();


    // 難易度ごとの情報を読み込む
    for(int i = 0; i < diffcultySize; i++){
        // JSONファイルを読み込む
        jsonFilePath[i] = directoryPath + difficultyName[i] + ".json";
        nlohmann::json noteData = SEED::Methods::File::GetJson(jsonFilePath[i], true);
        noteDatas[i] = noteData;

        // ファイルが存在しない場合はスキップ
        if(noteDatas[i].empty()){ continue; }

        // JSONからデータを取得
        difficulty[i] = noteDatas[i]["difficulty"];
        notesDesignerName[i] = noteDatas[i]["notesDesigner"];
    }


    // スコア情報を初期化
    directoryPath = "Resources/ScoreDatas/" + _folderName + "/scoreData.json";
    nlohmann::json scoreData = SEED::Methods::File::GetJson(directoryPath);

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