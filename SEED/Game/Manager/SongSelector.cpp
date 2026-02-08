#include "SongSelector.h"
#include <SEED/Source/SEED.h>
#include <SEED/Source/Manager/ImGuiManager/ImGuiManager.h>
#include <SEED/Lib/Structs/Transform.h>
#include <Game/GameSystem.h>
#include <Game/Scene/Scene_Game/State/GameState_Play.h>
#include <Game/Scene/Scene_Clear/Scene_Clear.h>
#include <Game/Objects/SongSelect/SelectBackGroundDrawer.h>
#include <SEED/Source/Basic/SceneTransition/HexagonTransition.h>

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
    auto songFolders = SEED::Methods::File::GetFolderList("Resources/NoteDatas", true, false);

    // フォルダ名を元に楽曲情報を構築
    for(const auto& folderName : songFolders){
        auto& data = songList.emplace_back(new SongInfo());
        data->Initialize(folderName.generic_string());
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
    sceneOutTimer_.Initialize(1.0f, 0.0f, true);
    songVolumeFadeInTimer_.Initialize(0.2f);
    songVolumeFadeOutTimer_.Initialize(1.0f);

    // 初期位置に移動
    ShiftItem(true);

    // UI内容の更新
    UpdateUIContents(true);

    // ジャケット矩形の初期化
    jacket3D_ = SEED::Methods::Shape::MakeEqualQuad(3.0f);
    jacket3D_.cullMode = D3D12_CULL_MODE_NONE; // カリングなし
    jacket3D_.lightingType = SEED::LIGHTINGTYPE_NONE; // ライティングなし
    jacket3D_.GH = SEED::TextureManager::LoadTexture(
        "../../Resources/NoteDatas/" + currentSong.first->folderName + "/" + currentSong.first->folderName + ".png"
    );

    // カメラコントロールポイントの読み込み
    auto* hierarchy = SEED::GameSystem::GetScene()->GetHierarchy();
    cameraControlPts_ = hierarchy->LoadObject("SelectScene/cameraControlPts.prefab");
    auto& controlPoints = cameraControlPts_->GetComponent<SEED::Routine3DComponent>()->GetControlPoints();
    if(!controlPoints.empty()){
        preCameraTransform_ = controlPoints[0].first;
    }

    // カーソル当たり判定オブジェクトの取得
    pMouseCursor_ = hierarchy->GetGameObject2D("cursorColliderObj");

    // カメラの取得
    SEED::Instance::RemoveCamera("gameCamera");
    SEED::Instance::SetMainCamera("default");
    camera_ = SEED::Instance::GetMainCamera();

    // パーティクルを初期化しエミッターを読み込む
    SEED::ParticleManager::DeleteAll();// 既存のエフェクトを削除
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

            auto* scene = SEED::GameSystem::GetScene();
            SEED::GameSystem::GetScene()->ChangeState(new GameState_Play(scene, *currentSong.first, (int)currentDifficulty));
            return;
        }
    }
}

////////////////////////////////////////////////////////////////////
// 更新処理
////////////////////////////////////////////////////////////////////
void SongSelector::Update(){

    if(!isPlayWaiting_){
        // マウス入力の確認
        CheckMouseInput();

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

    // マスターボリューム調整
    MasterVolumeAdjust();


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
    SEED::Instance::DrawQuad(jacket3D_);
}

///////////////////////////////////////////////////////////////
// 入力関数初期化
///////////////////////////////////////////////////////////////
void SongSelector::InitializeInput(){

    // 縦方向入力-----------------------------------------------------
    verticalInput_.Value = [&]{
        if(SEED::GameSystem::GetScene()->HasEvent()){
            return SEED::GeneralEnum::UpDown::NONE;// イベント中は入力を受け付けない
        }
        if(SEED::Input::IsPressKey({ DIK_W,DIK_UP })){
            return SEED::GeneralEnum::UpDown::UP;
        }
        if(SEED::Input::IsPressKey({ DIK_S,DIK_DOWN })){
            return SEED::GeneralEnum::UpDown::DOWN;
        }
        if(mouseInputVertical_ != SEED::GeneralEnum::UpDown::NONE){
            return mouseInputVertical_;
        }
        return SEED::GeneralEnum::UpDown::NONE;
    };
    verticalInput_.Trigger = [&]{
        if(SEED::GameSystem::GetScene()->HasEvent()){
            return false;// イベント中は入力を受け付けない
        }

        // キーボードでの方向入力
        if(SEED::Input::IsTriggerKey({ DIK_W,DIK_UP,DIK_S,DIK_DOWN })){
            return true;
        }

        // マウスでの方向入力
        if(mouseInputVertical_ != SEED::GeneralEnum::UpDown::NONE){
            return true;
        }

        return false;
    };

    // ソート変更入力-------------------------------------------------
    modeChangeInput_.Value = [&]{
        if(SEED::GameSystem::GetScene()->HasEvent()){
            return SEED::GeneralEnum::LR::NONE;// イベント中は入力を受け付けない
        }
        if(SEED::Input::IsPressKey(DIK_Q)){
            return SEED::GeneralEnum::LR::LEFT;
        }
        if(SEED::Input::IsPressKey(DIK_E)){
            return SEED::GeneralEnum::LR::RIGHT;
        }
        if(mouseInputModeChange_ != SEED::GeneralEnum::LR::NONE){
            return mouseInputModeChange_;
        }
        return SEED::GeneralEnum::LR::NONE;
    };
    modeChangeInput_.Trigger = [&]{
        if(SEED::GameSystem::GetScene()->HasEvent()){
            return false;// イベント中は入力を受け付けない
        }
        if(SEED::Input::IsTriggerKey({ DIK_Q,DIK_E })){
            return true;
        }
        if(mouseInputModeChange_ != SEED::GeneralEnum::LR::NONE){
            return true;
        }
        return false;
    };

    // 難易度変更入力-------------------------------------------------
    difficultyChangeInput_.Value = []{
        if(SEED::GameSystem::GetScene()->HasEvent()){
            return SEED::GeneralEnum::LR::NONE;// イベント中は入力を受け付けない
        }
        if(SEED::Input::IsPressKey({ DIK_A,DIK_LEFT })){
            return SEED::GeneralEnum::LR::LEFT;
        }
        if(SEED::Input::IsPressKey({ DIK_D,DIK_RIGHT })){
            return SEED::GeneralEnum::LR::RIGHT;
        }
        return SEED::GeneralEnum::LR::NONE;
    };
    difficultyChangeInput_.Trigger = []{
        if(SEED::GameSystem::GetScene()->HasEvent()){
            return false;// イベント中は入力を受け付けない
        }
        if(SEED::Input::IsTriggerKey({ DIK_A,DIK_LEFT,DIK_D,DIK_RIGHT })){
            return true;
        }
        return false;
    };

    // 決定入力-------------------------------------------------------
    decideInput_.Trigger = [&]{
        if(SEED::GameSystem::GetScene()->HasEvent()){
            return false;// イベント中は入力を受け付けない
        }
        if(SEED::Input::IsTriggerKey({ DIK_SPACE,DIK_RETURN })){
            return true;
        }
        if(mouseDecideInput_){
            return true;
        }
        return false;
    };

    // 戻る入力-------------------------------------------------------
    backInput_.Trigger = [&]{
        if(SEED::GameSystem::GetScene()->HasEvent()){
            return false;// イベント中は入力を受け付けない
        }
        if(SEED::Input::IsTriggerKey(DIK_ESCAPE)){
            return true;
        }
        if(mouseBackInput_){
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
    auto* hierarchy = SEED::GameSystem::GetScene()->GetHierarchy();
    songControlPts_ = hierarchy->LoadObject2D("SelectScene/songControlPts.prefab");
    difficultyControlPts_ = hierarchy->LoadObject2D("SelectScene/difficultyControlPts.prefab");
    songControlPts_->SetIsActive(false);
    difficultyControlPts_->SetIsActive(false);

    // UIアイテムの数の初期化
    size_t size = songControlPts_->GetComponent<SEED::Routine2DComponent>()->GetControlPoints().size() - 2;
    visibleSongs.resize(size);
    visibleGroups.resize(size);
    songUIs.resize(size);
    groupUIs.resize(size);

    // centerのTransform2Dを取得
    int centerIdx = (int)(size / 2);
    songUICenterTransform_ = songControlPts_->GetComponent<SEED::Routine2DComponent>()->GetControlPoint(centerIdx + 1);
    centerIdx = (int)difficultyControlPts_->GetComponent<SEED::Routine2DComponent>()->GetControlPoints().size() / 2;
    difficultyUICenterTransform_ = difficultyControlPts_->GetComponent<SEED::Routine2DComponent>()->GetControlPoint(centerIdx);

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
            // グループ名からレベルを抽出して比較
            int32_t aLevel = std::stoi(a.groupName.substr(2));
            int32_t bLevel = std::stoi(b.groupName.substr(2));
            return aLevel < bLevel;
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
                return SEED::Methods::String::CompareStr(a.first->songName, b.first->songName);
            });
            break;

        case SortMode::Artist:
            // アーティストでソート
            std::sort(
                songs.begin(), songs.end(),
                [](const Track& a, const Track& b){
                return SEED::Methods::String::CompareStr(a.first->artistName, b.first->artistName);
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

    if(currentSong.first->audioFilePath != preSongName_){
        // 曲名が変わったら再生する曲を切り替える
        if(isPlayAudio){

            // タイマー・フラグの初期化
            isSelectBGMPlaying_ = false;
            songVolumeFadeInTimer_.Reset();
            songVolumeFadeOutTimer_.Reset();

            // 今の曲を停止して再生
            preSongName_ = currentSong.first->audioFilePath;
            SEED::AudioManager::EndAudio(songHandle_);
            songHandle_ = SEED::AudioManager::PlayAudio(
                currentSong.first->audioFilePath,
                true,
                0.0f,
                currentSong.first->songPreviewRange.min
            );

        } else{
            // タイマー・フラグの初期化
            isSelectBGMPlaying_ = true;
            songVolumeFadeInTimer_.Reset();
            songVolumeFadeOutTimer_.Reset();

            // 今の曲を停止して再生
            SEED::AudioManager::EndAudio(songHandle_);
            songHandle_ = SEED::AudioManager::PlayAudio(
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
            int32_t index = SEED::Methods::Math::Spiral(currentSongIndex + dif, 0, int32_t(currentGroup->groupMembers.size()) - 1);
            visibleSongs[i] = &currentGroup->groupMembers[index];
        }

        // 前方の要素を設定
        for(int i = centerIdx_; i >= 0; i--){
            int32_t dif = i - centerIdx_;
            int32_t index = SEED::Methods::Math::Spiral(currentSongIndex + dif, 0, int32_t(currentGroup->groupMembers.size()) - 1);
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
            int32_t index = SEED::Methods::Math::Spiral(currentGroupIndex + dif, 0, int32_t(songGroups.size()) - 1);
            visibleGroups[i] = &songGroups[index];
        }

        // 前方の要素を設定
        for(int i = centerIdx_; i >= 0; i--){
            int32_t dif = i - centerIdx_;
            int32_t index = SEED::Methods::Math::Spiral(currentGroupIndex + dif, 0, int32_t(songGroups.size()) - 1);
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
    SEED::GeneralEnum::UpDown verticalValue = verticalInput_.Value();
    SEED::GeneralEnum::LR modeChangeValue = modeChangeInput_.Value();
    SEED::GeneralEnum::LR difficultyChangeValue = difficultyChangeInput_.Value();

    // 縦方向入力がある場合
    if(verticalValue != SEED::GeneralEnum::UpDown::NONE){
        inputTimer_.Update(1.0f, true);

        // タイマーがループしたらインデックスを更新
        if(inputTimer_.IsLoopedNow()){
            // 可視曲の更新
            currentSongIndex += (verticalValue == SEED::GeneralEnum::UpDown::UP) ? -1 : 1;
            currentSongIndex = SEED::Methods::Math::Spiral(currentSongIndex, 0, int32_t(currentGroup->groupMembers.size()) - 1);
            isShiftItem_ = true;
            shiftDirection_ = verticalValue;
            UpdateVisibleSongs();
            itemShiftTimer_.Reset();

            // 音声再生
            SEED::AudioManager::PlayAudio(AudioDictionary::Get("ItemSelect"), false, 0.5f);

            return;
        }

    } else{
        // 入力がない場合はタイマーを最大値に固定(次押した際にすぐ動くように)
        inputTimer_.currentTime = inputTimer_.duration;
    }

    if(!isShiftItem_){
        // ソートモードの変更
        if(modeChangeValue != SEED::GeneralEnum::LR::NONE && modeChangeInput_.Trigger()){
            int sortModeInt = (int)currentSortMode;
            sortModeInt += (modeChangeValue == SEED::GeneralEnum::LR::LEFT) ? -1 : 1;
            sortModeInt = SEED::Methods::Math::Spiral(sortModeInt, 0, (int)SortMode::kMaxCount - 1);
            currentSortMode = (SortMode)sortModeInt;

            // ソート処理を実行
            Sort();
            // 項目の更新
            UpdateVisibleSongs();
            UpdateUIContents();

            // 音声再生
            SEED::AudioManager::PlayAudio(AudioDictionary::Get("ChangeMode"), false, 0.5f);

            return;
        }

        // 難易度の変更
        if(difficultyChangeInput_.Value() != SEED::GeneralEnum::LR::NONE && difficultyChangeInput_.Trigger()){
            int difficultyInt = (int)currentDifficulty;
            difficultyInt += (difficultyChangeValue == SEED::GeneralEnum::LR::LEFT) ? -1 : 1;
            difficultyInt = std::clamp(difficultyInt, 0, (int)TrackDifficulty::kMaxDifficulty - 1);

            // 難易度が変わっていたら更新
            if(difficultyInt != (int32_t)currentDifficulty){
                currentDifficulty = (TrackDifficulty)difficultyInt;
                // 現在の曲情報を更新
                currentSong = { currentSong.first,currentDifficulty };

                // ソート処理を実行
                Sort();
                // 項目の更新
                UpdateVisibleSongs();
                UpdateUIContents();

                // 音声再生
                SEED::AudioManager::PlayAudio(AudioDictionary::Get("ChangeMode"), false, 0.5f);
                return;
            }
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

            // 音声再生
            SEED::AudioManager::PlayAudio(AudioDictionary::Get("DecideSong"), false, 0.5f);

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


            // タイマー・フラグの初期化
            isSelectBGMPlaying_ = true;
            songVolumeFadeInTimer_.Reset();
            songVolumeFadeOutTimer_.Reset();

            // グループ選択時はデフォルト選曲BGMを再生
            preSongName_.clear();
            SEED::AudioManager::EndAudio(songHandle_);
            songHandle_ = SEED::AudioManager::PlayAudio(
                AudioDictionary::Get("SelectBGM"),
                true, 0.5f
            );

            // 音声再生
            SEED::AudioManager::PlayAudio(AudioDictionary::Get("Return"), false, 0.5f);

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
    SEED::GeneralEnum::UpDown verticalValue = verticalInput_.Value();
    SEED::GeneralEnum::LR modeChangeValue = modeChangeInput_.Value();

    // 縦方向入力がある場合
    if(verticalValue != SEED::GeneralEnum::UpDown::NONE){
        inputTimer_.Update(1.0f, true);

        // タイマーがループしたらインデックスを更新
        if(inputTimer_.IsLoopedNow()){
            currentGroupIndex += (verticalValue == SEED::GeneralEnum::UpDown::UP) ? -1 : 1;
            currentGroupIndex = SEED::Methods::Math::Spiral(currentGroupIndex, 0, int32_t(songGroups.size()) - 1);
            isShiftItem_ = true;
            shiftDirection_ = verticalValue;
            itemShiftTimer_.Reset();
            UpdateVisibleGroups();

            // 音声再生
            SEED::AudioManager::PlayAudio(AudioDictionary::Get("ItemSelect"), false, 0.5f);

            return;
        }

    } else{
        // 入力がない場合はタイマーを最大値に固定(次押した際にすぐ動くように)
        inputTimer_.currentTime = inputTimer_.duration;
    }

    // グループモードの変更
    if(modeChangeValue != SEED::GeneralEnum::LR::NONE && modeChangeInput_.Trigger()){
        int groupModeInt = (int)currentGroupMode;
        groupModeInt += (modeChangeValue == SEED::GeneralEnum::LR::LEFT) ? -1 : 1;
        groupModeInt = SEED::Methods::Math::Spiral(groupModeInt, 0, (int)GroupMode::kMaxCount - 1);
        currentGroupMode = (GroupMode)groupModeInt;
        // ソート処理を実行
        Sort();
        // 項目の更新
        UpdateVisibleGroups();
        UpdateUIContents();

        // 音声再生
        SEED::AudioManager::PlayAudio(AudioDictionary::Get("ChangeMode"), false, 0.5f);

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

            // 音声再生
            SEED::AudioManager::PlayAudio(AudioDictionary::Get("DecideSong"), false, 0.5f);

            return;
        }

        // escでメニュー表示
        if(backInput_.Trigger()){
            auto* scene = SEED::GameSystem::GetScene();
            scene->CauseEvent(new GameState_SelectMenu(scene));

            // 音声再生
            SEED::AudioManager::PlayAudio(AudioDictionary::Get("OpenSelectMenu"), false, 0.5f);

            return;
        }
    }
}

/////////////////////////////////////////////////////////////////////////////////////
// 難易度選択処理
/////////////////////////////////////////////////////////////////////////////////////
void SongSelector::SelectDifficulty(){

    // 各種入力値の取得
    SEED::GeneralEnum::UpDown verticalValue = verticalInput_.Value();

    // 縦方向入力がある場合
    if(verticalValue != SEED::GeneralEnum::UpDown::NONE){
        inputTimer_.Update(1.0f, true);

        // タイマーがループしたらインデックスを更新
        if(inputTimer_.IsLoopedNow()){
            // 難易度の更新
            TrackDifficulty previousDifficulty = currentDifficulty;
            int nextDiffInt = int(currentDifficulty) + ((verticalValue == SEED::GeneralEnum::UpDown::UP) ? 1 : -1);
            nextDiffInt = std::clamp(nextDiffInt, 0, (int)TrackDifficulty::kMaxDifficulty - 1);
            currentDifficulty = (TrackDifficulty)nextDiffInt;

            // 難易度が変わっていたらフラグを立てる
            if(currentDifficulty != previousDifficulty){
                isShiftItem_ = true;
                shiftDirection_ = verticalValue;
                itemShiftTimer_.Reset();

                // currentSongを現在の難易度に合わせて更新
                currentSong = { currentSong.first,currentDifficulty };

                // 音声再生
                SEED::AudioManager::PlayAudio(AudioDictionary::Get("ItemSelect"), false, 0.5f);
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

            // 音声再生
            SEED::AudioManager::PlayAudio(AudioDictionary::Get("ToPlay"), false, 0.5f);

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

            // 音声再生
            SEED::AudioManager::PlayAudio(AudioDictionary::Get("Return"), false, 0.5f);

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
    float ease = itemShiftTimer_.GetEase(SEED::Methods::Easing::Type::Out_Sine);

    // アイテムの移動処理
    if(selectMode_ == SelectMode::Song or allUpdate){
        // コントロールポイントの取得
        std::vector<RoutinePoint2D> controlPts = songControlPts_->GetComponent<SEED::Routine2DComponent>()->GetControlPoints();

        // 動かす方向の取得
        int32_t dir = 0;
        if(shiftDirection_ != SEED::GeneralEnum::UpDown::NONE){
            dir = (shiftDirection_ == SEED::GeneralEnum::UpDown::UP) ? -1 : 1;
        }

        // アイテム移動処理
        for(int32_t i = 0; i < songUIs.size(); i++){
            // 終点と始点要素を求める
            int32_t aimIdx = i + 1;
            int32_t originIdx = aimIdx + dir;

            // 補完して行列を更新
            songUIs[i]->localTransform_ = SEED::Methods::SRT::Interpolate(
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
        std::vector<RoutinePoint2D> controlPts = songControlPts_->GetComponent<SEED::Routine2DComponent>()->GetControlPoints();

        // 動かす方向の取得
        int32_t dir = 0;
        if(shiftDirection_ != SEED::GeneralEnum::UpDown::NONE){
            dir = (shiftDirection_ == SEED::GeneralEnum::UpDown::UP) ? -1 : 1;
        }

        // アイテム移動処理
        for(int32_t i = 0; i < groupUIs.size(); i++){
            // 終点と始点要素を求める
            int32_t aimIdx = i + 1;
            int32_t originIdx = aimIdx + dir;

            // 補完して行列を更新
            groupUIs[i]->localTransform_ = SEED::Methods::SRT::Interpolate(
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
        std::vector<RoutinePoint2D> controlPts = difficultyControlPts_->GetComponent<SEED::Routine2DComponent>()->GetControlPoints();

        // 動かす方向の取得
        int32_t dir = 0;
        if(shiftDirection_ != SEED::GeneralEnum::UpDown::NONE){
            dir = (shiftDirection_ == SEED::GeneralEnum::UpDown::UP) ? 1 : -1;
        }

        // アイテム移動処理
        for(int32_t i = 0; i < difficultyUIs.size(); i++){
            // 終点と始点要素を求める
            int32_t originIdx = int32_t(currentDifficulty) - dir + i;
            int32_t aimIdx = originIdx + dir;
            // 補完して行列を更新
            difficultyUIs[i]->localTransform_ = SEED::Methods::SRT::Interpolate(
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
        shiftDirection_ = SEED::GeneralEnum::UpDown::NONE;
    }
}


/////////////////////////////////////////////////////////////////////////////////////
// カメラの制御
/////////////////////////////////////////////////////////////////////////////////////
void SongSelector::CameraControl(){

    // 制御点オブジェクトが存在しない場合は抜ける
    if(!cameraControlPts_){ return; }

    // 制御点一覧の取得
    auto& controlPoints = cameraControlPts_->GetComponent<SEED::Routine3DComponent>()->GetControlPoints();
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
    float ease = cameraMoveTimer_.GetEase(SEED::Methods::Easing::Type::InOut_Expo);

    // カメラの位置・注視点を補完して更新
    camera_->SetTransform(
        SEED::Methods::SRT::Interpolate(
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
        jacket3D_.rotate.y += rotateSpeed * SEED::ClockManager::DeltaTime();

        // 上下に揺らす
        float swingHeight = 0.1f; // 揺らす高さ
        float t = std::sin(SEED::ClockManager::TotalTime() * 2.0f) * swingHeight; // サイン波で上下に揺らす
        jacket3D_.translate.y = t; // ジャケットのY座標を更新

        // ジャケットのテクスチャを更新
        jacket3D_.GH = SEED::TextureManager::LoadTexture(
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

            auto* ui = songUIs[i]->GetComponent<SEED::UIComponent>();
            int32_t difficulty = (int)visibleSongs[i]->second;
            SongInfo* songInfo = visibleSongs[i]->first;

            // 画像の更新
            ui->GetSprite("itemBG").color = uiBackColors_[(int)difficulty];
            ui->GetSprite("levelBG").color = uiBackColors_[(int)difficulty];
            ui->GetSprite("Jacket").GH = SEED::TextureManager::LoadTexture(
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
                ui->GetSprite("clearIcon").GH = SEED::TextureManager::LoadTexture("Select/apIcon.png");
                ui->GetSprite("clearIcon").color = SEED::Color(1.0f);

            } else if(songInfo->clearIcons[difficulty] == ClearIcon::FullCombo){
                ui->GetSprite("clearIcon").GH = SEED::TextureManager::LoadTexture("Select/fcIcon.png");
                ui->GetSprite("clearIcon").color = SEED::Color(1.0f);

            } else{
                ui->GetSprite("clearIcon").color = SEED::Color(0.0f);
            }
        }
    }

    if(selectMode_ == SelectMode::Group or allUpdate){
        // グループ選択UIの更新
        for(int32_t i = 0; i < groupUIs.size(); i++){
            auto* ui = groupUIs[i]->GetComponent<SEED::UIComponent>();
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
            auto* ui = difficultyUIs[i]->GetComponent<SEED::UIComponent>();
            auto* songInfo = currentSong.first;
            int32_t difficulty = int(TrackDifficulty::Parallel) - i;

            // 画像の更新
            ui->GetSprite("itemBG").color = uiBackColors_[difficulty];
            ui->GetSprite("levelBG").color = uiBackColors_[difficulty];
            ui->GetSprite("Jacket").GH = SEED::TextureManager::LoadTexture(
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
                ui->GetSprite("clearIcon").GH = SEED::TextureManager::LoadTexture("Select/apIcon.png");
                ui->GetSprite("clearIcon").color = SEED::Color(1.0f);

            } else if(songInfo->clearIcons[difficulty] == ClearIcon::FullCombo){
                ui->GetSprite("clearIcon").GH = SEED::TextureManager::LoadTexture("Select/fcIcon.png");
                ui->GetSprite("clearIcon").color = SEED::Color(1.0f);

            } else{
                ui->GetSprite("clearIcon").color = SEED::Color(0.0f);
            }
        }
    }
}

//////////////////////////////////////////////////////////////////////////////////
// 難易度選択画面への更新
//////////////////////////////////////////////////////////////////////////////////
void SongSelector::ToDifficultySelectUpdate(float timeScale){

    toDifficultySelectTimer_.Update(timeScale);
    float ease = toDifficultySelectTimer_.GetEase(SEED::Methods::Easing::Type::InOut_Expo);

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
            difficultyUIs[i]->localTransform_.translate = SEED::Methods::Math::Lerp(
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
    float ease = playWaitTimer_.GetEase(SEED::Methods::Easing::Type::Out_Expo);

    // UIのフェードアウトと中央寄せ
    for(int i = 0; i < difficultyUIs.size(); i++){
        int32_t idx = int32_t(TrackDifficulty::Parallel) - i;
        if(idx == (int)currentDifficulty){
            difficultyUIs[i]->localTransform_.translate = SEED::Methods::Math::Lerp(
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
        // トランジション演出開始
        sceneOutTimer_.Restart();
        auto* transition = SEED::SceneTransitionDrawer::AddTransition<SEED::HexagonTransition>();
        transition->SetHexagonInfo(32.0f);
        transition->StartTransition(sceneOutTimer_.duration - SEED::ClockManager::DeltaTime(), 1.0f);
    }

    // 遷移タイマー
    sceneOutTimer_.Update();
    if(sceneOutTimer_.IsFinished()){
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
    SEED::GeneralEnum::UpDown inputDir = verticalInput_.Value();
    {
        if(inputDir == SEED::GeneralEnum::UpDown::UP){
            buttonUIPressScaleTimer_Up_.Reset();

        } else if(inputDir == SEED::GeneralEnum::UpDown::DOWN){
            buttonUIPressScaleTimer_Down_.Reset();
        }
        buttonUIPressScaleTimer_Up_.Update();
        buttonUIPressScaleTimer_Down_.Update();
    }

    SEED::GeneralEnum::LR modeChangeDir = modeChangeInput_.Value();
    {
        if(modeChangeDir == SEED::GeneralEnum::LR::LEFT){
            buttonUIPressScaleTimer_Q_.Reset();

        } else if(modeChangeDir == SEED::GeneralEnum::LR::RIGHT){
            buttonUIPressScaleTimer_E_.Reset();
        }
        buttonUIPressScaleTimer_Q_.Update();
        buttonUIPressScaleTimer_E_.Update();
    }


    float ease = buttonUIScalingTimer_.GetEase(SEED::Methods::Easing::Type::InOut_Expo);
    float sinValue = std::sinf((3.14f / 1.5f) * songSelectButtonUI_->GetAliveTime());
    float waveRadius = 5.0f;

    for(int32_t i = 0; i < 2; i++){
        {
            // 必要な変数の取得・計算
            auto* songChild = songSelectButtonUI_->GetChild(i);
            auto* difficultyChild = difficultySelectButtonUI_->GetChild(i);
            SEED::Timer* buttonUIPressScaleTimer = (i == 0) ? &buttonUIPressScaleTimer_Up_ : &buttonUIPressScaleTimer_Down_;
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
            SEED::Timer* modeChangeButtonScaleTimer = (i == 0) ? &buttonUIPressScaleTimer_Q_ : &buttonUIPressScaleTimer_E_;
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
    if(SEED::GameSystem::GetScene()->HasEvent()){
        backButtonUI_->GetComponent<SEED::UIComponent>()->GetText(0).text = "閉じる";
        backButtonUI_->GetComponent<SEED::UIComponent>()->GetText(0).isStaticDraw = true;
        backButtonUI_->GetComponent<SEED::UIComponent>()->GetSprite(0).isStaticDraw = true;

    } else{
        if(selectMode_ == SelectMode::Group){
            backButtonUI_->GetComponent<SEED::UIComponent>()->GetText(0).text = "プレイ設定/メニュー";
            modeChangeButtonUI_->SetIsActive(true);
            auto& text = modeChangeButtonUI_->GetChild("modeText")->GetComponent<SEED::UIComponent>()->GetText(0);
            text.text = ModeUtil::groupModeNames[(int)currentGroupMode];

        } else if(selectMode_ == SelectMode::Song){
            backButtonUI_->GetComponent<SEED::UIComponent>()->GetText(0).text = "もどる";
            modeChangeButtonUI_->SetIsActive(true);
            auto& text = modeChangeButtonUI_->GetChild("modeText")->GetComponent<SEED::UIComponent>()->GetText(0);
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

    } else if(modeChangeInput_.Trigger()){
        SelectBackGroundDrawer::isGrooveStart_ = true;

    } else if(decideInput_.Trigger()){
        SelectBackGroundDrawer::isGrooveStart_ = true;

    } else if(backInput_.Trigger()){
        SelectBackGroundDrawer::isGrooveStart_ = true;
    }
}

////////////////////////////////////////////////////////////////////////////////
// マスターボリューム調整
////////////////////////////////////////////////////////////////////////////////
void SongSelector::MasterVolumeAdjust(){

    if(!isSelectBGMPlaying_){

        // フェードイン処理
        if(!songVolumeFadeInTimer_.IsFinished()){

            songVolumeFadeInTimer_.Update();
            float fadeInT = songVolumeFadeInTimer_.GetProgress();
            SEED::AudioManager::SetAudioVolume(songHandle_, fadeInT * currentSong.first->songVolume);

        } else{// フェードアウト処理

            // 境界時間を過ぎていたらフェードアウト開始
            float curAudioTime = SEED::AudioManager::GetAudioPlayTime(songHandle_);
            float fadeOutBorderTime = currentSong.first->songPreviewRange.max - songVolumeFadeOutTimer_.duration;

            if(curAudioTime >= fadeOutBorderTime){
                songVolumeFadeOutTimer_.Update();

                // フェードアウト処理
                float fadeOutT = 1.0f - songVolumeFadeOutTimer_.GetProgress();
                SEED::AudioManager::SetAudioVolume(songHandle_, fadeOutT * currentSong.first->songVolume);

                // フェードアウトが終了したらプレビュー位置に戻して再生し直す
                if(songVolumeFadeOutTimer_.IsFinished()){
                    songVolumeFadeOutTimer_.Reset();
                    songVolumeFadeInTimer_.Reset();

                    SEED::AudioManager::EndAudio(songHandle_);
                    songHandle_ = SEED::AudioManager::PlayAudio(
                        currentSong.first->audioFilePath,
                        true,
                        0.0f,
                        currentSong.first->songPreviewRange.min
                    );
                }
            }
        }
    }

}

//////////////////////////////////////////////////////////////////////////////////
// マウス入力のチェック
//////////////////////////////////////////////////////////////////////////////////
void SongSelector::CheckMouseInput(){

    {// 縦方向のマウス入力チェック

        // マウスホイールでの方向入力
        int32_t mouseWheel = SEED::Input::GetMouseWheel();
        if(mouseWheel > 0){
            mouseInputVertical_ = SEED::GeneralEnum::UpDown::UP;
        } else if(mouseWheel < 0){
            mouseInputVertical_ = SEED::GeneralEnum::UpDown::DOWN;
        }

        // マウスボタンでの方向入力
        if(SEED::Input::IsTriggerMouse(SEED::MOUSE_BUTTON::LEFT)){

            std::vector<SEED::GameObject2D*> upColliderObjs;
            upColliderObjs.push_back(songSelectButtonUI_->GetChild(0));

            std::vector<SEED::GameObject2D*> downColliderObjs;
            downColliderObjs.push_back(songSelectButtonUI_->GetChild(1));

            switch(selectMode_){
            case SelectMode::Group:
            {
                for(int32_t i = 0; i < groupUIs.size(); i++){
                    if(i != centerIdx_){
                        i < centerIdx_ ? upColliderObjs.push_back(groupUIs[i]) : downColliderObjs.push_back(groupUIs[i]);
                    }
                }
                break;
            }
            case SelectMode::Song:
            {
                for(int32_t i = 0; i < songUIs.size(); i++){
                    if(i != centerIdx_){
                        i < centerIdx_ ? upColliderObjs.push_back(songUIs[i]) : downColliderObjs.push_back(songUIs[i]);
                    }
                }
                break;
            }
            case SelectMode::Difficulty:
            {
                upColliderObjs.push_back(difficultySelectButtonUI_->GetChild(0));
                downColliderObjs.push_back(difficultySelectButtonUI_->GetChild(1));
                for(int32_t i = 0; i < difficultyUIs.size(); i++){
                    if(((int)TrackDifficulty::Parallel - i) != (int)currentDifficulty){
                        ((int)TrackDifficulty::Parallel - i) > (int)currentDifficulty ?
                            upColliderObjs.push_back(difficultyUIs[i]) :
                            downColliderObjs.push_back(difficultyUIs[i]);
                    }
                }
                break;
            }
            default:
                break;
            }

            // マウスカーソルと当たっているものに応じて方向を返す
            for(auto& upObj : upColliderObjs){
                if(upObj->GetIsCollided(pMouseCursor_)){
                    mouseInputVertical_ = SEED::GeneralEnum::UpDown::UP;
                }
            }

            for(auto& downObj : downColliderObjs){
                if(downObj->GetIsCollided(pMouseCursor_)){
                    mouseInputVertical_ = SEED::GeneralEnum::UpDown::DOWN;
                }
            }

        } else if(!SEED::Input::IsPressMouse(SEED::MOUSE_BUTTON::LEFT)){
            if(mouseWheel == 0){
                mouseInputVertical_ = SEED::GeneralEnum::UpDown::NONE;
            }
        }
    }

    {// モード変更のマウス入力チェック
        if(SEED::Input::IsTriggerMouse(SEED::MOUSE_BUTTON::LEFT)){

            // マウスカーソルと当たっているものに応じて方向を返す
            if(modeChangeButtonUI_->GetChild(0)->GetIsCollided(pMouseCursor_)){
                mouseInputModeChange_ = SEED::GeneralEnum::LR::LEFT;

            } else if(modeChangeButtonUI_->GetChild(1)->GetIsCollided(pMouseCursor_)){
                mouseInputModeChange_ = SEED::GeneralEnum::LR::RIGHT;
            }

        } else{
            mouseInputModeChange_ = SEED::GeneralEnum::LR::NONE;
        }
    }

    {// 戻るマウス入力チェック
        if(SEED::Input::IsTriggerMouse(SEED::MOUSE_BUTTON::LEFT)){
            // マウスカーソルと当たっているものに応じて決定・戻るを返す
            if(backButtonUI_->GetIsCollided(pMouseCursor_)){
                mouseBackInput_ = true;
            }
        } else{
            mouseBackInput_ = false;
        }
    }

    {// 決定マウス入力チェック
        if(SEED::Input::IsTriggerMouse(SEED::MOUSE_BUTTON::LEFT)){
            // マウスカーソルと当たっているものに応じて決定・戻るを返す
            SEED::GameObject2D* centerUI = nullptr;
            if(selectMode_ == SelectMode::Group){
                centerUI = groupUIs[centerIdx_];
            } else if(selectMode_ == SelectMode::Song){
                centerUI = songUIs[centerIdx_];
            } else if(selectMode_ == SelectMode::Difficulty){
                centerUI = difficultyUIs[int(TrackDifficulty::Parallel) - int(currentDifficulty)];
            }
            if(centerUI && centerUI->GetIsCollided(pMouseCursor_)){
                mouseDecideInput_ = true;
            }
        } else{
            mouseDecideInput_ = false;
        }
    }
}

//////////////////////////////////////////////////////////////////////////////////
// 編集
//////////////////////////////////////////////////////////////////////////////////
void SongSelector::Edit(){
#ifdef _DEBUG

    SEED::ImFunc::CustomBegin("SongSelector", SEED::MoveOnly_TitleBar);
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
    SEED::Methods::File::CreateJsonFile("Resources/Jsons/Settings/song_selector.json", j);
}

//////////////////////////////////////////////////////////////////////////////////
// JSONから読み込み
//////////////////////////////////////////////////////////////////////////////////
void SongSelector::FromJson(){
    nlohmann::json j = SEED::Methods::File::GetJson("Resources/Jsons/Settings/song_selector.json");
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
