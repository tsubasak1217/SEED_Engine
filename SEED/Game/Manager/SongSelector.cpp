#include "SongSelector.h"
#include <SEED/Lib/Functions/MyFunc.h>
#include <SEED/Source/SEED.h>
#include <SEED/Source/Manager/ImGuiManager/ImGuiManager.h>
#include <SEED/Lib/Structs/Transform.h>
#include <Game/GameSystem.h>
#include <Game/Scene/Scene_Game/State/GameState_Play.h>
#include <Game/Scene/Scene_Clear/Scene_Clear.h>
#include <Game/Objects/SongSelect/SelectBackGroundDrawer.h>

// コンストラクタ
SongSelector::SongSelector(){
}

// デストラクタ
SongSelector::~SongSelector(){

    // 終了時にjsonに情報を保存
    std::string savePath = "Resources/Jsons/Settings/song_selector.json";

    // jsonファイルに保存
    std::ofstream ofs(savePath);
    if(ofs.is_open()){
        nlohmann::json json = ToJson();
        ofs << json.dump(4);
        ofs.close();
    }
}

void SongSelector::Initialize(){

    // NoteDatasの階層にあるフォルダ名を一覧取得
    auto songFolders = MyFunc::GetFolderList("Resources/NoteDatas");

    // フォルダ名を元にSongInfoを生成
    for(const auto& folderName : songFolders){
        // フォルダ名を取得
        auto& data = songList.emplace_back(new SongInfo());
        data->Initialize(folderName);
    }

    // 描画クラスの初期化
    SongInfoDrawer::Initialize();

    // jsonから設定を読み込む
    std::string loadPath = "Resources/Jsons/Settings/song_selector.json";
    std::ifstream ifs(loadPath);
    if(ifs.is_open()){
        nlohmann::json json;
        ifs >> json;
        ifs.close();
        FromJson(json);
    } else{
        // ファイルが存在しない場合はデフォルト値を設定
        currentGroupIndex = 0;
        currentSongIndex = 0;
        currentGroupMode = GroupMode::None;
        currentSortMode = SortMode::Difficulty;
        currentDifficulty = TrackDifficulty::Basic;
    }

    // ソート処理を実行
    Sort();

    // 現在のグループを設定
    auto it = songGroups.begin();
    std::advance(it, currentGroupIndex);
    currentGroup = &(*it);

    // 現在の曲を設定
    if(currentGroup && !currentGroup->groupMembers.empty()){
        auto songIt = currentGroup->groupMembers.begin();
        std::advance(songIt, currentSongIndex);
        currentSong = *songIt;
    } else{
        currentSong.first = nullptr; // グループが空の場合はnullに設定
    }

    // 可視曲の初期化
    UpdateVisibleItems();

    // ジャケット矩形の初期化
    jacket3D_ = MakeEqualQuad(3.0f);
    jacket3D_.cullMode = D3D12_CULL_MODE_NONE; // カリングなし
    jacket3D_.lightingType = LIGHTINGTYPE_NONE; // ライティングなし
    jacket3D_.GH = TextureManager::LoadTexture(
        "../../Resources/NoteDatas/" + currentSong.first->folderName + "/" + currentSong.first->folderName + ".png"
    );

    // カメラの取得
    SEED::RemoveCamera("gameCamera");
    SEED::SetMainCamera("default");
    camera_ = SEED::GetMainCamera();

    // パーティクルを出す
    EffectSystem::DeleteAll();// 既存のエフェクトを削除
    EffectSystem::AddEffectEndless("selectScene.json", { 0.0f,0.0f,0.0f }, nullptr);
}


/////////////////////////////////////////////////////////////////////
// フレーム終了時処理
/////////////////////////////////////////////////////////////////////
void SongSelector::EndFrame(){
    // 曲の選択が完了したら、曲の詳細を非表示にする
    if(changeSceneOrder_){
        Scene_Clear::SetJacketPath(
            "Resources/NoteDatas/" + currentSong.first->folderName + "/" + currentSong.first->folderName + ".png"
        );

        auto* scene = GameSystem::GetScene();
        GameSystem::GetScene()->ChangeState(new GameState_Play(scene, currentSong.first->noteDatas[(int)currentDifficulty]));
        return;
    }
}

////////////////////////////////////////////////////////////////////
// 更新処理
////////////////////////////////////////////////////////////////////
void SongSelector::Update(){

    if(!isStartPlay){
        // UIの更新
        UpdateSongUI();
        // 項目の選択
        SelectItems();

        // 曲の詳細表示
        if(slideOutTimer > 0.0f){
            // 曲の詳細画面を表示
            DisplaySongDetail();
        }

        // 制御点の位置を編集
        Edit();

    } else{
        StartPlay(); // ゲームのプレイを開始
    }

    // カメラの制御
    CameraControl();

    // ジャケットの更新
    UpdateJacket();
}


////////////////////////////////////////////////////////////////////
// 描画処理
////////////////////////////////////////////////////////////////////
void SongSelector::Draw(){

    // 曲のリストを描画
    if(selectMode_ == SelectMode::Song){
        if(slideOutTimer < kMaxSlideOutTime){
            for(int i = 0; i < visibleSongs.size(); i++){
                if(visibleSongs[i]){
                    SongInfoDrawer::Draw(*visibleSongs[i]->first, visibleItemTransforms[i], visibleSongs[i]->second, i == centerIndex);
                }
            }
        }

        if(slideOutTimer > 0.0f){

            if(!isStartPlay){
                // 難易度のリストを描画
                for(int i = 0; i < songAllDifficulty.size(); i++){
                    if(songAllDifficulty[i].first){
                        SongInfoDrawer::Draw(
                            *songAllDifficulty[i].first,
                            songAllDifficultyTransforms[i],
                            songAllDifficulty[i].second,
                            aimIndices[i] == difficultyTransformCenterIdx
                        );
                    }
                }
            } else{
                // 難易度のリストを描画
                float t = std::clamp(playWaitTimer / (kPlayWaitTime * 0.5f), 0.0f, 1.0f);
                for(int i = 0; i < songAllDifficulty.size(); i++){
                    if(songAllDifficulty[i].first){
                        if(aimIndices[i] == difficultyTransformCenterIdx){
                            // 選択局はそのまま描画
                            SongInfoDrawer::Draw(
                                *songAllDifficulty[i].first,
                                songAllDifficultyTransforms[i],
                                songAllDifficulty[i].second,
                                aimIndices[i] == difficultyTransformCenterIdx
                            );
                        } else{
                            // それ以外は透明にして消していく
                            SongInfoDrawer::Draw(
                                *songAllDifficulty[i].first,
                                songAllDifficultyTransforms[i],
                                songAllDifficulty[i].second,
                                aimIndices[i] == difficultyTransformCenterIdx,
                                1.0f - t
                            );
                        }
                    }
                }
            }
        }

    } else{
        for(int i = 0; i < visibleGroups.size(); i++){
            if(visibleGroups[i]){
                SongInfoDrawer::Draw(*visibleGroups[i], visibleItemTransforms[i], currentDifficulty, i == centerIndex);
            }
        }
    }


    // ジャケットの描画
    SEED::DrawQuad(jacket3D_);
}

///////////////////////////////////////////////////////////////
// ソート処理
///////////////////////////////////////////////////////////////
void SongSelector::Sort(){
    // まずグループ分け
    CreateGroup();
    // グループ内でのソート
    SortInGroup();
    // 各曲のインデックスを更新
    UpdateIndex();
    // 可視曲の更新
    UpdateVisibleItems();
}

///////////////////////////////////////////////////////////////////
// グループ分け
///////////////////////////////////////////////////////////////////
void SongSelector::CreateGroup(){

    songGroups.clear();

    // まずGroupModeに応じたgroup分けを行う
    switch(currentGroupMode){
    case GroupMode::None:
    {
        // グループ分けしない場合は、全ての曲を一つのグループにまとめる(曲単位)
        songGroups.emplace_back(SongGroup());
        songGroups.back().groupName = "全楽曲"; // グループ名を設定
        for(auto& song : songList){
            songGroups.back().groupMembers.push_back({ song.get(),currentDifficulty });
        }

        break;
    }
    case GroupMode::Genre:
    {
        // ジャンルごとにグループ分け(曲単位)
        for(auto& song : songList){
            auto genre = song->genre;
            // ジャンルが空の場合はスキップ
            if(genre == std::nullopt){ continue; }

            // 既存のグループを探す
            auto it = std::find_if(songGroups.begin(), songGroups.end(),
                [&genre](const SongGroup& group){
                return !group.groupMembers.empty() && group.groupMembers.front().first->genre == genre;
            });

            // 見つからなければ新しいグループを作成
            if(it == songGroups.end()){
                songGroups.emplace_back(SongGroup());
                it = std::prev(songGroups.end());
                songGroups.back().groupName = GroupNameUtils::genreNames[int(genre.value())]; // グループ名を設定
            }

            // グループに曲を追加
            it->groupMembers.push_back({ song.get(),currentDifficulty });
        }

        // ジャンルごとにソート
        songGroups.sort(
            [](const SongGroup& a, const SongGroup& b){
            return a.groupMembers.front().first->genre < b.groupMembers.front().first->genre;
        });

        break;
    }
    case GroupMode::Difficulty:
    {
        // 難易度ごとにグループ分け(譜面単位)
        for(auto& song : songList){

            // 既存のグループを探す
            for(int i = 0; i < (int)TrackDifficulty::kMaxDifficulty; i++){

                auto difficulty = song->difficulty[i];
                if(song->noteDatas[i].empty()){ continue; } // 譜面が存在しない場合はスキップ

                auto it = std::find_if(songGroups.begin(), songGroups.end(),
                    [&](const SongGroup& group){
                    return !group.groupMembers.empty() &&
                        group.groupName == "Lv" + std::to_string(difficulty);
                });

                // 見つからなければ新しいグループを作成
                if(it == songGroups.end()){
                    songGroups.emplace_back(SongGroup());
                    it = std::prev(songGroups.end());
                    songGroups.back().groupName = "Lv" + std::to_string(difficulty); // グループ名を設定
                }

                // グループに曲を追加
                it->groupMembers.push_back({ song.get(),TrackDifficulty(i) });
            }
        }

        // 難易度ごとにソート
        songGroups.sort(
            [&](const SongGroup& a, const SongGroup& b){
            return a.groupName < b.groupName;
        });

        break;
    }
    case GroupMode::Rank:
    {
        // ランクごとにグループ分け(譜面単位)
        for(auto& song : songList){

            // 既存のグループを探す
            for(int i = 0; i < (int)TrackDifficulty::kMaxDifficulty; i++){
                ScoreRank rank = song->ranks[i];
                if(song->noteDatas[i].empty()){ continue; } // 譜面が存在しない場合はスキップ

                // 既存のグループを探す
                auto it = std::find_if(songGroups.begin(), songGroups.end(),
                    [&](const SongGroup& group){
                    return !group.groupMembers.empty() &&
                        group.groupName == GroupNameUtils::rankNames[(int)rank];
                });

                // 見つからなければ新しいグループを作成
                if(it == songGroups.end()){
                    songGroups.emplace_back(SongGroup());
                    it = std::prev(songGroups.end());
                    songGroups.back().groupName = GroupNameUtils::rankNames[(int)rank]; // グループ名を設定
                }

                // グループに曲を追加
                it->groupMembers.push_back({ song.get(),TrackDifficulty(i) });
            }
        }

        // ランクごとにソート
        songGroups.sort(
            [&](const SongGroup& a, const SongGroup& b){
            return a.groupMembers.front().first->ranks[(int)currentDifficulty] <
                b.groupMembers.front().first->ranks[(int)currentDifficulty];
        });

        break;
    }
    default:
        break;
    }
}


/////////////////////////////////////////////////////////////////////////////////////
// グループ内でのソート処理
/////////////////////////////////////////////////////////////////////////////////////
void SongSelector::SortInGroup(){

    for(auto& group : songGroups){

        switch(currentSortMode){

        case SortMode::Title:
            // タイトルでソート
            group.groupMembers.sort(
                [](const SongAndDiffidulty& a, const SongAndDiffidulty& b){
                return MyFunc::CompareStr(a.first->songName, b.first->songName);
            });
            break;

        case SortMode::Artist:
            // アーティストでソート
            group.groupMembers.sort(
                [](const SongAndDiffidulty& a, const SongAndDiffidulty& b){
                return MyFunc::CompareStr(a.first->artistName, b.first->artistName);
            });
            break;

        case SortMode::Difficulty:
            // 難易度でソート
            group.groupMembers.sort(
                [&](const SongAndDiffidulty& a, const SongAndDiffidulty& b){
                return a.first->difficulty[(int)a.second] < b.first->difficulty[(int)b.second];
            });
            break;

        case SortMode::Score:

            // スコアでソート
            group.groupMembers.sort(
                [&](const SongAndDiffidulty& a, const SongAndDiffidulty& b){
                return a.first->score[(int)a.second] < b.first->score[(int)b.second];
            });
            break;

        case SortMode::ClearIcon:

            // クリアアイコンでソート
            group.groupMembers.sort(
                [&](const SongAndDiffidulty& a, const SongAndDiffidulty& b){
                return a.first->clearIcons[(int)a.second] < b.first->clearIcons[(int)b.second];
            });
            break;

        default:
            // それ以外のソートモードは未対応
            break;
        }
    }
}


///////////////////////////////////////////////////////////////////////////////////////
// インデックスの更新処理
///////////////////////////////////////////////////////////////////////////////////////
void SongSelector::UpdateIndex(){

    int32_t nextGroupIndex = 0;

    for(auto& group : songGroups){

        // グループのインデックスを更新
        group.groupIdx = nextGroupIndex; // グループのインデックスを設定
        nextGroupIndex++;

        // 各グループ内の曲のインデックスを更新
        int32_t nextSongIndex = 0;
        for(auto& song : group.groupMembers){
            // 曲のインデックスを更新
            song.first->sortIndex[(int)song.second] = nextSongIndex++;
            song.first->groupIndex[(int)song.second] = group.groupIdx;
        }
    }

    // 現在のインデックスを更新
    if(currentSong.first){
        currentGroupIndex = currentSong.first->groupIndex[(int)currentSong.second];
        currentSongIndex = currentSong.first->sortIndex[(int)currentSong.second];

        auto it = songGroups.begin();
        std::advance(it, currentGroupIndex);
        currentGroup = &(*it); // 現在のグループを更新
    }
}

/////////////////////////////////////////////////////////////////////////////////////
// 可視曲の更新処理
/////////////////////////////////////////////////////////////////////////////////////
void SongSelector::UpdateVisibleItems(){

    if(!currentGroup){ return; }
    if(currentGroup->groupMembers.empty()){ return; }

    {

        auto itGroup = songGroups.begin();
        std::advance(itGroup, currentGroupIndex);
        currentGroup = &(*itGroup); // 現在のグループを更新

        // 可視オブジェクトの更新
        auto it = currentGroup->groupMembers.begin();
        std::advance(it, currentSongIndex);
        visibleSongs[centerIndex] = &(*it); // 現在の曲を可視曲リストに設定

        for(int i = 0; i <= kVisibleRadius; i++){

            int32_t index[2] = {
                MyFunc::Spiral(currentSongIndex + i, 0, int(currentGroup->groupMembers.size()) - 1),
                MyFunc::Spiral(currentSongIndex - i, 0, int(currentGroup->groupMembers.size()) - 1)
            };

            auto it2 = currentGroup->groupMembers.begin();
            std::advance(it2, index[0]);
            visibleSongs[centerIndex + i] = &(*it2); // 前方の曲を可視曲リストに設定

            auto it3 = currentGroup->groupMembers.begin();
            std::advance(it3, index[1]);
            visibleSongs[centerIndex - i] = &(*it3); // 後方の曲を可視曲リストに設定
        }
    }
    {
        // 可視オブジェクトの更新
        auto it = songGroups.begin();
        std::advance(it, currentGroupIndex);
        visibleGroups[centerIndex] = &(*it); // 現在の曲を可視曲リストに設定

        for(int i = 0; i <= kVisibleRadius; i++){

            int32_t index[2] = {
                MyFunc::Spiral(currentGroupIndex + i, 0, int(songGroups.size()) - 1),
                MyFunc::Spiral(currentGroupIndex - i, 0, int(songGroups.size()) - 1)
            };

            auto it2 = songGroups.begin();
            std::advance(it2, index[0]);
            visibleGroups[centerIndex + i] = &(*it2); // 前方の曲を可視曲リストに設定

            auto it3 = songGroups.begin();
            std::advance(it3, index[1]);
            visibleGroups[centerIndex - i] = &(*it3); // 後方の曲を可視曲リストに設定
        }
    }
}



//////////////////////////////////////////////////////////////////////////////////
// 項目の選択処理
//////////////////////////////////////////////////////////////////////////////////
void SongSelector::SelectItems(){
    static float kIntervalTimeFirst = 0.5f; // 最初の入力のみのインターバル時間
    static float intervalTimer = 0.0f;
    static bool isFirstInput = true;
    bool changed = false;

    // ---------------------- 選択物の変更 ---------------------------
    if(slideOutTimer == 0){
        if(Input::IsTriggerKey({ DIK_W,DIK_S,DIK_UP,DIK_DOWN })){
            changed = true;
            isItemChanged = true;
            itemChangeTimer = 0.0f; // タイマーをリセット
            intervalTimer = kIntervalTimeFirst; // インターバルタイマーをリセット

        } else if(Input::IsPressKey({ DIK_W,DIK_S,DIK_UP,DIK_DOWN })){
            if(intervalTimer <= 0.0f){
                changed = true;
                isItemChanged = true;
                itemChangeTimer = 0.0f; // タイマーをリセット

                // インターバルタイマーをリセット(最初だけ長めにとる)
                if(isFirstInput){
                    intervalTimer = kIntervalTimeFirst;
                    isFirstInput = false; // 最初の入力フラグを解除
                } else{
                    intervalTimer = kItemChangeTime;
                }
            } else{
                intervalTimer -= ClockManager::DeltaTime();
            }
        } else{
            isFirstInput = true;
        }
    }

    //----------------------- 選択モードの変更--------------------------
    if(selectMode_ == SelectMode::Song){

        if(slideOutTimer == 0){
            if(Input::IsTriggerKey(DIK_Q)){
                // ソートモードを変更
                currentSortMode = SortMode(MyFunc::Spiral(int(currentSortMode) - 1, 0, int(SortMode::kMaxCount) - 1));
                Sort();

            } else if(Input::IsTriggerKey(DIK_E)){
                // ソートモードを変更
                currentSortMode = SortMode(MyFunc::Spiral(int(currentSortMode) + 1, 0, int(SortMode::kMaxCount) - 1));
                Sort();

            } else if(Input::IsTriggerKey({ DIK_A,DIK_LEFT })){
                // 難易度を変更
                TrackDifficulty nextDifficulty = TrackDifficulty(std::clamp(int(currentDifficulty) - 1, 0, int(TrackDifficulty::kMaxDifficulty) - 1));
                // 譜面があれば
                if(!currentSong.first->noteDatas[(int)nextDifficulty].empty()){
                    currentDifficulty = nextDifficulty; // 難易度を更新
                    currentSong.second = nextDifficulty; // 現在の曲の難易度も更新
                    SelectBackGroundDrawer::currentDifficulty = nextDifficulty; // 背景の難易度を更新
                    Sort();
                }

            } else if(Input::IsTriggerKey({ DIK_D,DIK_RIGHT })){
                // 難易度を変更
                TrackDifficulty nextDifficulty = TrackDifficulty(std::clamp(int(currentDifficulty) + 1, 0, int(TrackDifficulty::kMaxDifficulty) - 1));
                // 譜面があれば
                if(!currentSong.first->noteDatas[(int)nextDifficulty].empty()){
                    currentDifficulty = nextDifficulty; // 難易度を更新
                    currentSong.second = nextDifficulty; // 現在の曲の難易度も更新
                    SelectBackGroundDrawer::currentDifficulty = nextDifficulty; // 背景の難易度を更新
                    Sort();
                }

            } else if(Input::IsTriggerKey({ DIK_SPACE })){
                // 楽曲を決定して詳細画面へ
                isDecideSong = true;
                slideOutTimer = 0.0001f;
                CreateDifficultyList(); // 難易度のリストを作成

            } else if(Input::IsTriggerKey({ DIK_ESCAPE })){
                // グループ選択に戻る
                selectMode_ = SelectMode::Group;
            }
        } else{
            // プレイ開始
            if(Input::IsTriggerKey({ DIK_SPACE })){
                // 楽曲を決定して詳細画面へ
                isStartPlay = true;
                playWaitTimer = 0.0f; // プレイ待機タイマーをリセット

            } else if(Input::IsTriggerKey({ DIK_ESCAPE })){// 楽曲選択に戻る
                isDecideSong = false;
                Sort();
            }
        }

    } else{

        if(Input::IsTriggerKey(DIK_Q)){
            currentGroupMode = GroupMode(MyFunc::Spiral(int(currentGroupMode) - 1, 0, int(GroupMode::kMaxCount) - 1));
            Sort();

        } else if(Input::IsTriggerKey(DIK_E)){
            currentGroupMode = GroupMode(MyFunc::Spiral(int(currentGroupMode) + 1, 0, int(GroupMode::kMaxCount) - 1));
            Sort();

        } else if(Input::IsTriggerKey({ DIK_SPACE })){
            selectMode_ = SelectMode::Song;
        }
    }


    //---------------- 入力に応じて選択曲・グループを変更 -----------------
    if(changed){

        // 入力方向の更新
        if(Input::IsPressKey({ DIK_W,DIK_UP })){
            changeDirection = UpDown::UP;
            for(int i = 0; i < visibleSongs.size(); i++){
                if(visibleSongs[i]){
                    visibleItemTransforms[i] = itemAimTransforms[i];
                }
            }

        } else{
            changeDirection = UpDown::DOWN;
            for(int i = 0; i < visibleSongs.size(); i++){
                if(visibleSongs[i]){
                    visibleItemTransforms[i] = itemAimTransforms[2 + i];
                }
            }
        }


        if(selectMode_ == SelectMode::Song){
            if(currentGroup){
                currentSongIndex = MyFunc::Spiral(
                    currentSongIndex + (changeDirection == UpDown::DOWN ? 1 : -1),
                    0, int(currentGroup->groupMembers.size()) - 1
                );

                // 現在の曲を更新
                auto it = currentGroup->groupMembers.begin();
                std::advance(it, currentSongIndex);
                currentSong = *it;
                // 再生する曲のハンドルを更新
                AudioManager::EndAudio(songHandle_); // 曲を停止
                songHandle_ = AudioManager::PlayAudio(currentSong.first->audioFilePath, true);

                // ジャケットの更新
                jacket3D_.GH = TextureManager::LoadTexture(
                    "../../Resources/NoteDatas/" + currentSong.first->folderName + "/" + currentSong.first->folderName + ".png"
                );
            }

        } else{
            if(songGroups.size()){
                currentGroupIndex = MyFunc::Spiral(
                    currentGroupIndex + (changeDirection == UpDown::DOWN ? 1 : -1),
                    0, int(songGroups.size()) - 1
                );

                // 現在のグループを更新
                auto it = songGroups.begin();
                std::advance(it, currentGroupIndex);
                currentGroup = &(*it);
                currentSongIndex = 0; // グループ変更時は曲のインデックスをリセット
            }
        }


        // 可視曲を更新
        UpdateVisibleItems();
        // 難易度の更新
        if(currentSong.first){
            currentDifficulty = currentSong.second; // 現在の曲の難易度を更新
            SelectBackGroundDrawer::currentDifficulty = currentDifficulty; // 背景の難易度を更新
        }
    }
}


/////////////////////////////////////////////////////////////////////////////////////
// 曲のUIを更新
/////////////////////////////////////////////////////////////////////////////////////
void SongSelector::UpdateSongUI(){

    if(isItemChanged){

        // アイテムの座標を設定
        float t = std::clamp(itemChangeTimer / kItemChangeTime, 0.0f, 1.0f);
        float ease = EaseOutExpo(t);
        for(int i = 0; i < visibleSongs.size(); i++){
            if(visibleSongs[i]){
                if(changeDirection == UpDown::UP){// 上を押したので下に移動
                    visibleItemTransforms[i] =
                        MyFunc::Interpolate(itemAimTransforms[i], itemAimTransforms[1 + i], ease);

                } else{// 下を押したので上に移動
                    visibleItemTransforms[i] =
                        MyFunc::Interpolate(itemAimTransforms[i + 2], itemAimTransforms[1 + i], ease);
                }
            }
        }


        // 一定時間経過したらアイテム変更
        if(itemChangeTimer >= kItemChangeTime){

            // アイテム変更フラグをリセット
            isItemChanged = false;

            // アイテムの座標を設定(動いていないときの座標に設定)
            for(int i = 0; i < visibleSongs.size(); i++){
                if(visibleSongs[i]){
                    visibleItemTransforms[i] = itemAimTransforms[1 + i];
                }
            }

        } else{
            // アイテム変更タイマーを更新
            itemChangeTimer = std::clamp(itemChangeTimer + ClockManager::DeltaTime(), 0.0f, kItemChangeTime);
        }

    } else{

        if(!isDecideSong){
            // アイテムの座標を設定(動いていないときの座標に設定)
            for(int i = 0; i < visibleSongs.size(); i++){
                if(visibleSongs[i]){
                    visibleItemTransforms[i] = itemAimTransforms[1 + i];
                }
            }
        }
    }
}

/////////////////////////////////////////////////////////////////////////////////////
// スライドアウト処理
/////////////////////////////////////////////////////////////////////////////////////
void SongSelector::DisplaySongDetail(){

    float prevT = slideOutTimer / kMaxSlideOutTime;

    // 時間の更新
    if(isDecideSong){
        slideOutTimer = std::clamp(slideOutTimer + ClockManager::DeltaTime(), 0.0f, kMaxSlideOutTime);

    } else{
        slideOutTimer = std::clamp(slideOutTimer - ClockManager::DeltaTime(), 0.0f, kMaxSlideOutTime);
    }

    // アイテムの位置をスライドアウト
    float t = std::clamp(slideOutTimer / kMaxSlideOutTime, 0.0f, 1.0f);
    float ease = EaseInOutExpo(t);

    if(t < 1.0f){
        for(int i = 0; i < visibleSongs.size(); i++){
            if(visibleSongs[i]){

                if(i == centerIndex){
                    // 中心の曲はそのまま
                    visibleItemTransforms[i] = MyFunc::Interpolate(
                        itemAimTransforms[1 + i], difficultyAimTransforms[difficultyTransformCenterIdx], ease
                    );

                } else{
                    // スライドアウトの位置を設定
                    visibleItemTransforms[i].translate.x = itemAimTransforms[1 + i].translate.x + (slideOutWidth * ease);
                }
            }
        }

        for(int i = 0; i < songAllDifficultyTransforms.size(); i++){
            if(aimIndices[i] != difficultyTransformCenterIdx){
                // 中心の難易度はそのまま
                songAllDifficultyTransforms[i].scale = difficultyAimTransforms[aimIndices[i]].scale * ease;
            } else{
                //見えないようにスケールを0にする
                songAllDifficultyTransforms[i].scale = { 0.0f,0.0f };
            }
        }

    } else{// スライドアウトが完了したら

        if(prevT < 1.0f){
            for(int i = 0; i < aimIndices.size(); i++){
                if(aimIndices[i] == difficultyTransformCenterIdx){
                    songAllDifficultyTransforms[i].scale =
                        difficultyAimTransforms[aimIndices[i]].scale; // 中心の難易度のスケールを元に戻す
                }
            }
        }

        UpdateDifficultyList();
    }
}

/////////////////////////////////////////////////////////////////////////////////////
// 難易度のリストを作成
/////////////////////////////////////////////////////////////////////////////////////
void SongSelector::CreateDifficultyList(){

    songAllDifficulty.clear();
    songAllDifficultyTransforms.clear();
    aimIndices.clear();

    if(currentSong.first){

        int center = -1;

        // 現在の曲の難易度を取得
        for(int i = 0; i < (int)TrackDifficulty::kMaxDifficulty; i++){
            if(!currentSong.first->noteDatas[i].empty()){
                songAllDifficulty.push_back({ currentSong.first,TrackDifficulty(i) });

                if(i == (int)currentDifficulty){
                    // 現在の難易度のインデックスを保存
                    aimIndices.push_back(difficultyTransformCenterIdx);
                    center = i;
                } else{
                    // 他の難易度は仮に-1を設定
                    aimIndices.push_back(-1);
                }
            }
        }

        // 難易度の位置を設定
        for(int i = 0; i < aimIndices.size(); i++){
            // centerからの相対位置を計算して設定
            int index = difficultyTransformCenterIdx - (i - center);
            aimIndices[i] = index;
            songAllDifficultyTransforms.push_back(difficultyAimTransforms[index]);
        }
    }
}

///////////////////////////////////////////////////////////////////////////////////////
// 難易度のリストを更新
///////////////////////////////////////////////////////////////////////////////////////
void SongSelector::UpdateDifficultyList(){
    static float kIntervalTimeFirst = 0.5f; // 最初の入力のみのインターバル時間
    static float intervalTimer = 0.0f;
    static bool isFirstInput = true;
    bool changed = false;

    if(Input::IsTriggerKey({ DIK_W,DIK_S,DIK_UP,DIK_DOWN })){
        changed = true;
        difficultySelectTimer = 0.0f; // タイマーをリセット
        intervalTimer = kIntervalTimeFirst; // インターバルタイマーをリセット

    } else if(Input::IsPressKey({ DIK_W,DIK_S,DIK_UP,DIK_DOWN })){
        if(intervalTimer <= 0.0f){
            changed = true;
            difficultySelectTimer = 0.0f; // タイマーをリセット

            // インターバルタイマーをリセット(最初だけ長めにとる)
            if(isFirstInput){
                intervalTimer = kIntervalTimeFirst;
                isFirstInput = false; // 最初の入力フラグを解除
            } else{
                intervalTimer = kItemChangeTime;
            }

        } else{
            intervalTimer -= ClockManager::DeltaTime();
        }
    } else{
        isFirstInput = true;
    }

    // ----------------------- 難易度の変更 --------------------------
    if(changed){
        // 入力方向の更新
        if(Input::IsPressKey({ DIK_W,DIK_UP })){
            changeDirection = UpDown::UP;

            if(aimIndices.front() != difficultyTransformCenterIdx + (int)songAllDifficultyTransforms.size() - 1){
                for(int i = 0; i < aimIndices.size(); i++){
                    aimIndices[i] = std::clamp(aimIndices[i] + 1, 0, (int)difficultyAimTransforms.size() - 1);
                }
            }

        } else{
            changeDirection = UpDown::DOWN;
            if(aimIndices.back() != difficultyTransformCenterIdx - ((int)songAllDifficultyTransforms.size() - 1)){
                for(int i = 0; i < aimIndices.size(); i++){
                    aimIndices[i] = std::clamp(aimIndices[i] - 1, 0, (int)difficultyAimTransforms.size() - 1);
                }
            }
        }

        for(int i = 0; i < aimIndices.size(); i++){
            if(aimIndices[i] == difficultyTransformCenterIdx){
                currentSong = songAllDifficulty[i]; // 現在の曲を更新
                currentDifficulty = currentSong.second; // 現在の難易度を更新
                SelectBackGroundDrawer::currentDifficulty = currentDifficulty; // 背景の難易度を更新
            }
        }
    }

    // 難易度の座標を設定
    float t = std::clamp(difficultySelectTimer / kItemChangeTime, 0.0f, 1.0f);
    float ease = EaseOutExpo(t);
    for(int i = 0; i < songAllDifficultyTransforms.size(); i++){
        // 中心の難易度はそのまま
        songAllDifficultyTransforms[i] = MyFunc::Interpolate(
            songAllDifficultyTransforms[i], difficultyAimTransforms[aimIndices[i]], ease
        );
    }

    difficultySelectTimer = std::clamp(difficultySelectTimer + ClockManager::DeltaTime(), 0.0f, kItemChangeTime);
}


/////////////////////////////////////////////////////////////////////////////////////
// ゲームのプレイを開始
/////////////////////////////////////////////////////////////////////////////////////
void SongSelector::StartPlay(){

    static Transform2D centerTransform = Transform2D({ 1.5f,1.5f, }, 0.0f, kWindowCenter);

    // プレイ待機時間を更新
    playWaitTimer = std::clamp(playWaitTimer + ClockManager::DeltaTime(), 0.0f, kPlayWaitTime);
    float t = std::clamp(playWaitTimer / kPlayWaitTime, 0.0f, 1.0f);
    float ease = EaseInOutExpo(t);

    // 選択アイテムの位置を中央へ
    for(int i = 0; i < aimIndices.size(); i++){
        if(aimIndices[i] == difficultyTransformCenterIdx){
            // 中心の難易度はそのまま
            songAllDifficultyTransforms[i] = MyFunc::Interpolate(
                songAllDifficultyTransforms[i], centerTransform, ease
            );
        }
    }

    // シーンのステートを遷移
    if(t == 1.0f){
        changeSceneOrder_ = true;
    }
}

/////////////////////////////////////////////////////////////////////////////////////
// カメラの制御
/////////////////////////////////////////////////////////////////////////////////////
void SongSelector::CameraControl(){

    //
    static int controlIndex = 0;
    static int preControlIndex = 0;
    static float kCameraControlTime = 0.8f; // カメラ制御の時間
    static Timer cameraTimer = Timer(kCameraControlTime);

    if(!isStartPlay){
        if(selectMode_ == SelectMode::Song){

            // 曲選択画面---------------------------------------------
            if(!isDecideSong){

                controlIndex = 1;

                if(controlIndex != preControlIndex){// 入った瞬間
                    preCameraControlTransform_ = camera_->GetTransform();
                    cameraTimer = Timer(kCameraControlTime);
                }

                // カメラ補間
                camera_->SetTransform(
                    MyFunc::Interpolate(
                        preCameraControlTransform_,
                        cameraControlTransforms_[controlIndex],
                        cameraTimer.GetEase(Easing::InOutExpo)
                    )
                );


            } else{// 選曲詳細画面-------------------------------------

                controlIndex = 2;

                if(controlIndex != preControlIndex){// 出た瞬間
                    preCameraControlTransform_ = camera_->GetTransform();
                    cameraTimer = Timer(kCameraControlTime);
                }

                // カメラ補間
                camera_->SetTransform(
                    MyFunc::Interpolate(
                        preCameraControlTransform_,
                        cameraControlTransforms_[controlIndex],
                        cameraTimer.GetEase(Easing::InOutExpo)
                    )
                );
            }


        } else{// グループ選択画面---------------------------------------

            controlIndex = 0;

            if(controlIndex != preControlIndex){// 入った瞬間
                preCameraControlTransform_ = camera_->GetTransform();
                cameraTimer = Timer(kCameraControlTime);
            }


            // カメラ補間
            camera_->SetTransform(
                MyFunc::Interpolate(
                    preCameraControlTransform_,
                    cameraControlTransforms_[controlIndex],
                    cameraTimer.GetEase(Easing::InOutExpo)
                )
            );
        }
    } else{

        controlIndex = 3; // プレイ開始時のカメラ制御

        if(controlIndex != preControlIndex){// 入った瞬間
            preCameraControlTransform_ = camera_->GetTransform();
            cameraTimer = Timer(1.0f);
        }

        //カメラ補間
        camera_->SetTransform(
            MyFunc::Interpolate(
                preCameraControlTransform_,
                cameraControlTransforms_[controlIndex],
                cameraTimer.GetEase(Easing::InOutSine)
            )
        );
    }

    cameraTimer.Update();
    preControlIndex = controlIndex; // 前回のインデックスを保存


#ifdef _DEBUG

    for(int i = 0; i < 4; i++){
        // guizmoに登録
        ImGuiManager::RegisterGuizmoItem(&cameraControlTransforms_[i]);
    }

#endif // _DEBUG

}

/////////////////////////////////////////////////////////////////////////////////////
// ジャケットの更新
/////////////////////////////////////////////////////////////////////////////////////
void SongSelector::UpdateJacket(){

    if(currentSong.first){

        // ジャケットを回転
        float rotateSpeed = 3.14f * 0.25f;
        jacket3D_.rotate.y += rotateSpeed * ClockManager::DeltaTime();

        // 上下に揺らす
        float swingHeight = 0.1f; // 揺らす高さ
        float t = std::sin(ClockManager::TotalTime() * 2.0f) * swingHeight; // サイン波で上下に揺らす
        jacket3D_.translate.y = t; // ジャケットのY座標を更新

    }
}

/////////////////////////////////////////////////////////////////////////////////////
// アイテムの位置を編集
/////////////////////////////////////////////////////////////////////////////////
void SongSelector::Edit(){

#ifdef _DEBUG


    // 曲の配置編集
    if(isEditSongTransform){
        std::vector<Vector2> controlPoints;
        for(int i = 0; i < itemAimTransforms.size(); i++){

            // Guizmoで操作できるように登録
            ImGuiManager::RegisterGuizmoItem(&itemAimTransforms[i]);
            controlPoints.push_back(itemAimTransforms[i].translate);
        }

        SEED::DrawSpline(controlPoints, 8, { 1.0f,0.0f,0.0f,1.0f }, true);
    }

    // 難易度の配置編集
    if(isEditDifficultyTransform){
        std::vector<Vector2> controlPoints;
        // 難易度の配置編集
        for(int i = 0; i < difficultyAimTransforms.size(); i++){
            // Guizmoで操作できるように登録
            ImGuiManager::RegisterGuizmoItem(&difficultyAimTransforms[i]);
            controlPoints.push_back(difficultyAimTransforms[i].translate);
        }
        SEED::DrawSpline(controlPoints, 8, { 0.0f,1.0f,0.0f,1.0f }, true);
    }


    ImFunc::CustomBegin("SongSelecter", MoveOnly_TitleBar);
    {
        ImGui::Checkbox("Edit Song Transform", &isEditSongTransform);
        ImGui::Checkbox("Edit Difficulty Transform", &isEditDifficultyTransform);
        SongInfoDrawer::Edit();

        ImGui::Spacing();
        ImGui::Separator();
        if(ImGui::CollapsingHeader("選曲項目のトランスフォーム")){
            ImGui::Indent();
            for(int i = 0; i < itemAimTransforms.size(); i++){
                ImGui::PushID(i);
                ImGui::Text("Item %d", i);
                ImGui::InputFloat2("Translate", &itemAimTransforms[i].translate.x);
                ImGui::InputFloat2("Scale", &itemAimTransforms[i].scale.x);
                ImGui::InputFloat("Rotation", &itemAimTransforms[i].rotate);
                ImGui::PopID();
                ImGui::Separator();
            }
            ImGui::Unindent();
        }

        ImGui::Spacing();
        ImGui::Separator();
        if(ImGui::CollapsingHeader("難易度項目のトランスフォーム")){
            ImGui::Indent();
            for(int i = 0; i < difficultyAimTransforms.size(); i++){
                ImGui::PushID(i);
                ImGui::Text("Item %d", i);
                ImGui::InputFloat2("Translate", &difficultyAimTransforms[i].translate.x);
                ImGui::InputFloat2("Scale", &difficultyAimTransforms[i].scale.x);
                ImGui::InputFloat("Rotation", &difficultyAimTransforms[i].rotate);
                ImGui::PopID();
                ImGui::Separator();
            }
            ImGui::Unindent();
        }

        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Text("グループ分け: %s", SelectUtil::groupModeNames[int(currentGroupMode)].c_str());
        ImGui::Text("楽曲ソート: %s", SelectUtil::sortModeNames[int(currentSortMode)].c_str());
        ImGui::Text("現在のグループ名: %s", currentGroup ? currentGroup->groupName.c_str() : "なし");
        ImGui::Text("現在のグループの楽曲数: %d", currentGroup ? currentGroup->groupMembers.size() : 0);
        ImGui::Text("現在の難易度: %d", int(currentDifficulty));
        ImGui::Text("楽曲番号: %d", currentSongIndex + 1);
        ImGui::Text("slideOutTimer: %.2f", slideOutTimer);

        //カメラ制御点の編集
        ImGui::Separator();
        static int currentEdittingCameraControlIndex = 0;
        ImFunc::Combo("カメラ制御点", currentEdittingCameraControlIndex, { "グループ選択時","曲選択時","難易度選択時","決定時" });
        if(ImGui::Button("カメラのトランスフォームを保存")){
            auto* debugCamera = SEED::GetCamera("debug");
            cameraControlTransforms_[currentEdittingCameraControlIndex] = debugCamera->GetTransform();
        }

        ImGui::End();
    }

#endif // _DEBUG
}

nlohmann::json SongSelector::ToJson(){
    nlohmann::ordered_json json;
    // 現在の選択状態を保存
    json["currentGroupIndex"] = currentGroupIndex;
    json["currentSongIndex"] = currentSongIndex;
    json["currentGroupMode"] = (int)currentGroupMode;
    json["currentSortMode"] = (int)currentSortMode;
    json["currentDifficulty"] = (int)currentDifficulty;

    // アイテムの位置を保存
    for(int i = 0; i < itemAimTransforms.size(); i++){
        json["itemAimTransforms"].push_back(itemAimTransforms[i]);
    }

    // 難易度の位置を保存
    for(int i = 0; i < difficultyAimTransforms.size(); i++){
        json["difficultyTransforms"].push_back(difficultyAimTransforms[i]);
    }

    // カメラのトランスフォーム
    for(int i = 0; i < 4; i++){
        json["cameraControlTransforms"].push_back(cameraControlTransforms_[i]);
    }

    return json;
}

void SongSelector::FromJson(const nlohmann::json& json){
    // 現在の選択状態を復元
    currentGroupIndex = json["currentGroupIndex"];
    currentSongIndex = json["currentSongIndex"];
    currentGroupMode = static_cast<GroupMode>(json["currentGroupMode"]);
    currentSortMode = static_cast<SortMode>(json["currentSortMode"]);
    currentDifficulty = static_cast<TrackDifficulty>(json["currentDifficulty"]);
    SelectBackGroundDrawer::currentDifficulty = currentDifficulty; // 背景の難易度を更新

    // アイテムの位置を復元
    for(int i = 0; i < itemAimTransforms.size(); i++){
        if(i < json["itemAimTransforms"].size()){
            itemAimTransforms[i] = json["itemAimTransforms"][i];
        } else{
            itemAimTransforms[i] = Transform2D(); // デフォルト値を設定
        }
    }

    // 難易度の位置を復元
    if(json.contains("difficultyTransforms")){
        for(int i = 0; i < difficultyAimTransforms.size(); i++){
            if(i < json["difficultyTransforms"].size()){
                difficultyAimTransforms[i] = json["difficultyTransforms"][i];
            } else{
                difficultyAimTransforms[i] = Transform2D(); // デフォルト値を設定
            }
        }
    }

    // カメラのトランスフォームを復元
    if(json.contains("cameraControlTransforms")){
        for(int i = 0; i < 4; i++){
            if(i < json["cameraControlTransforms"].size()){
                cameraControlTransforms_[i] = json["cameraControlTransforms"][i];
            } else{
                cameraControlTransforms_[i] = Transform(); // デフォルト値を設定
            }
        }
    }
}
