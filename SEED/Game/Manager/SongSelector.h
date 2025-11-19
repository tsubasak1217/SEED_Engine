#pragma once
#include <memory>
#include <vector>
#include <SEED/Source/Basic/Object/GameObject.h>
#include <SEED/Source/Basic/Object/GameObject2D.h>
#include <Game/Objects/SongSelect/SongInfo.h>
#include <Game/Objects/SongSelect/SongSelectUtils.h>
#include <SEED/Lib/Functions/Easing.h>
#include <SEED/Lib/enums/Direction.h>
#include <SEED/Source/Manager/AudioManager/AudioManager.h>
#include <SEED/Lib/Shapes/Quad.h>
#include <SEED/Source/Basic/Camera/BaseCamera.h>
#include <SEED/Source/Manager/InputManager/InputHandler.h>

/// <summary>
/// 楽曲選択の管理クラス 
/// </summary>
class SongSelector{
public:
    SongSelector();
    ~SongSelector();
    void Initialize();
    void Update();
    void Draw();
    void EndFrame();

private:
    void InitializeInput(); // 入力関数初期化
    void InitializeUIs(); // UI初期化

private:// ソート関連
    void Sort(); // ソート
    void CreateGroup();// グループ分け
    void SortInGroup();// グループ内ソート
    void UpdateIndex(); // インデックス更新
    void UpdateVisibleSongs(bool isPlayAudio = true, bool isUpdateUIs = true); // 表示されている楽曲の更新
    void UpdateVisibleGroups(bool isUpdateUIs = true); // 表示されているグループの更新
    void FindTrack(const Track& track, int32_t& outSongIndex, int32_t& outGroupIndex);

private:// 選択関連
    void SelectItems();// 項目選択処理
    void SelectSong();// 楽曲選択処理
    void SelectGroup();// グループ選択処理
    void SelectDifficulty();// 難易度選択処理
    void ShiftItem(bool allUpdate = false); // アイテム移動処理

private:// 更新関連
    void CameraControl();// カメラ制御
    void UpdateJacket(); // ジャケットの更新
    void UpdateUIContents(bool allUpdate = false); // UI内容更新
    void ToDifficultySelectUpdate(float timeScale);
    void PlayWaitUpdate();
    void UpdateSelectButtonUIs();
    void UpdateBGDrawerInfo();

private:// 入出力
    void Edit();
    void ToJson();
    void FromJson();

private:
    // 楽曲・グループ
    std::vector<std::unique_ptr<SongInfo>> songList; // 楽曲情報のリスト
    std::vector<SongGroup> songGroups; // 楽曲グループのリスト

    // 選択状況
    int32_t currentGroupIndex = 0; // 現在のグループインデックス
    int32_t currentSongIndex = 0; // 現在の楽曲インデックス
    TrackDifficulty currentDifficulty = TrackDifficulty::Basic; // 現在の難易度
    SongGroup* currentGroup = nullptr; // 現在選択中のグループ
    Track currentSong; // 現在選択中の楽曲
    int32_t centerIdx_;
    UpDown shiftDirection_ = UpDown::NONE; // アイテム移動方向

    // 各種モード
    SelectMode selectMode_ = SelectMode::Group;// 選択中のものを判別するための変数
    GroupMode currentGroupMode = GroupMode::None;; // グループ分け方法
    SortMode currentSortMode = SortMode::Difficulty;; // ソートモード

    // コントロールポイント
    GameObject2D* songControlPts_ = nullptr; // 楽曲選択時コントロールポイント
    GameObject2D* difficultyControlPts_ = nullptr; // 難易度選択時コントロールポイント
    GameObject* cameraControlPts_ = nullptr; // カメラ制御用コントロールポイント

    // 表示曲・グループのリスト
    std::vector<Track*> visibleSongs; // 表示されている楽曲のリスト
    std::vector<SongGroup*> visibleGroups; // 表示されているグループのリスト
    std::vector<GameObject2D*> songUIs; // 楽曲UIのリスト
    std::vector<GameObject2D*> groupUIs; // グループUIのリスト
    std::vector<GameObject2D*> difficultyUIs; // 難易度ごとUIのリスト

    // ボタンUI
    GameObject2D* songSelectButtonUI_ = nullptr;
    GameObject2D* difficultySelectButtonUI_ = nullptr;
    GameObject2D* backButtonUI_ = nullptr;
    GameObject2D* modeChangeButtonUI_ = nullptr;

    // 入力格納変数
    InputHandler<UpDown> verticalInput_;
    InputHandler<LR> holozontalInput_;
    InputHandler<LR> modeChangeInput_;
    InputHandler<bool> decideInput_;
    InputHandler<bool> backInput_;

    // Timer
    Timer inputTimer_;
    Timer itemShiftTimer_;
    Timer toDifficultySelectTimer_;
    Timer playWaitTimer_;
    Timer cameraMoveTimer_;
    Timer buttonUIScalingTimer_;
    Timer buttonUIPressScaleTimer_Up_;
    Timer buttonUIPressScaleTimer_Down_;
    Timer buttonUIPressScaleTimer_Q_;
    Timer buttonUIPressScaleTimer_E_;

    // フラグ類
    static inline bool isFirstPlay_ = true;
    bool changeSceneOrder_ = false;// シーン変更命令フラグ
    bool isShiftItem_ = false; // アイテムが変更されたかどうか
    bool isTransitionDifficulty_Select_ = false; // 難易度選択へ移行中かどうか
    bool isPlayWaiting_ = false; // 曲再生待ち中かどうか

    // 音声
    AudioHandle songHandle_;

    // 色
    std::array<Color, (int)TrackDifficulty::kMaxDifficulty> uiBackColors_;

    // その他細かいもの
    BaseCamera* camera_ = nullptr; // カメラ
    Quad jacket3D_;
    std::string preGroupName_;
    std::string preSongName_;
    Transform2D songUICenterTransform_;
    Transform2D difficultyUICenterTransform_;
    int32_t cameraControlIdx_;
    int32_t preCameraControlIdx_;
    Transform preCameraTransform_;
    std::array<float, 4> cameraInterpolationTimes_ = { 1.0f, 1.0f, 1.0f, 1.0f };
};