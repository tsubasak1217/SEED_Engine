#include "SongSelector.h"
#include <SEED/Lib/Functions/MyFunc.h>
#include <SEED/Source/SEED.h>
#include <SEED/Source/Manager/ImGuiManager/ImGuiManager.h>
#include <SEED/Lib/Structs/Transform.h>
#include <Game/GameSystem.h>
#include <Game/Scene/Scene_Game/State/GameState_Play.h>
#include <Game/Scene/Scene_Clear/Scene_Clear.h>
#include <Game/Objects/SongSelect/SelectBackGroundDrawer.h>

// state
#include <Game/Scene/Scene_Game/State/GameState_SelectMenu.h>

// コンストラクタ
SongSelector::SongSelector(){
}

// デストラクタ
SongSelector::~SongSelector(){
    // 設定を保存
    ToJson();
}

void SongSelector::Initialize(){

    // NoteDatasの階層にあるフォルダ名を一覧取得
    auto songFolders = MyFunc::GetFolderList("Resources/NoteDatas");

    // フォルダ名を元に楽曲情報を構築
    for(const auto& folderName : songFolders){
        auto& data = songList.emplace_back(new SongInfo());
        data->Initialize(folderName);
    }

    // jsonから設定を読み込む
    FromJson();

    // UIの初期化
    InitializeUIs();

    // ソート処理を実行
    Sort();

    // 可視項目の更新
    UpdateVisibleGroups(false);
    UpdateVisibleSongs(false, false);
    preGroupName_ = currentGroup->groupName;

    // Timerの初期化
    inputTimer_.Initialize(0.3f, 0.3f);
    itemShiftTimer_.Initialize(0.2f, 0.2f);
    toDifficultySelectTimer_.Initialize(0.8f);
    playWaitTimer_.Initialize(2.0f);
    cameraMoveTimer_.Initialize(cameraInterpolationTimes_[0]);
    buttonUIScalingTimer_.Initialize(0.8f, 0.8f);
    buttonUIPressScaleTimer_Up_.Initialize(0.3f);
    buttonUIPressScaleTimer_Down_.Initialize(0.3f);
    buttonUIPressScaleTimer_Q_.Initialize(0.3f);
    buttonUIPressScaleTimer_E_.Initialize(0.3f);

    // 初期位置に移動
    ShiftItem(true);

    // UI内容の更新
    UpdateUIContents(true);

    // ジャケット矩形の初期化
    jacket3D_ = MakeEqualQuad(3.0f);
    jacket3D_.cullMode = D3D12_CULL_MODE_NONE; // カリングなし
    jacket3D_.lightingType = LIGHTINGTYPE_NONE; // ライティングなし
    jacket3D_.GH = TextureManager::LoadTexture(
        "../../Resources/NoteDatas/" + currentSong.first->folderName + "/" + currentSong.first->folderName + ".png"
    );

    // カメラコントロールポイントの読み込み
    auto* hierarchy = GameSystem::GetScene()->GetHierarchy();
    cameraControlPts_ = hierarchy->LoadObject("SelectScene/cameraControlPts.prefab");
    auto& controlPoints = cameraControlPts_->GetComponent<Routine3DComponent>()->GetControlPoints();
    if(!controlPoints.empty()){
        preCameraTransform_ = controlPoints[0].first;
    }

    // カメラの取得
    SEED::RemoveCamera("gameCamera");
    SEED::SetMainCamera("default");
    camera_ = SEED::GetMainCamera();

    // パーティクルを初期化しエミッターを読み込む
    ParticleManager::DeleteAll();// 既存のエフェクトを削除
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
        }
    }
}

////////////////////////////////////////////////////////////////////
// 更新処理
////////////////////////////////////////////////////////////////////
void SongSelector::Update(){

    if(!isPlayWaiting_){
        // 項目の選択
        if(!isTransitionDifficulty_Select_){
            SelectItems();

        } else{// 難易度選択へ移行中
            ToDifficultySelectUpdate(selectMode_ == SelectMode::Difficulty ? 1.0f : -1.0f);
        }

        // アイテム移動処理
        if(isShiftItem_){
            ShiftItem();
        }

        // 背景描画情報の更新
        UpdateBGDrawerInfo();

    } else{
        PlayWaitUpdate();
    }

    // カメラの制御
    CameraControl();

    // ジャケットの更新
    UpdateJacket();

    // ボタンUIの更新
    UpdateSelectButtonUIs();

#ifdef _DEBUG
    // 編集
    Edit();
#endif // _DEBUG
}


////////////////////////////////////////////////////////////////////
// 描画処理
////////////////////////////////////////////////////////////////////
void SongSelector::Draw(){
    // ジャケットの描画
    SEED::DrawQuad(jacket3D_);
}

///////////////////////////////////////////////////////////////
// 入力関数初期化
///////////////////////////////////////////////////////////////
void SongSelector::InitializeInput(){

    // 縦方向入力-----------------------------------------------------
    verticalInput_.Value = []{
        if(GameSystem::GetScene()->HasEvent()){
            return UpDown::NONE;// イベント中は入力を受け付けない
        }
        if(Input::IsPressKey({ DIK_W,DIK_UP })){
            return UpDown::UP;
        }
        if(Input::IsPressKey({ DIK_S,DIK_DOWN })){
            return UpDown::DOWN;
        }
        return UpDown::NONE;
    };
    verticalInput_.Trigger = []{
        if(GameSystem::GetScene()->HasEvent()){
            return false;// イベント中は入力を受け付けない
        }
        if(Input::IsTriggerKey({ DIK_W,DIK_UP,DIK_S,DIK_DOWN })){
            return true;
        }
        return false;
    };

    // 横方向入力-----------------------------------------------------
    holozontalInput_.Value = []{
        if(GameSystem::GetScene()->HasEvent()){
            return LR::NONE;// イベント中は入力を受け付けない
        }
        if(Input::IsPressKey({ DIK_A,DIK_LEFT })){
            return LR::LEFT;
        }
        if(Input::IsPressKey({ DIK_D,DIK_RIGHT })){
            return LR::RIGHT;
        }
        return LR::NONE;
    };
    holozontalInput_.Trigger = []{
        if(GameSystem::GetScene()->HasEvent()){
            return false;// イベント中は入力を受け付けない
        }
        if(Input::IsTriggerKey({ DIK_A,DIK_LEFT,DIK_D,DIK_RIGHT })){
            return true;
        }
        return false;
    };

    // ソート変更入力-------------------------------------------------
    modeChangeInput_.Value = []{
        if(GameSystem::GetScene()->HasEvent()){
            return LR::NONE;// イベント中は入力を受け付けない
        }
        if(Input::IsPressKey(DIK_Q)){
            return LR::LEFT;
        }
        if(Input::IsPressKey(DIK_E)){
            return LR::RIGHT;
        }
        return LR::NONE;
    };
    modeChangeInput_.Trigger = []{
        if(GameSystem::GetScene()->HasEvent()){
            return false;// イベント中は入力を受け付けない
        }
        if(Input::IsTriggerKey({ DIK_Q,DIK_E })){
            return true;
        }
        return false;
    };

    // 決定入力-------------------------------------------------------
    decideInput_.Trigger = []{
        if(GameSystem::GetScene()->HasEvent()){
            return false;// イベント中は入力を受け付けない
        }
        if(Input::IsTriggerKey({ DIK_SPACE,DIK_RETURN })){
            return true;
        }
        return false;
    };

    // 戻る入力-------------------------------------------------------
    backInput_.Trigger = []{
        if(GameSystem::GetScene()->HasEvent()){
            return false;// イベント中は入力を受け付けない
        }
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
    songControlPts_->SetIsActive(false);
    difficultyControlPts_->SetIsActive(false);

    // UIアイテムの数の初期化
    size_t size = songControlPts_->GetComponent<Routine2DComponent>()->GetControlPoints().size() - 2;
    visibleSongs.resize(size);
    visibleGroups.resize(size);
    songUIs.resize(size);
    groupUIs.resize(size);

    // centerのTransform2Dを取得
    int centerIdx = (int)(size / 2);
    songUICenterTransform_ = songControlPts_->GetComponent<Routine2DComponent>()->GetControlPoint(centerIdx + 1);
    centerIdx = (int)difficultyControlPts_->GetComponent<Routine2DComponent>()->GetControlPoints().size() / 2;
    difficultyUICenterTransform_ = difficultyControlPts_->GetComponent<Routine2DComponent>()->GetControlPoint(centerIdx);

    // 楽曲UI・グループUIの読み込み
    for(size_t i = 0; i < size; i++){
        songUIs[i] = hierarchy->LoadObject2D("SelectScene/songUI.prefab");
        groupUIs[i] = hierarchy->LoadObject2D("SelectScene/genreUI.prefab");
        songUIs[i]->GetChild(0)->SetIsActive(false);
        groupUIs[i]->GetChild(0)->SetIsActive(false);
    }

    difficultyUIs.resize((size_t)TrackDifficulty::kMaxDifficulty);
    for(size_t i = 0; i < difficultyUIs.size(); i++){
        difficultyUIs[i] = hierarchy->LoadObject2D("SelectScene/songUI.prefab");
        difficultyUIs[i]->aditionalTransform_.scale = Vector2(0.0f);
        // "space"UIを非アクティブにする
        difficultyUIs[i]->GetChild(0)->SetIsActive(false);
    }

    // 選択モードに応じてアクティブを切る
    if(selectMode_ != SelectMode::Song){
        for(auto& ui : songUIs){
            ui->SetIsActive(false);
        }
    }
    if(selectMode_ != SelectMode::Group){
        for(auto& ui : groupUIs){
            ui->SetIsActive(false);
        }
    }
    if(selectMode_ != SelectMode::Difficulty){
        for(auto& ui : difficultyUIs){
            ui->SetIsActive(false);
        }
    }

    // ボタンUIの読み込み
    songSelectButtonUI_ = hierarchy->LoadObject2D("SelectScene/ui_WS_1.prefab");
    difficultySelectButtonUI_ = hierarchy->LoadObject2D("SelectScene/ui_WS_2.prefab");
    backButtonUI_ = hierarchy->LoadObject2D("SelectScene/escUI.prefab");
    modeChangeButtonUI_ = hierarchy->LoadObject2D("SelectScene/ui_QE.prefab");

}

///////////////////////////////////////////////////////////////
// ソート処理
///////////////////////////////////////////////////////////////
void SongSelector::Sort(){
    // まずグループ分け
    CreateGroup();
    // グループ内でのソート
    SortInGroup();
    // 各曲・グループのインデックスを更新
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

    // まだnullptrならデフォルト値を適用
    if(!currentGroup){
        currentGroupIndex = std::clamp(currentGroupIndex, 0, int32_t(songGroups.size()) - 1);
        currentSongIndex = std::clamp(currentSongIndex, 0, int32_t(songGroups[currentGroupIndex].groupMembers.size()) - 1);
        currentGroup = &songGroups[currentGroupIndex];
        currentSong = currentGroup->groupMembers[currentSongIndex];
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
// 可視曲・グループの更新処理
/////////////////////////////////////////////////////////////////////////////////////

void SongSelector::UpdateVisibleSongs(bool isPlayAudio, bool isUpdateUIs){

    if(!currentGroup){ return; }
    if(currentGroup->groupMembers.empty()){ return; }

    // 現在の選択を更新
    currentGroup = &songGroups[currentGroupIndex];
    currentSong = currentGroup->groupMembers[currentSongIndex];
    currentDifficulty = currentSong.second;

    if(currentSong.first->songName != preSongName_){
        // 曲名が変わったら再生する曲を切り替える
        if(isPlayAudio){
            preSongName_ = currentSong.first->songName;
            AudioManager::EndAudio(songHandle_);
            songHandle_ = AudioManager::PlayAudio(
                currentSong.first->audioFilePath,
                true, 0.5f
            );
        } else{
            AudioManager::EndAudio(songHandle_);
            songHandle_ = AudioManager::PlayAudio(
                AudioDictionary::Get("SelectBGM"),
                true, 0.5f
            );
        }
    }

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

    // UI情報の更新
    if(isUpdateUIs){
        UpdateUIContents();
    }
}

void SongSelector::UpdateVisibleGroups(bool isUpdateUIs){

    if(!currentGroup){ return; }
    if(currentGroup->groupMembers.empty()){ return; }
    // 現在の選択を更新
    currentGroup = &songGroups[currentGroupIndex];

    // 真ん中に表示する要素のインデックスを計算
    centerIdx_ = int(visibleSongs.size()) / 2;

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

    // UI情報の更新
    if(isUpdateUIs){
        UpdateUIContents();
    }
}


/////////////////////////////////////////////////////////////////////////////////////
// 曲の検索処理
/////////////////////////////////////////////////////////////////////////////////////
void SongSelector::FindTrack(const Track& track, int32_t& outSongIndex, int32_t& outGroupIndex){

    // 全グループを走査
    for(size_t g = 0; g < songGroups.size(); g++){
        const auto& group = songGroups[g];

        // グループ内のメンバーを検索
        for(size_t s = 0; s < group.groupMembers.size(); s++){
            const auto& member = group.groupMembers[s];
            if(member.first == track.first && member.second == track.second){
                outGroupIndex = static_cast<int32_t>(g);
                outSongIndex = static_cast<int32_t>(s);
                return;
            }
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
            UpdateVisibleSongs();
            itemShiftTimer_.Reset();
            return;
        }

    } else{
        // 入力がない場合はタイマーを最大値に固定(次押した際にすぐ動くように)
        inputTimer_.currentTime = inputTimer_.duration;
    }

    if(!isShiftItem_){
        // ソートモードの変更
        if(modeChangeValue != LR::NONE && modeChangeInput_.Trigger()){
            int sortModeInt = (int)currentSortMode;
            sortModeInt += (modeChangeValue == LR::LEFT) ? -1 : 1;
            sortModeInt = MyFunc::Spiral(sortModeInt, 0, (int)SortMode::kMaxCount - 1);
            currentSortMode = (SortMode)sortModeInt;

            // ソート処理を実行
            Sort();
            // 項目の更新
            UpdateVisibleSongs();
            UpdateUIContents();
            return;
        }

        // 決定入力があれば難易度選択へ移行
        if(decideInput_.Trigger()){
            selectMode_ = SelectMode::Difficulty;
            currentDifficulty = currentSong.second;
            isTransitionDifficulty_Select_ = true;
            UpdateUIContents();
            ShiftItem();
            // UIのアクティブ状態を切り替え
            for(auto& ui : difficultyUIs){
                ui->SetIsActive(true, false);
            }

            return;
        }

        // 戻る入力があればグループ選択へ戻る
        if(backInput_.Trigger()){
            selectMode_ = SelectMode::Group;
            preGroupName_ = currentGroup->groupName;


            // 項目の更新
            UpdateVisibleGroups();
            UpdateUIContents();
            ShiftItem();

            // グループ選択時はデフォルト選曲BGMを再生
            AudioManager::EndAudio(songHandle_);
            preSongName_.clear();
            songHandle_ = AudioManager::PlayAudio(
                AudioDictionary::Get("SelectBGM"),
                true, 0.5f
            );

            // UIのアクティブ状態を切り替え
            for(auto& ui : groupUIs){
                ui->SetIsActive(true, false);
            }
            for(auto& ui : songUIs){
                ui->SetIsActive(false);
            }

            return;
        }
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
            isShiftItem_ = true;
            shiftDirection_ = verticalValue;
            itemShiftTimer_.Reset();
            UpdateVisibleGroups();
            return;
        }

    } else{
        // 入力がない場合はタイマーを最大値に固定(次押した際にすぐ動くように)
        inputTimer_.currentTime = inputTimer_.duration;
    }

    // グループモードの変更
    if(modeChangeValue != LR::NONE && modeChangeInput_.Trigger()){
        int groupModeInt = (int)currentGroupMode;
        groupModeInt += (modeChangeValue == LR::LEFT) ? -1 : 1;
        groupModeInt = MyFunc::Spiral(groupModeInt, 0, (int)GroupMode::kMaxCount - 1);
        currentGroupMode = (GroupMode)groupModeInt;
        // ソート処理を実行
        Sort();
        // 項目の更新
        UpdateVisibleGroups();
        UpdateUIContents();
        return;
    }


    if(!isShiftItem_){
        // 決定入力があれば楽曲選択へ移行
        if(decideInput_.Trigger()){
            selectMode_ = SelectMode::Song;

            // グループが変わっていたら曲インデックスをリセット
            if(currentGroup->groupName != preGroupName_){
                currentSongIndex = 0;
            }

            // 可視曲の更新
            UpdateVisibleSongs();
            ShiftItem();

            // UIのアクティブ状態を切り替え
            for(auto& ui : songUIs){
                ui->SetIsActive(true, false);
            }
            for(auto& ui : groupUIs){
                ui->SetIsActive(false);
            }

            return;
        }

        // escでメニュー表示
        if(backInput_.Trigger()){
            auto* scene = GameSystem::GetScene();
            scene->CauseEvent(new GameState_SelectMenu(scene));

            return;
        }
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

            // 難易度が変わっていたらフラグを立てる
            if(currentDifficulty != previousDifficulty){
                isShiftItem_ = true;
                shiftDirection_ = verticalValue;
                itemShiftTimer_.Reset();

                // currentSongを現在の難易度に合わせて更新
                currentSong = { currentSong.first,currentDifficulty };
            }
            return;
        }

    } else{
        // 入力がない場合はタイマーを最大値に固定(次押した際にすぐ動くように)
        inputTimer_.currentTime = inputTimer_.duration;
    }


    if(!isShiftItem_){
        // 決定入力があればプレイシーンへ移行
        if(decideInput_.Trigger()){
            isPlayWaiting_ = true;
            return;
        }

        // 戻る入力があれば楽曲選択へ戻る
        if(backInput_.Trigger()){
            selectMode_ = SelectMode::Song;

            // currentSongを現在の難易度に合わせて更新
            currentSong = { currentSong.first,currentDifficulty };
            Sort();
            FindTrack(currentSong, currentSongIndex, currentGroupIndex);
            UpdateVisibleSongs();
            ShiftItem();

            // UIのアクティブ状態を切り替え
            isTransitionDifficulty_Select_ = true;
            for(auto& ui : songUIs){
                ui->SetIsActive(true, false);
            }

            return;
        }
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

            // 真ん中の要素の"space"UIを拡縮
            if(i == centerIdx_){
                if(selectMode_ == SelectMode::Song){
                    songUIs[i]->GetChild(0)->SetIsActive(true);
                    songUIs[i]->GetChild(0)->aditionalTransform_.scale = Vector2(ease);
                }
            }

            songUIs[i]->UpdateMatrix(true);
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

            // 真ん中の要素の"space"UIを拡縮
            if(i == centerIdx_){
                if(selectMode_ == SelectMode::Group){
                    groupUIs[i]->GetChild(0)->SetIsActive(true);
                    groupUIs[i]->GetChild(0)->aditionalTransform_.scale = Vector2(ease);
                }
            }

            groupUIs[i]->UpdateMatrix(true);
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

            if(selectMode_ == SelectMode::Difficulty){
                if(int32_t(TrackDifficulty::Parallel) - i == int32_t(currentDifficulty)){
                    // 真ん中の要素の"space"UIを拡縮
                    difficultyUIs[i]->GetChild(0)->SetIsActive(true);
                    difficultyUIs[i]->GetChild(0)->aditionalTransform_.scale = Vector2(ease);
                } else{
                    difficultyUIs[i]->GetChild(0)->SetIsActive(false);
                }
            }

            difficultyUIs[i]->UpdateMatrix(true);
        }
    }

    // タイマーが終了したらリセット
    if(itemShiftTimer_.IsFinished()){
        isShiftItem_ = false;
        shiftDirection_ = UpDown::NONE;
    }
}


/////////////////////////////////////////////////////////////////////////////////////
// カメラの制御
/////////////////////////////////////////////////////////////////////////////////////
void SongSelector::CameraControl(){

    // 制御点オブジェクトが存在しない場合は抜ける
    if(!cameraControlPts_){ return; }

    // 制御点一覧の取得
    auto& controlPoints = cameraControlPts_->GetComponent<Routine3DComponent>()->GetControlPoints();
    // 制御点が足りない場合は抜ける
    if(controlPoints.size() < 4){ return; }

    // 制御点番号の更新
    preCameraControlIdx_ = cameraControlIdx_;
    if(selectMode_ == SelectMode::Song){
        cameraControlIdx_ = 1;
    } else if(selectMode_ == SelectMode::Group){
        cameraControlIdx_ = 0;
    } else if(selectMode_ == SelectMode::Difficulty){
        if(!isPlayWaiting_){
            cameraControlIdx_ = 2;
        } else{
            cameraControlIdx_ = 3;
        }
    }

    // 制御点が切り替わった場合
    if(preCameraControlIdx_ != cameraControlIdx_){
        cameraMoveTimer_.Initialize(cameraInterpolationTimes_[cameraControlIdx_]);
        preCameraTransform_ = camera_->GetTransform();
    }

    // カメラ移動タイマーの更新
    cameraMoveTimer_.Update();
    float ease = cameraMoveTimer_.GetEase(Easing::InOutExpo);

    // カメラの位置・注視点を補完して更新
    camera_->SetTransform(
        MyFunc::Interpolate(
            preCameraTransform_,
            controlPoints[cameraControlIdx_].first,
            ease
        )
    );
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

        // ジャケットのテクスチャを更新
        jacket3D_.GH = TextureManager::LoadTexture(
            "../../Resources/NoteDatas/" + currentSong.first->folderName + "/" + currentSong.first->folderName + ".png"
        );

    }
}

//////////////////////////////////////////////////////////////////////////////////
// UI内容の更新
//////////////////////////////////////////////////////////////////////////////////
void SongSelector::UpdateUIContents(bool allUpdate){

    if(selectMode_ == SelectMode::Song or allUpdate){
        // 曲選択UIの更新
        for(int32_t i = 0; i < songUIs.size(); i++){

            auto* ui = songUIs[i]->GetComponent<UIComponent>();
            int32_t difficulty = (int)visibleSongs[i]->second;
            SongInfo* songInfo = visibleSongs[i]->first;

            // 画像の更新
            ui->GetSprite("itemBG").color = uiBackColors_[(int)difficulty];
            ui->GetSprite("levelBG").color = uiBackColors_[(int)difficulty];
            ui->GetSprite("Jacket").GH = TextureManager::LoadTexture(
                "../../Resources/NoteDatas/" + songInfo->folderName + "/" + songInfo->folderName + ".png"
            );

            // スコアを小数点4位までに変換
            std::string scoreStr = "-----------";
            std::string rankStr = "-----";
            if(songInfo->score[difficulty] > 0.0f){
                scoreStr = std::to_string(songInfo->score[difficulty]);
                scoreStr = scoreStr.substr(0, scoreStr.find(".") + 5) + "%";
                rankStr = GroupNameUtils::rankNames[(int)songInfo->ranks[difficulty]];
            }

            // テキストの更新
            ui->GetText("title").text = songInfo->songName;
            ui->GetText("artistName").text = songInfo->artistName;
            ui->GetText("score").text = scoreStr;
            ui->GetText("level").text = std::to_string(songInfo->difficulty[difficulty]);
            ui->GetText("bpm").text = "BPM" + std::to_string(static_cast<int>(songInfo->bpm));
            ui->GetText("rank").text = rankStr;

            // AP,FCアイコンの表示更新
            if(songInfo->clearIcons[difficulty] == ClearIcon::Perfect){
                ui->GetSprite("clearIcon").GH = TextureManager::LoadTexture("Select/apIcon.png");
                ui->GetSprite("clearIcon").color = Color(1.0f);

            } else if(songInfo->clearIcons[difficulty] == ClearIcon::FullCombo){
                ui->GetSprite("clearIcon").GH = TextureManager::LoadTexture("Select/fcIcon.png");
                ui->GetSprite("clearIcon").color = Color(1.0f);

            } else{
                ui->GetSprite("clearIcon").color = Color(0.0f);
            }
        }
    }

    if(selectMode_ == SelectMode::Group or allUpdate){
        // グループ選択UIの更新
        for(int32_t i = 0; i < groupUIs.size(); i++){
            auto* ui = groupUIs[i]->GetComponent<UIComponent>();
            SongGroup* group = visibleGroups[i];

            // Spriteの更新
            ui->GetSprite("itemBG").color = uiBackColors_[(int)currentDifficulty];
            // Textの更新
            ui->GetText("genre").text = group->groupName;
        }

    }

    if(selectMode_ == SelectMode::Difficulty or allUpdate){
        // 難易度選択UIの更新
        for(int32_t i = 0; i < difficultyUIs.size(); i++){
            auto* ui = difficultyUIs[i]->GetComponent<UIComponent>();
            auto* songInfo = currentSong.first;
            int32_t difficulty = int(TrackDifficulty::Parallel) - i;

            // 画像の更新
            ui->GetSprite("itemBG").color = uiBackColors_[difficulty];
            ui->GetSprite("levelBG").color = uiBackColors_[difficulty];
            ui->GetSprite("Jacket").GH = TextureManager::LoadTexture(
                "../../Resources/NoteDatas/" + songInfo->folderName + "/" + songInfo->folderName + ".png"
            );

            // スコアを小数点4位までに変換
            std::string scoreStr = "-----------";
            std::string rankStr = "-----";
            if(songInfo->score[difficulty] > 0.0f){
                scoreStr = std::to_string(songInfo->score[difficulty]);
                scoreStr = scoreStr.substr(0, scoreStr.find(".") + 5) + "%";
                rankStr = GroupNameUtils::rankNames[(int)songInfo->ranks[difficulty]];
            }

            // テキストの更新
            ui->GetText("title").text = songInfo->songName;
            ui->GetText("artistName").text = songInfo->artistName;
            ui->GetText("score").text = scoreStr;
            ui->GetText("level").text = std::to_string(songInfo->difficulty[difficulty]);
            ui->GetText("bpm").text = "BPM" + std::to_string(static_cast<int>(songInfo->bpm));
            ui->GetText("rank").text = rankStr;

            // AP,FCアイコンの表示更新
            if(songInfo->clearIcons[difficulty] == ClearIcon::Perfect){
                ui->GetSprite("clearIcon").GH = TextureManager::LoadTexture("Select/apIcon.png");
                ui->GetSprite("clearIcon").color = Color(1.0f);

            } else if(songInfo->clearIcons[difficulty] == ClearIcon::FullCombo){
                ui->GetSprite("clearIcon").GH = TextureManager::LoadTexture("Select/fcIcon.png");
                ui->GetSprite("clearIcon").color = Color(1.0f);

            } else{
                ui->GetSprite("clearIcon").color = Color(0.0f);
            }
        }
    }
}

//////////////////////////////////////////////////////////////////////////////////
// 難易度選択画面への更新
//////////////////////////////////////////////////////////////////////////////////
void SongSelector::ToDifficultySelectUpdate(float timeScale){

    toDifficultySelectTimer_.Update(timeScale);
    float ease = toDifficultySelectTimer_.GetEase(Easing::InOutExpo);

    // UIの拡縮
    for(int i = 0; i < songUIs.size(); i++){
        if(i == centerIdx_){
            songUIs[i]->aditionalTransform_.scale = Vector2(0.0f);
            songUIs[i]->UpdateMatrix(true);
        } else{
            songUIs[i]->aditionalTransform_.translate.x = 680.0f * ease;
            songUIs[i]->UpdateMatrix(true);
        }
    }

    for(int i = 0; i < difficultyUIs.size(); i++){
        int32_t idx = int32_t(TrackDifficulty::Parallel) - i;
        if(idx == (int)currentDifficulty){
            difficultyUIs[i]->aditionalTransform_.scale = Vector2(1.0f);
            difficultyUIs[i]->localTransform_.translate = MyMath::Lerp(
                songUICenterTransform_.translate,
                difficultyUICenterTransform_.translate,
                ease
            );
            difficultyUIs[i]->UpdateMatrix(true);
        } else{
            difficultyUIs[i]->aditionalTransform_.scale = Vector2(ease);
            difficultyUIs[i]->UpdateMatrix(true);
        }
    }

    // タイマーが終点か始点に達したらフラグを更新
    if(toDifficultySelectTimer_.IsFinishedNow()){

        // 遷移フラグをオフに
        isTransitionDifficulty_Select_ = false;

        // UIのアクティブ状態を切り替え
        for(auto& ui : difficultyUIs){
            ui->SetIsActive(true, false);
        }
        for(auto& ui : songUIs){
            ui->SetIsActive(false);
        }
    } else if(toDifficultySelectTimer_.IsReturnNow()){

        // 遷移フラグをオフに
        isTransitionDifficulty_Select_ = false;

        // UIのアクティブ状態を切り替え
        for(auto& ui : difficultyUIs){
            ui->SetIsActive(false);
            ui->aditionalTransform_.scale = Vector2(0.0f);
        }
        for(auto& ui : songUIs){
            ui->SetIsActive(true, false);
            ui->aditionalTransform_.scale = Vector2(1.0f);
            ui->UpdateMatrix(true);
        }
    }
}

//////////////////////////////////////////////////////////////////////////////////
// プレイ待機画面の更新
//////////////////////////////////////////////////////////////////////////////////
void SongSelector::PlayWaitUpdate(){
    playWaitTimer_.Update();
    float ease = playWaitTimer_.GetEase(Easing::OutExpo);

    // UIのフェードアウトと中央寄せ
    for(int i = 0; i < difficultyUIs.size(); i++){
        int32_t idx = int32_t(TrackDifficulty::Parallel) - i;
        if(idx == (int)currentDifficulty){
            difficultyUIs[i]->localTransform_.translate = MyMath::Lerp(
                difficultyUICenterTransform_.translate,
                kWindowCenter,
                ease
            );
            difficultyUIs[i]->UpdateMatrix(true);
        } else{
            difficultyUIs[i]->masterColor_.value.w = 1.0f - ease;
        }
    }

    // タイマーが終了したらシーン遷移フラグを立てる
    if(playWaitTimer_.IsFinishedNow()){
        changeSceneOrder_ = true;
    }
}


//////////////////////////////////////////////////////////////////////////////////
// 選択ボタンUIの更新
//////////////////////////////////////////////////////////////////////////////////
void SongSelector::UpdateSelectButtonUIs(){

    {// 選択状況に応じてUIの大きさを切り替えるためのタイマー更新
        if(selectMode_ == SelectMode::Group){
            buttonUIScalingTimer_.Update();
        }
        if(selectMode_ == SelectMode::Song){
            buttonUIScalingTimer_.Update();
        }
        if(selectMode_ == SelectMode::Difficulty){
            if(!isPlayWaiting_){
                buttonUIScalingTimer_.Update(-1.0f);
            } else{
                buttonUIScalingTimer_.Update();
            }
        }
    }

    // ボタンの入力に反応して一時的に大きくする用のタイマー更新
    UpDown inputDir = verticalInput_.Value();
    {
        if(inputDir == UpDown::UP){
            buttonUIPressScaleTimer_Up_.Reset();

        } else if(inputDir == UpDown::DOWN){
            buttonUIPressScaleTimer_Down_.Reset();
        }
        buttonUIPressScaleTimer_Up_.Update();
        buttonUIPressScaleTimer_Down_.Update();
    }

    LR modeChangeDir = modeChangeInput_.Value();
    {
        if(modeChangeDir == LR::LEFT){
            buttonUIPressScaleTimer_Q_.Reset();

        } else if(modeChangeDir == LR::RIGHT){
            buttonUIPressScaleTimer_E_.Reset();
        }
        buttonUIPressScaleTimer_Q_.Update();
        buttonUIPressScaleTimer_E_.Update();
    }


    float ease = buttonUIScalingTimer_.GetEase(Easing::InOutExpo);
    float sinValue = std::sinf((3.14f / 1.5f) * songSelectButtonUI_->GetAliveTime());
    float waveRadius = 5.0f;

    for(int32_t i = 0; i < 2; i++){
        {
            // 必要な変数の取得・計算
            auto* songChild = songSelectButtonUI_->GetChild(i);
            auto* difficultyChild = difficultySelectButtonUI_->GetChild(i);
            Timer* buttonUIPressScaleTimer = (i == 0) ? &buttonUIPressScaleTimer_Up_ : &buttonUIPressScaleTimer_Down_;
            float baseScale = 1.0f + 0.2f * (1.0f - buttonUIPressScaleTimer->GetProgress());

            // 上下ボタンUIの拡縮
            songChild->aditionalTransform_.scale = Vector2(baseScale * ease);
            difficultyChild->aditionalTransform_.scale = Vector2(baseScale * (1.0f - ease));

            // 上下に動かす
            songChild->aditionalTransform_.translate.y = waveRadius * sinValue * (i == 0 ? -1.0f : 1.0f);
            difficultyChild->aditionalTransform_.translate.y = waveRadius * sinValue * (i == 0 ? -1.0f : 1.0f);

            // 端で表示しないようにする処理
            if(currentDifficulty == TrackDifficulty::Basic){
                if(i == 1){
                    difficultyChild->aditionalTransform_.scale = Vector2(0.0f);
                }
            } else if(currentDifficulty == TrackDifficulty::Parallel){
                if(i == 0){
                    difficultyChild->aditionalTransform_.scale = Vector2(0.0f);
                }
            }

            // 行列の更新
            songChild->UpdateMatrix();
            difficultyChild->UpdateMatrix();
        }

        {// QEボタンUIの更新
            auto* modeChangeUIChild = modeChangeButtonUI_->GetChild(i);
            Timer* modeChangeButtonScaleTimer = (i == 0) ? &buttonUIPressScaleTimer_Q_ : &buttonUIPressScaleTimer_E_;
            float baseScale = 1.0f + 0.2f * (1.0f - modeChangeButtonScaleTimer->GetProgress());

            // 上下ボタンUIの拡縮
            modeChangeUIChild->aditionalTransform_.scale = Vector2(baseScale * ease);

            // 上下に動かす
            modeChangeUIChild->aditionalTransform_.translate.x = waveRadius * sinValue * (i == 0 ? -1.0f : 1.0f);

            // 行列の更新
            modeChangeUIChild->UpdateMatrix();
        }
    }


    // 表示内容の更新
    if(GameSystem::GetScene()->HasEvent()){
        backButtonUI_->GetComponent<UIComponent>()->GetText(0).text = "閉じる";
        backButtonUI_->GetComponent<UIComponent>()->GetText(0).isStaticDraw = true;
        backButtonUI_->GetComponent<UIComponent>()->GetSprite(0).isStaticDraw = true;

    } else{
        if(selectMode_ == SelectMode::Group){
            backButtonUI_->GetComponent<UIComponent>()->GetText(0).text = "メニュー";
            modeChangeButtonUI_->SetIsActive(true);
            auto& text = modeChangeButtonUI_->GetChild("modeText")->GetComponent<UIComponent>()->GetText(0);
            text.text = ModeUtil::groupModeNames[(int)currentGroupMode];

        } else if(selectMode_ == SelectMode::Song){
            backButtonUI_->GetComponent<UIComponent>()->GetText(0).text = "もどる";
            modeChangeButtonUI_->SetIsActive(true);
            auto& text = modeChangeButtonUI_->GetChild("modeText")->GetComponent<UIComponent>()->GetText(0);
            text.text = ModeUtil::sortModeNames[(int)currentSortMode];

        } else{
            modeChangeButtonUI_->SetIsActive(false);
        }
    }


    // プレイ待機中は選択ボタンを非アクティブにする
    if(isPlayWaiting_){
        songSelectButtonUI_->SetIsActive(false);
    }
}

//////////////////////////////////////////////////////////////////////////////////
// 背景描画情報の更新
//////////////////////////////////////////////////////////////////////////////////
void SongSelector::UpdateBGDrawerInfo(){
    SelectBackGroundDrawer::currentDifficulty = currentDifficulty;

    // 入力があった際にグルーブさせる
    if(verticalInput_.Trigger()){
        SelectBackGroundDrawer::isGrooveStart_ = true;
    
    } else if(holozontalInput_.Trigger()){
        SelectBackGroundDrawer::isGrooveStart_ = true;

    } else if(modeChangeInput_.Trigger()){
        SelectBackGroundDrawer::isGrooveStart_ = true;
    
    } else if(decideInput_.Trigger()){
        SelectBackGroundDrawer::isGrooveStart_ = true;
    
    } else if(backInput_.Trigger()){
        SelectBackGroundDrawer::isGrooveStart_ = true;
    }
}

//////////////////////////////////////////////////////////////////////////////////
// 編集
//////////////////////////////////////////////////////////////////////////////////
void SongSelector::Edit(){
#ifdef _DEBUG

    ImFunc::CustomBegin("SongSelector", MoveOnly_TitleBar);
    {
        // 色の編集
        for(int i = 0; i < uiBackColors_.size(); i++){
            std::string label = "UI Back Color " + std::to_string(i);
            ImGui::ColorEdit4(label.c_str(), &uiBackColors_[i].value.x);
        }

        ImGui::Separator();//--------------------------------
        // カメラ補間時間の編集
        for(int i = 0; i < cameraInterpolationTimes_.size(); i++){
            std::string label = "カメラ補間時間" + std::to_string(i);
            ImGui::DragFloat(label.c_str(), &cameraInterpolationTimes_[i], 0.1f, 0.1f, 10.0f);
        }

        ImGui::Separator();//--------------------------------

        // 現在の各種モード
        ImGui::Text("現在のソート方法: %s", ModeUtil::sortModeNames[(int)currentSortMode].c_str());
        ImGui::Text("現在のグループ方法: %s", ModeUtil::groupModeNames[(int)currentGroupMode].c_str());

        ImGui::Separator();//--------------------------------

        // indexの表示
        ImGui::Text("現在のグループIndex: %d", currentGroupIndex);
        ImGui::Text("現在の楽曲Index: %d", currentSongIndex);

        ImGui::End();
    }

#endif // _DEBUG
}

//////////////////////////////////////////////////////////////////////////////////
// JSONへ書き出し
//////////////////////////////////////////////////////////////////////////////////
void SongSelector::ToJson(){
    nlohmann::json j;
    // 色の書き出し
    for(int i = 0; i < uiBackColors_.size(); i++){
        j["uiBackColors"][i] = uiBackColors_[i];
    }

    // カメラ補間時間の書き出し
    for(int i = 0; i < cameraInterpolationTimes_.size(); i++){
        j["cameraMoveDurations"][i] = cameraInterpolationTimes_[i];
    }

    // indexの書き出し
    j["currentGroupIndex"] = currentGroupIndex;
    j["currentSongIndex"] = currentSongIndex;

    // modeの書き出し
    j["currentSortMode"] = (int)currentSortMode;
    j["currentGroupMode"] = (int)currentGroupMode;
    selectMode_ == SelectMode::Difficulty ? selectMode_ = SelectMode::Song : selectMode_;
    j["selectMode"] = (int)selectMode_;

    // 難易度の書き出し
    j["currentDifficulty"] = (int)currentDifficulty;

    // JSONの保存
    MyFunc::CreateJsonFile("Resources/Jsons/Settings/song_selector.json", j);
}

//////////////////////////////////////////////////////////////////////////////////
// JSONから読み込み
//////////////////////////////////////////////////////////////////////////////////
void SongSelector::FromJson(){
    nlohmann::json j = MyFunc::GetJson("Resources/Jsons/Settings/song_selector.json");
    if(!j.empty()){

        // 色の読み込み
        if(j.contains("uiBackColors")){
            for(int i = 0; i < uiBackColors_.size(); i++){
                uiBackColors_[i] = j["uiBackColors"][i];
            }
        }

        // カメラ補間時間の読み込み
        if(j.contains("cameraMoveDurations")){
            for(int i = 0; i < cameraInterpolationTimes_.size(); i++){
                cameraInterpolationTimes_[i] = j["cameraMoveDurations"][i];
            }
        }

        // indexの読み込み
        currentGroupIndex = j.value("currentGroupIndex", 0);
        currentSongIndex = j.value("currentSongIndex", 0);

        // modeの読み込み
        currentSortMode = (SortMode)j.value("currentSortMode", 0);
        currentGroupMode = (GroupMode)j.value("currentGroupMode", 0);
        if(!isFirstPlay_){
            selectMode_ = (SelectMode)j.value("selectMode", 0);
        } else{
            selectMode_ = SelectMode::Group;
            isFirstPlay_ = false;
        }

        // 難易度の読み込み
        currentDifficulty = (TrackDifficulty)j.value("currentDifficulty", 0);
    }
}
