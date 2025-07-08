#include "SongSelector.h"
#include <SEED/Lib/Functions/MyFunc/MyFunc.h>
#include <SEED/Source/SEED.h>
#include <SEED/Source/Manager/ImGuiManager/ImGuiManager.h>
#include <SEED/Lib/Structs/Transform.h>

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
        currentSong = nullptr; // グループが空の場合はnullに設定
    }

    // 可視曲の初期化
    UpdateVisibleItems();
}

////////////////////////////////////////////////////////////////////
// 更新処理
////////////////////////////////////////////////////////////////////
void SongSelector::Update(){

    // 項目の選択
    SelectItems();
    // UIの更新
    UpdateSongUI();
    // 制御点の位置を編集
    Edit();
}


////////////////////////////////////////////////////////////////////
// 描画処理
////////////////////////////////////////////////////////////////////
void SongSelector::Draw(){

    // 曲のリストを描画
    if(selectMode_ == SelectMode::Song){
        for(int i = 0; i < visibleSongs.size(); i++){
            if(visibleSongs[i]){
                SongInfoDrawer::Draw(*visibleSongs[i], visibleItemTransforms[i], currentDifficulty, i == centerIndex);
            }
        }
    } else{
        for(int i = 0; i < visibleGroups.size(); i++){
            if(visibleGroups[i]){
                SongInfoDrawer::Draw(*visibleGroups[i], visibleItemTransforms[i], currentDifficulty, i == centerIndex);
            }
        }
    }
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
        // グループ分けしない場合は、全ての曲を一つのグループにまとめる
        songGroups.emplace_back(SongGroup());
        songGroups.back().groupName = "全楽曲"; // グループ名を設定
        for(auto& song : songList){
            songGroups.back().groupMembers.push_back(song.get());
        }

        break;
    }
    case GroupMode::Genre:
    {
        // ジャンルごとにグループ分け
        for(auto& song : songList){
            auto genre = song->genre;
            // ジャンルが空の場合はスキップ
            if(genre == std::nullopt){ continue; }

            // 既存のグループを探す
            auto it = std::find_if(songGroups.begin(), songGroups.end(),
                [&genre](const SongGroup& group){
                return !group.groupMembers.empty() && group.groupMembers.front()->genre == genre;
            });

            // 見つからなければ新しいグループを作成
            if(it == songGroups.end()){
                songGroups.emplace_back(SongGroup());
                it = std::prev(songGroups.end());
                songGroups.back().groupName = GroupNameUtils::genreNames[int(genre.value())]; // グループ名を設定
            }

            // グループに曲を追加
            it->groupMembers.push_back(song.get());
        }

        // ジャンルごとにソート
        songGroups.sort(
            [](const SongGroup& a, const SongGroup& b){
            return a.groupMembers.front()->genre < b.groupMembers.front()->genre;
        });

        break;
    }
    case GroupMode::Difficulty:
    {
        // 難易度ごとにグループ分け
        for(auto& song : songList){
            auto difficulty = song->difficulty[(int)currentDifficulty];

            // 既存のグループを探す
            auto it = std::find_if(songGroups.begin(), songGroups.end(),
                [&](const SongGroup& group){
                return !group.groupMembers.empty() &&
                    group.groupMembers.front()->difficulty[(int)currentDifficulty] == difficulty;
            });

            // 見つからなければ新しいグループを作成
            if(it == songGroups.end()){
                songGroups.emplace_back(SongGroup());
                it = std::prev(songGroups.end());
                songGroups.back().groupName = std::to_string(difficulty); // グループ名を設定
            }

            // グループに曲を追加
            it->groupMembers.push_back(song.get());
        }

        // 難易度ごとにソート
        songGroups.sort(
            [&](const SongGroup& a, const SongGroup& b){
            return a.groupMembers.front()->difficulty[(int)currentDifficulty] <
                b.groupMembers.front()->difficulty[(int)currentDifficulty];
        });

        break;
    }
    case GroupMode::Rank:
    {
        // ランクごとにグループ分け
        for(auto& song : songList){
            ScoreRank rank = song->ranks[(int)currentDifficulty];

            // 既存のグループを探す
            auto it = std::find_if(songGroups.begin(), songGroups.end(),
                [&](const SongGroup& group){
                return !group.groupMembers.empty() &&
                    group.groupMembers.front()->ranks[(int)currentDifficulty] == rank;
            });
            // 見つからなければ新しいグループを作成
            if(it == songGroups.end()){
                songGroups.emplace_back(SongGroup());
                it = std::prev(songGroups.end());
                songGroups.back().groupName = GroupNameUtils::rankNames[(int)rank]; // グループ名を設定
            }
            // グループに曲を追加
            it->groupMembers.push_back(song.get());
        }

        // ランクごとにソート
        songGroups.sort(
            [&](const SongGroup& a, const SongGroup& b){
            return a.groupMembers.front()->ranks[(int)currentDifficulty] <
                b.groupMembers.front()->ranks[(int)currentDifficulty];
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
                [](const SongInfo* a, const SongInfo* b){
                return MyFunc::CompareStr(a->songName, b->songName);
            });
            break;

        case SortMode::Artist:
            // アーティストでソート
            group.groupMembers.sort(
                [](const SongInfo* a, const SongInfo* b){
                return MyFunc::CompareStr(a->artistName, b->artistName);
            });
            break;

        case SortMode::Difficulty:
            // 難易度でソート
            group.groupMembers.sort(
                [&](const SongInfo* a, const SongInfo* b){
                return a->difficulty[(int)currentDifficulty] < b->difficulty[(int)currentDifficulty];
            });
            break;

        case SortMode::Score:

            // スコアでソート
            group.groupMembers.sort(
                [&](const SongInfo* a, const SongInfo* b){
                return a->score[(int)currentDifficulty] < b->score[(int)currentDifficulty];
            });
            break;

        case SortMode::ClearIcon:

            // クリアアイコンでソート
            group.groupMembers.sort(
                [&](const SongInfo* a, const SongInfo* b){
                return a->clearIcons[(int)currentDifficulty] < b->clearIcons[(int)currentDifficulty];
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
            song->sortIndex = nextSongIndex++;
            song->groupIndex = group.groupIdx;
        }
    }

    // 現在のインデックスを更新
    if(currentSong){
        currentGroupIndex = currentSong->groupIndex;
        currentSongIndex = currentSong->sortIndex;
    }
}

/////////////////////////////////////////////////////////////////////////////////////
// 可視曲の更新処理
/////////////////////////////////////////////////////////////////////////////////////
void SongSelector::UpdateVisibleItems(){

    if(!currentGroup){ return; }
    if(currentGroup->groupMembers.empty()){ return; }

    {
        // 可視オブジェクトの更新
        auto it = currentGroup->groupMembers.begin();
        std::advance(it, currentSongIndex);
        visibleSongs[centerIndex] = *it; // 現在の曲を可視曲リストに設定

        for(int i = 0; i <= kVisibleRadius; i++){

            int32_t index[2] = {
                MyFunc::Spiral(currentSongIndex + i, 0, int(currentGroup->groupMembers.size()) - 1),
                MyFunc::Spiral(currentSongIndex - i, 0, int(currentGroup->groupMembers.size()) - 1)
            };

            auto it2 = currentGroup->groupMembers.begin();
            std::advance(it2, index[0]);
            visibleSongs[centerIndex + i] = *it2; // 前方の曲を可視曲リストに設定

            auto it3 = currentGroup->groupMembers.begin();
            std::advance(it3, index[1]);
            visibleSongs[centerIndex - i] = *it3; // 後方の曲を可視曲リストに設定
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

    //----------------------- 選択モードの変更--------------------------
    if(selectMode_ == SelectMode::Song){
        if(Input::IsTriggerKey({ DIK_A,DIK_LEFT })){
            selectMode_ = SelectMode::Group;
        }
    } else{

        if(Input::IsTriggerKey(DIK_Q)){
            currentGroupMode = GroupMode(MyFunc::Spiral(int(currentGroupMode) - 1, 0, int(GroupMode::kMaxCount) - 1));
            Sort();
            UpdateVisibleItems();

        } else if(Input::IsTriggerKey(DIK_E)){
            currentGroupMode = GroupMode(MyFunc::Spiral(int(currentGroupMode) + 1, 0, int(GroupMode::kMaxCount) - 1));
            Sort();
            UpdateVisibleItems();

        } else if(Input::IsTriggerKey({ DIK_D,DIK_RIGHT,DIK_SPACE })){
            selectMode_ = SelectMode::Song;
        }
    }


    //---------------- 入力に応じて選択曲・グループを変更 -----------------
    if(changed){
        if(selectMode_ == SelectMode::Song){
            if(currentGroup){
                currentSongIndex = MyFunc::Spiral(currentSongIndex + 1, 0, int(currentGroup->groupMembers.size()) - 1);

                // 現在の曲を更新
                auto it = currentGroup->groupMembers.begin();
                std::advance(it, currentSongIndex);
                currentSong = *it;
            }

        } else{
            if(songGroups.size()){
                currentGroupIndex = MyFunc::Spiral(currentGroupIndex + 1, 0, int(songGroups.size()) - 1);

                // 現在の曲を更新
                auto it = songGroups.begin();
                std::advance(it, currentGroupIndex);
                currentGroup = &(*it);
            }
        }

        // 入力方向の更新
        if(Input::IsPressKey({ DIK_W,DIK_UP })){
            changeDirection = UpDown::UP;
        } else{
            changeDirection = UpDown::DOWN;
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
                        MyFunc::Interpolate(itemAimTransforms[1 + i], itemAimTransforms[2 + i], ease);

                } else{// 下を押したので上に移動
                    visibleItemTransforms[i] =
                        MyFunc::Interpolate(itemAimTransforms[1 + i], itemAimTransforms[i], ease);
                }
            }
        }


        // 一定時間経過したらアイテム変更
        if(itemChangeTimer >= kItemChangeTime){

            // アイテム変更フラグをリセット
            isItemChanged = false;

            // 可視曲を更新
            UpdateVisibleItems();

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
        // アイテムの座標を設定(動いていないときの座標に設定)
        for(int i = 0; i < visibleSongs.size(); i++){
            if(visibleSongs[i]){
                visibleItemTransforms[i] = itemAimTransforms[1 + i];
            }
        }
    }
}

/////////////////////////////////////////////////////////////////////////////////////
// アイテムの位置を編集
/////////////////////////////////////////////////////////////////////////////////
void SongSelector::Edit(){

#ifdef _DEBUG

    std::vector<Vector2> controlPoints;

    for(int i = 0; i < itemAimTransforms.size(); i++){

        // Guizmoで操作できるように登録
        ImGuiManager::RegisterGuizmoItem(&itemAimTransforms[i]);
        controlPoints.push_back(itemAimTransforms[i].translate);
    }

    // スプライン曲線
    if(splineCurveVisible){
        SEED::DrawSpline(controlPoints, 8, { 1.0f,0.0f,0.0f,1.0f }, true);
    }

    ImFunc::CustomBegin("SongSelecter", MoveOnly_TitleBar);
    {
        SongInfoDrawer::Edit();

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

    return json;
}

void SongSelector::FromJson(const nlohmann::json& json){
    // 現在の選択状態を復元
    currentGroupIndex = json["currentGroupIndex"];
    currentSongIndex = json["currentSongIndex"];
    currentGroupMode = static_cast<GroupMode>(json["currentGroupMode"]);
    currentSortMode = static_cast<SortMode>(json["currentSortMode"]);
    currentDifficulty = static_cast<TrackDifficulty>(json["currentDifficulty"]);

    // アイテムの位置を復元
    for(int i = 0; i < itemAimTransforms.size(); i++){
        if(i < json["itemAimTransforms"].size()){
            itemAimTransforms[i] = json["itemAimTransforms"][i];
        } else{
            itemAimTransforms[i] = Transform2D(); // デフォルト値を設定
        }
    }
}
