#include "RythmGameManager.h"
#include <SEED/Source/Manager/CameraManager/CameraManager.h>
#include <SEED/Source/SEED.h>
#include <Game/Config/PlaySettings.h>

/////////////////////////////////////////////////////////////////////////////////
// static変数の初期化
/////////////////////////////////////////////////////////////////////////////////
RythmGameManager* RythmGameManager::instance_ = nullptr;

////////////////////////////////////////////////////////////////////////////////
// コンストラクタ・デストラクタ
////////////////////////////////////////////////////////////////////////////////
RythmGameManager::RythmGameManager(){
}

RythmGameManager::~RythmGameManager(){
    // カメラの登録解除
    CameraManager::GetInstance()->DeleteCamera("gameCamera");
}

////////////////////////////////////////////////////////////////////////////////
// インスタンスの取得
////////////////////////////////////////////////////////////////////////////////
RythmGameManager* RythmGameManager::GetInstance(){
    if(!instance_){
        instance_ = new RythmGameManager();
    }
    return instance_;
}

//////////////////////////////////////////////////////////////////////////////////
// 初期化
//////////////////////////////////////////////////////////////////////////////////
void RythmGameManager::Initialize(){
    // カメラの初期化
    gameCamera_ = std::make_unique<BaseCamera>();
    gameCamera_->SetTranslation(Vector3(0.0f, 0.0f, 0.0f));
    gameCamera_->UpdateMatrix();

    // カメラの登録,設定
    CameraManager::GetInstance()->AddCamera("gameCamera", gameCamera_.get());
    SEED::SetCamera("gameCamera");
    SEED::SetCamera("debug");

    // settingsの初期化
    PlaySettings::GetInstance();

    // Inputの初期化
    PlayerInput::GetInstance()->Initialize();

    // 譜面データの初期化
    notesData_ = std::make_unique<NotesData>(true);

    // プレイフィールドの初期化
    PlayField::GetInstance()->SetNoteData(notesData_.get());

    // 判定の初期化
    Judgement::GetInstance();

    // カーソルの設定
    SEED::SetIsRepeatCursor(false);
}

//////////////////////////////////////////////////////////////////////////////////
// フレーム開始処理
//////////////////////////////////////////////////////////////////////////////////
void RythmGameManager::BeginFrame(){
    // inputのフレーム開始処理
    PlayerInput::GetInstance()->BeginFrame();
    // 譜面データのフレーム開始処理
    if(notesData_){
        notesData_->BeginFrame();
    }
}

//////////////////////////////////////////////////////////////////////////////////
// フレーム終了処理
//////////////////////////////////////////////////////////////////////////////////
void RythmGameManager::EndFrame(){
}

//////////////////////////////////////////////////////////////////////////////////
// 更新
//////////////////////////////////////////////////////////////////////////////////
void RythmGameManager::Update(){
    // Inputの更新
    PlayerInput::GetInstance()->Update();

    // 譜面データの更新
    notesData_->Update();

    // ノーツの判定
    Judgement::GetInstance()->Judge(notesData_.get());

    // プレイフィールドの更新
    PlayField::GetInstance()->Update();

    // escapeキーでカーソルのフラグを切り替え
    if(Input::IsPressKey(DIK_ESCAPE)){
        SEED::ToggleRepeatCursor();
    }
}

//////////////////////////////////////////////////////////////////////////////////
// 描画
//////////////////////////////////////////////////////////////////////////////////
void RythmGameManager::Draw(){
    // Inputのカーソル描画
    PlayerInput::GetInstance()->Draw();
    
    // プレイフィールドの描画
    PlayField::GetInstance()->Draw();

    // 譜面データの描画
    notesData_->Draw();
}
