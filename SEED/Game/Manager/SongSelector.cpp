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
}

void SongSelector::Initialize(){

    // NoteDatasの階層にあるフォルダ名を一覧取得
    auto songFolders = MyFunc::GetFolderList("Resources/NoteDatas");

    // フォルダ名を元に楽曲情報を構築
    for(const auto& folderName : songFolders){
        auto& data = songList.emplace_back(new SongInfo());
        data->Initialize(folderName);
    }

    // 描画クラスの初期化
    SongInfoDrawer::Initialize();

    // jsonから設定を読み込む
    FromJson();

    // UIの初期化
    InitializeUIs();

    // ソート処理を実行
    Sort();

    // Timerの初期化
    inputTimer_.Initialize(0.3f, 0.3f);
    itemShiftTimer_.Initialize(0.2f, 0.2f);

    // 初期位置に移動
    ShiftItem(true);

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

    // パーティクルを初期化しエミッターを読み込む
    ParticleManager::DeleteAll();// 既存のエフェクトを削除
    auto* hierarchy = GameSystem::GetScene()->GetHierarchy();
    hierarchy->LoadObject("SelectScene/cubeParticle.prefab");

    // 入力関数の初期化
    InitializeInput();
}


/////////////////////////////////////////////////////////////////////
// フレーム終了時処理
/////////////////////////////////////////////////////////////////////
void SongSelector::EndFrame(){
    // 曲の選択が完了したら、曲の詳細を非表示にする
    if(changeSceneOrder_){

        if(selectMode_ == SelectMode::Difficulty){
            // ゲームシーンに移行
            Scene_Clear::SetJacketPath(
                "Resources/NoteDatas/" + currentSong.first->folderName + "/" + currentSong.first->folderName + ".png"
            );

            auto* scene = GameSystem::GetScene();
            GameSystem::GetScene()->ChangeState(new GameState_Play(scene, *currentSong.first, (int)currentDifficulty));
            return;
        } else{
            //titleに戻る
            GameSystem::GetScene()->ChangeScene("Title");
            return;
        }
    }
}

////////////////////////////////////////////////////////////////////
// 更新処理
////////////////////////////////////////////////////////////////////
void SongSelector::Update(){

    // アイテム移動処理
    if(isShiftItem_){
        ShiftItem();
    }

    // 項目の選択
    if(!isTransitionToDifficultySelect_){
        SelectItems();
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

    } else{

    }

    // ジャケットの描画
    SEED::DrawQuad(jacket3D_);
}

///////////////////////////////////////////////////////////////
// 入力関数初期化
///////////////////////////////////////////////////////////////
void SongSelector::InitializeInput(){

    // 縦方向入力-----------------------------------------------------
    verticalInput_.Value = []{
        if(Input::IsPressKey({ DIK_W,DIK_UP })){
            return UpDown::UP;
        }
        if(Input::IsPressKey({ DIK_S,DIK_DOWN })){
            return UpDown::DOWN;
        }
        return UpDown::NONE;
    };
    verticalInput_.Trigger = []{
        if(Input::IsTriggerKey({ DIK_W,DIK_UP,DIK_S,DIK_DOWN })){
            return true;
        }
        return false;
    };

    // 横方向入力-----------------------------------------------------
    holozontalInput_.Value = []{
        if(Input::IsPressKey({ DIK_A,DIK_LEFT })){
            return LR::LEFT;
        }
        if(Input::IsPressKey({ DIK_D,DIK_RIGHT })){
            return LR::RIGHT;
        }
        return LR::NONE;
    };
    holozontalInput_.Trigger = []{
        if(Input::IsTriggerKey({ DIK_A,DIK_LEFT,DIK_D,DIK_RIGHT })){
            return true;
        }
        return false;
    };

    // ソート変更入力-------------------------------------------------
    modeChangeInput_.Value = []{
        if(Input::IsPressKey(DIK_Q)){
            return LR::LEFT;
        }
        if(Input::IsPressKey(DIK_E)){
            return LR::RIGHT;
        }
        return LR::NONE;
    };
    modeChangeInput_.Trigger = []{
        if(Input::IsTriggerKey({ DIK_Q,DIK_E })){
            return true;
        }
        return false;
    };

    // 決定入力-------------------------------------------------------
    decideInput_.Trigger = []{
        if(Input::IsTriggerKey({ DIK_SPACE,DIK_RETURN })){
            return true;
        }
        return false;
    };

    // 戻る入力-------------------------------------------------------
    backInput_.Trigger = []{
        if(Input::IsTriggerKey(DIK_ESCAPE)){
            return true;
        }
        return false;
    };

}

///////////////////////////////////////////////////////////////
// UI初期化
///////////////////////////////////////////////////////////////
void SongSelector::InitializeUIs(){
    // コントロールポイントの読み込み
    auto* hierarchy = GameSystem::GetScene()->GetHierarchy();
    songControlPts_ = hierarchy->LoadObject2D("SelectScene/songControlPts.prefab");
    difficultyControlPts_ = hierarchy->LoadObject2D("SelectScene/difficultyControlPts.prefab");
    songControlPts_->isActive_ = false;
    difficultyControlPts_->isActive_ = false;

    // UIアイテムの数の初期化
    size_t size = songControlPts_->GetComponent<Routine2DComponent>()->GetControlPoints().size() - 2;
    visibleSongs.resize(size);
    visibleGroups.resize(size);
    songUIs.resize(size);
    groupUIs.resize(size);

    // 楽曲UI・グループUIの読み込み
    for(size_t i = 0; i < size; i++){
        songUIs[i] = hierarchy->LoadObject2D("SelectScene/songUI.prefab");
        groupUIs[i] = hierarchy->LoadObject2D("SelectScene/genreUI.prefab");
    }

    difficultyUIs.resize((size_t)TrackDifficulty::kMaxDifficulty);
    for(size_t i = 0; i < difficultyUIs.size(); i++){
        difficultyUIs[i] = hierarchy->LoadObject2D("SelectScene/songUI.prefab");
    }

    // 選択モードに応じてアクティブを切る
    if(selectMode_ != SelectMode::Song){
        for(auto& ui : songUIs){
            ui->isActive_ = false;
        }
    }
    if(selectMode_ != SelectMode::Group){
        for(auto& ui : groupUIs){
            ui->isActive_ = false;
        }
    }
    if(selectMode_ != SelectMode::Difficulty){
        for(auto& ui : difficultyUIs){
            ui->isActive_ = false;
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
        std::sort(
            songGroups.begin(), songGroups.end(),
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
        std::sort(
            songGroups.begin(), songGroups.end(),
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
        std::sort(
            songGroups.begin(), songGroups.end(),
            [&](const SongGroup& a, const SongGroup& b){
            return a.groupMembers.front().first->ranks[(int)currentDifficulty] <
                b.groupMembers.front().first->ranks[(int)currentDifficulty];
        });

        break;
    }
    default:
        break;
    }

    // まだnullptrならデフォルト値を適用
    if(!currentGroup){
        currentGroup = &songGroups[0];
        currentSong = currentGroup->groupMembers[0];
    }
}


/////////////////////////////////////////////////////////////////////////////////////
// グループ内でのソート処理
/////////////////////////////////////////////////////////////////////////////////////
void SongSelector::SortInGroup(){

    for(auto& group : songGroups){

        auto& songs = group.groupMembers;

        switch(currentSortMode){

        case SortMode::Title:
            // タイトルでソート
            std::sort(
                songs.begin(), songs.end(),
                [](const Track& a, const Track& b){
                return MyFunc::CompareStr(a.first->songName, b.first->songName);
            });
            break;

        case SortMode::Artist:
            // アーティストでソート
            std::sort(
                songs.begin(), songs.end(),
                [](const Track& a, const Track& b){
                return MyFunc::CompareStr(a.first->artistName, b.first->artistName);
            });
            break;

        case SortMode::Difficulty:
            // 難易度でソート
            std::sort(
                songs.begin(), songs.end(),
                [&](const Track& a, const Track& b){
                return a.first->difficulty[(int)a.second] < b.first->difficulty[(int)b.second];
            });
            break;

        case SortMode::Score:

            // スコアでソート
            std::sort(
                songs.begin(), songs.end(),
                [&](const Track& a, const Track& b){
                return a.first->score[(int)a.second] < b.first->score[(int)b.second];
            });
            break;

        case SortMode::ClearIcon:

            // クリアアイコンでソート
            std::sort(
                songs.begin(), songs.end(),
                [&](const Track& a, const Track& b){
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
    // 現在の選択を更新
    currentGroup = &songGroups[currentGroupIndex];
    currentSong = currentGroup->groupMembers[currentSongIndex];

    // 真ん中に表示する要素のインデックスを計算
    centerIdx_ = int(visibleSongs.size()) / 2;

    {// 可視曲の更新-----------------------------------------

        // 真ん中の要素を選択中の曲に設定
        visibleSongs[centerIdx_] = &currentGroup->groupMembers[currentSongIndex]; // 現在の曲を可視曲リストに設定

        // 後方の要素を設定
        for(int i = centerIdx_; i < visibleSongs.size(); i++){
            int32_t dif = i - centerIdx_;
            int32_t index = MyFunc::Spiral(currentSongIndex + dif, 0, int32_t(currentGroup->groupMembers.size()) - 1);
            visibleSongs[i] = &currentGroup->groupMembers[index];
        }

        // 前方の要素を設定
        for(int i = centerIdx_; i >= 0; i--){
            int32_t dif = i - centerIdx_;
            int32_t index = MyFunc::Spiral(currentSongIndex + dif, 0, int32_t(currentGroup->groupMembers.size()) - 1);
            visibleSongs[i] = &currentGroup->groupMembers[index];
        }
    }

    {// 可視グループの更新----------------------------------------

        // 真ん中の要素を選択中のグループに設定
        visibleGroups[centerIdx_] = &songGroups[currentGroupIndex];

        // 後方の要素を設定
        for(int i = centerIdx_; i < visibleGroups.size(); i++){
            int32_t dif = i - centerIdx_;
            int32_t index = MyFunc::Spiral(currentGroupIndex + dif, 0, int32_t(songGroups.size()) - 1);
            visibleGroups[i] = &songGroups[index];
        }

        // 前方の要素を設定
        for(int i = centerIdx_; i >= 0; i--){
            int32_t dif = i - centerIdx_;
            int32_t index = MyFunc::Spiral(currentGroupIndex + dif, 0, int32_t(songGroups.size()) - 1);
            visibleGroups[i] = &songGroups[index];
        }
    }
}



//////////////////////////////////////////////////////////////////////////////////
// 項目の選択処理
//////////////////////////////////////////////////////////////////////////////////
void SongSelector::SelectItems(){

    // 選択モードに応じて処理を分岐
    if(selectMode_ == SelectMode::Song){
        // 曲選択処理
        SelectSong();

    } else if(selectMode_ == SelectMode::Group){
        // グループ選択処理
        SelectGroup();

    } else if(selectMode_ == SelectMode::Difficulty){
        // 難易度選択処理
        SelectDifficulty();
    }
}

/////////////////////////////////////////////////////////////////////////////////////
// 楽曲選択処理
/////////////////////////////////////////////////////////////////////////////////////
void SongSelector::SelectSong(){

    // 各種入力値の取得
    UpDown verticalValue = verticalInput_.Value();
    LR modeChangeValue = modeChangeInput_.Value();

    // 縦方向入力がある場合
    if(verticalValue != UpDown::NONE){
        inputTimer_.Update(1.0f, true);

        // タイマーがループしたらインデックスを更新
        if(inputTimer_.IsLoopedNow()){
            // 可視曲の更新
            currentSongIndex += (verticalValue == UpDown::UP) ? -1 : 1;
            currentSongIndex = MyFunc::Spiral(currentSongIndex, 0, int32_t(currentGroup->groupMembers.size()) - 1);
            isShiftItem_ = true;
            shiftDirection_ = verticalValue;
            UpdateVisibleItems();
            itemShiftTimer_.Reset();
            return;
        }

    } else{
        // 入力がない場合はタイマーを最大値に固定(次押した際にすぐ動くように)
        inputTimer_.currentTime = inputTimer_.duration;
    }

    // ソートモードの変更
    if(modeChangeValue != LR::NONE){
        int sortModeInt = (int)currentSortMode;
        sortModeInt += (modeChangeValue == LR::LEFT) ? -1 : 1;
        sortModeInt = MyFunc::Spiral(sortModeInt, 0, (int)SortMode::kMaxCount - 1);
        currentSortMode = (SortMode)sortModeInt;

        // ソート処理を実行
        Sort();
        return;
    }

    // 決定入力があれば難易度選択へ移行
    if(decideInput_.Trigger()){
        selectMode_ = SelectMode::Difficulty;
        for(auto& ui : difficultyUIs){
            ui->isActive_ = true;
        }
        for(auto& ui : songUIs){
            ui->isActive_ = false;
        }
        return;
    }

    // 戻る入力があればグループ選択へ戻る
    if(backInput_.Trigger()){
        selectMode_ = SelectMode::Group;

        // UIのアクティブ状態を切り替え
        for(auto& ui : groupUIs){
            ui->isActive_ = true;
        }
        for(auto& ui : songUIs){
            ui->isActive_ = false;
        }

        return;
    }
}

/////////////////////////////////////////////////////////////////////////////////////
// グループ選択処理
/////////////////////////////////////////////////////////////////////////////////////
void SongSelector::SelectGroup(){

    // 各種入力値の取得
    UpDown verticalValue = verticalInput_.Value();
    LR modeChangeValue = modeChangeInput_.Value();

    // 縦方向入力がある場合
    if(verticalValue != UpDown::NONE){
        inputTimer_.Update(1.0f, true);

        // タイマーがループしたらインデックスを更新
        if(inputTimer_.IsLoopedNow()){
            currentGroupIndex += (verticalValue == UpDown::UP) ? -1 : 1;
            currentGroupIndex = MyFunc::Spiral(currentGroupIndex, 0, int32_t(songGroups.size()) - 1);
            UpdateVisibleItems();
            isShiftItem_ = true;
            shiftDirection_ = verticalValue;
            itemShiftTimer_.Reset();
            return;
        }

    } else{
        // 入力がない場合はタイマーを最大値に固定(次押した際にすぐ動くように)
        inputTimer_.currentTime = inputTimer_.duration;
    }

    // グループモードの変更
    if(modeChangeValue != LR::NONE){
        int groupModeInt = (int)currentGroupMode;
        groupModeInt += (modeChangeValue == LR::LEFT) ? -1 : 1;
        groupModeInt = MyFunc::Spiral(groupModeInt, 0, (int)GroupMode::kMaxCount - 1);
        currentGroupMode = (GroupMode)groupModeInt;

        // ソート処理を実行
        Sort();
        return;
    }

    // 決定入力があれば楽曲選択へ移行
    if(decideInput_.Trigger()){
        selectMode_ = SelectMode::Song;

        // UIのアクティブ状態を切り替え
        for(auto& ui : songUIs){
            ui->isActive_ = true;
        }
        for(auto& ui : groupUIs){
            ui->isActive_ = false;
        }

        return;
    }

    // 戻る入力があればタイトルへ戻る
    if(backInput_.Trigger()){
        changeSceneOrder_ = true;
        return;
    }
}

/////////////////////////////////////////////////////////////////////////////////////
// 難易度選択処理
/////////////////////////////////////////////////////////////////////////////////////
void SongSelector::SelectDifficulty(){

    // 各種入力値の取得
    UpDown verticalValue = verticalInput_.Value();

    // 縦方向入力がある場合
    if(verticalValue != UpDown::NONE){
        inputTimer_.Update(1.0f, true);

        // タイマーがループしたらインデックスを更新
        if(inputTimer_.IsLoopedNow()){
            // 難易度の更新
            TrackDifficulty previousDifficulty = currentDifficulty;
            int nextDiffInt = int(currentDifficulty) + ((verticalValue == UpDown::UP) ? 1 : -1);
            nextDiffInt = std::clamp(nextDiffInt, 0, (int)TrackDifficulty::kMaxDifficulty - 1);
            currentDifficulty = (TrackDifficulty)nextDiffInt;
            UpdateVisibleItems();
            // 難易度が変わっていたらフラグを立てる
            if(currentDifficulty != previousDifficulty){
                isShiftItem_ = true;
                shiftDirection_ = verticalValue;
                itemShiftTimer_.Reset();
            }
            return;
        }

    } else{
        // 入力がない場合はタイマーを最大値に固定(次押した際にすぐ動くように)
        inputTimer_.currentTime = inputTimer_.duration;
    }

    // 決定入力があればプレイシーンへ移行
    if(decideInput_.Trigger()){
        changeSceneOrder_ = true;
        return;
    }

    // 戻る入力があれば楽曲選択へ戻る
    if(backInput_.Trigger()){
        selectMode_ = SelectMode::Song;
        // UIのアクティブ状態を切り替え
        for(auto& ui : songUIs){
            ui->isActive_ = true;
        }
        for(auto& ui : difficultyUIs){
            ui->isActive_ = false;
        }
        return;
    }
}

/////////////////////////////////////////////////////////////////////////////////////
// アイテムの移動処理
/////////////////////////////////////////////////////////////////////////////////////
void SongSelector::ShiftItem(bool allUpdate){

    // タイマーの更新
    itemShiftTimer_.Update();
    float ease = itemShiftTimer_.GetEase(Easing::OutSine);

    // アイテムの移動処理
    if(selectMode_ == SelectMode::Song or allUpdate){
        // コントロールポイントの取得
        std::vector<RoutinePoint2D> controlPts = songControlPts_->GetComponent<Routine2DComponent>()->GetControlPoints();

        // 動かす方向の取得
        int32_t dir = 0;
        if(shiftDirection_ != UpDown::NONE){
            dir = (shiftDirection_ == UpDown::UP) ? -1 : 1;
        }

        // アイテム移動処理
        for(int32_t i = 0; i < songUIs.size(); i++){
            // 終点と始点要素を求める
            int32_t aimIdx = i + 1;
            int32_t originIdx = aimIdx + dir;

            // 補完して行列を更新
            songUIs[i]->localTransform_ = MyFunc::Interpolate(
                controlPts[originIdx].first,
                controlPts[aimIdx].first,
                ease
            );
            songUIs[i]->UpdateMatrix();
        }

    }

    if(selectMode_ == SelectMode::Group or allUpdate){
        // コントロールポイントの取得
        std::vector<RoutinePoint2D> controlPts = songControlPts_->GetComponent<Routine2DComponent>()->GetControlPoints();

        // 動かす方向の取得
        int32_t dir = 0;
        if(shiftDirection_ != UpDown::NONE){
            dir = (shiftDirection_ == UpDown::UP) ? -1 : 1;
        }

        // アイテム移動処理
        for(int32_t i = 0; i < groupUIs.size(); i++){
            // 終点と始点要素を求める
            int32_t aimIdx = i + 1;
            int32_t originIdx = aimIdx + dir;

            // 補完して行列を更新
            groupUIs[i]->localTransform_ = MyFunc::Interpolate(
                controlPts[originIdx].first,
                controlPts[aimIdx].first,
                ease
            );
            groupUIs[i]->UpdateMatrix();
        }

    }

    if(selectMode_ == SelectMode::Difficulty or allUpdate){

        // コントロールポイントの取得
        std::vector<RoutinePoint2D> controlPts = difficultyControlPts_->GetComponent<Routine2DComponent>()->GetControlPoints();

        // 動かす方向の取得
        int32_t dir = 0;
        if(shiftDirection_ != UpDown::NONE){
            dir = (shiftDirection_ == UpDown::UP) ? 1 : -1;
        }

        // アイテム移動処理
        for(int32_t i = 0; i < difficultyUIs.size(); i++){
            // 終点と始点要素を求める
            int32_t originIdx = int32_t(currentDifficulty) - dir + i;
            int32_t aimIdx = originIdx + dir;
            // 補完して行列を更新
            difficultyUIs[i]->localTransform_ = MyFunc::Interpolate(
                controlPts[originIdx].first,
                controlPts[aimIdx].first,
                ease
            );
            difficultyUIs[i]->UpdateMatrix();
        }
    }

    // タイマーが終了したらリセット
    if(itemShiftTimer_.IsFinished()){
        isShiftItem_ = false;
    }
}


/////////////////////////////////////////////////////////////////////////////////////
// カメラの制御
/////////////////////////////////////////////////////////////////////////////////////
void SongSelector::CameraControl(){

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

//////////////////////////////////////////////////////////////////////////////////
// UI内容の更新
//////////////////////////////////////////////////////////////////////////////////
void SongSelector::UpdateUIContents(){

}

//////////////////////////////////////////////////////////////////////////////////
// JSONへ書き出し
//////////////////////////////////////////////////////////////////////////////////
void SongSelector::ToJson(){
}

//////////////////////////////////////////////////////////////////////////////////
// JSONから読み込み
//////////////////////////////////////////////////////////////////////////////////
void SongSelector::FromJson(){
    nlohmann::json j = MyFunc::GetJson("Resources/Jsons/Settings/song_selector.json");
    if(!j.empty()){

    }
}
