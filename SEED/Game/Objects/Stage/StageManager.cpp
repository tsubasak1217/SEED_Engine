#include "StageManager.h"
#include <SEED/Source/Manager/ImGuiManager/ImGuiManager.h>
#include <SEED/Source/SEED.h>
#include <SEED/Source/Manager/CollisionManager/Collision.h>
#include <Game/Objects/Actor/Player/Player.h>

/////////////////////////////////////////////////////////////////
// static変数
/////////////////////////////////////////////////////////////////
StageManager* StageManager::instance_ = nullptr;

/////////////////////////////////////////////////////////////////
// コンストラクタ・デストラクタ
/////////////////////////////////////////////////////////////////
StageManager* StageManager::GetInstance(){
    if(!instance_){
        instance_ = new StageManager();
    }
    return instance_;
}

StageManager::~StageManager(){
}


/////////////////////////////////////////////////////////////////
// 初期化
/////////////////////////////////////////////////////////////////
void StageManager::Initialize(){
    stages_.clear();
    currentStageNo_ = 0;
    kMaxStageNo_ = 10;
    kBlockSize_ = 10.0f;
    isEditMode_ = true;
    // エディットステージの初期化
    editStage_ = std::make_unique<Stage>();
    // ステージの読み込み
    LoadStages();

    // 上から見下ろすようにカメラを設定
    SEED::SetCamera("main");
    BaseCamera* camera = SEED::GetCamera();
    camera->SetTranslation(Vector3(0.0f, 100.0f, 0.0f));
    camera->SetRotation({ 3.141582f * 0.5f, 0.0f, 0.0f });
    //camera->SetProjectionMode(PROJECTIONMODE::ORTHO);
    SetCameraPosition(editStage_.get());

    // ブロックの名前を登録
    blockNameMap_["Wall"] = BlockType::Wall;
    blockNameMap_["Box"] = BlockType::Box;
    blockNameMap_["Player"] = BlockType::Player;
    blockNameMap_["Toge"] = BlockType::Toge;
}


/////////////////////////////////////////////////////////////////
// フレームの開始時処理
/////////////////////////////////////////////////////////////////
void StageManager::BeginFrame(){

    if(!isEditMode_){
        for(auto& stage : stages_){
            stage->BeginFrame();
        }
    } else{
        editStage_->BeginFrame();
    }
}

/////////////////////////////////////////////////////////////////
// フレームの終了時処理
/////////////////////////////////////////////////////////////////
void StageManager::EndFrame(){
    if(!isEditMode_){
        for(auto& stage : stages_){
            stage->EndFrame();
        }
    } else{
        editStage_->EndFrame();
    }
}


/////////////////////////////////////////////////////////////////
// 更新
/////////////////////////////////////////////////////////////////
void StageManager::Update(){

#ifdef _DEBUG
    EditStage();
#endif // _DEBUG

    if(!isEditMode_){
        for(auto& stage : stages_){
            stage->Update();
        }
    }
}


/////////////////////////////////////////////////////////////////
// 描画
/////////////////////////////////////////////////////////////////
void StageManager::Draw(){
    if(!isEditMode_){
        stages_[currentStageNo_]->Draw();
    } else{
        editStage_->Draw();
    }
}


/////////////////////////////////////////////////////////////////
// ステージの編集
/////////////////////////////////////////////////////////////////
void StageManager::EditStage(){
    static Vector2i selectAddress = Vector2i(-1, -1);
    static Vector2i selectAddress2 = Vector2i(-1, -1);
    static std::string selectBlockName = "Wall";
    static Vector2i stageSize = Vector2i(10, 10);
    static float cameraHeight = editStage_->GetCameraHeight();
    static int32_t selectFileIdx = 0;

    ImGui::Begin("StageEditor");
    ImGui::Checkbox("EditMode", &isEditMode_);

    if(!isEditMode_){
        ImGui::End();
        return;
    }

    /*-------------------------------------------*/
    // テストプレイを行うか
    /*-------------------------------------------*/
    if(ImGui::Checkbox("isTestPlay", &isTestPlay_)){
        if(!isTestPlay_){
            // 操作前の状態に戻す
            editStage_->LoadStage(filePaths_[selectFileIdx]);
        } else{
            // いったんファイルに書き出し
            OutputStage(editStage_.get());
        }
    }

    if(!isTestPlay_){

        /*-------------------------------------------*/
        // ファイル書き出し
        /*-------------------------------------------*/
        if(ImGui::Button("Output")){
            OutputStage(editStage_.get());

            // メッセージボックスを表示
            MessageBoxA(nullptr, "Output Complete", "StageEditor", MB_OK);
        }

        /*-------------------------------------------*/
        // ファイル読み込み
        /*-------------------------------------------*/
        if(ImGui::BeginCombo("Load From Directory", filePaths_[selectFileIdx].c_str())){
            for(auto& filePath : filePaths_){
                if(ImGui::Selectable(filePath.c_str())){
                    LoadStageToEditor(filePath);
                    selectFileIdx = (int32_t)std::distance(filePaths_.begin(), std::find(filePaths_.begin(), filePaths_.end(), filePath));
                }
            }
            ImGui::EndCombo();
        }

        /*-------------------------------------------*/
        // ステージ番号を選択
        /*-------------------------------------------*/
        std::string selectStageIdx = std::to_string(editStage_->GetStageNo());
        if(ImGui::BeginCombo("StageNo", (editStage_->GetStageNo() >= 0) ? selectStageIdx.c_str() : "editorStage")){
            for(int32_t i = -1; i < kMaxStageNo_; i++){
                bool isSelected = (currentStageNo_ == i);

                if(i != -1){
                    if(ImGui::Selectable(std::to_string(i).c_str(), isSelected)){
                        editStage_->SetStageNo(i);
                    }
                } else{
                    if(ImGui::Selectable("editorStage", isSelected)){
                        editStage_->SetStageNo(i);
                    }
                }

                if(isSelected){
                    ImGui::SetItemDefaultFocus();
                }
            }
            ImGui::EndCombo();
        }

        /*-------------------------------------------*/
        // ステージのサイズを設定
        /*-------------------------------------------*/
        if(ImGui::DragInt2("StageSize", &stageSize.x, 0.05f, 1)){
            editStage_->SetStageSize(Vector2i(stageSize.x, stageSize.y));
            editStage_->ResizeStage();
            SetCameraPosition(editStage_.get());
        };

        /*-------------------------------------------*/
        // カメラの高さを設定
        /*-------------------------------------------*/
        if(ImGui::DragFloat("CameraHeight", &cameraHeight, 0.5f)){
            editStage_->SetCameraHeight(cameraHeight);
            SetCameraPosition(editStage_.get());
        }

        /*-------------------------------------------*/
        // ドッペルゲンガーを生成する歩数
        /*-------------------------------------------*/
        static int32_t doppelgangerStep = Player::GetDoppelSteps();
        if(ImGui::DragInt("DoppelgangerStep", &doppelgangerStep, 0.05f, 0)){
            Player::SetDoppelSteps(doppelgangerStep);
        }

        /*-------------------------------------------*/
        // ブロックの種類を選択
        /*-------------------------------------------*/
        for(auto& blockName : blockNameMap_){
            // 端で改行する正方形のボタンを作成
            if(ImGui::Button(blockName.first.c_str(), ImVec2(100, 100))){
                selectBlockName = blockName.first;
            }

            // 次のアイテムの位置がウィンドウの右端を超えるなら改行
            if(ImGui::GetCursorPosX() + 100 + ImGui::GetStyle().ItemSpacing.x <= ImGui::GetWindowContentRegionMax().x){
                ImGui::SameLine();
            }
        }

        /*-------------------------------------------*/
        // マウス右ボタンでアドレスを選択し離すとブロックを追加
        /*-------------------------------------------*/
        if(Input::IsPressMouse(MOUSE_BUTTON::RIGHT)){
            selectAddress = SelectAddressByMouse();

        } else if(Input::IsReleaseMouse(MOUSE_BUTTON::RIGHT)){
            if(selectAddress != Vector2i(-1, -1)){
                editStage_->TryInsertBlock(selectAddress, blockNameMap_[selectBlockName]);
            }
        }

        /*-------------------------------------------*/
        // マウス左ボタンでブロックを選択
        /*-------------------------------------------*/
        if(Input::IsTriggerMouse(MOUSE_BUTTON::LEFT)){
            selectAddress2 = SelectAddressByMouse();
        }
        // escapeキーで選択したブロックを削除
        if(Input::IsTriggerKey(DIK_ESCAPE)){
            editStage_->DeleteBlock(selectAddress2);
        }

        /*-------------------------------------------*/
        // 行列のみ更新
        /*-------------------------------------------*/
        editStage_->UpdateMatrix();

    } else{
        // テストプレイ
        editStage_->Update();
    }

    ImGui::End();
}



void StageManager::SetCameraPosition(Stage* stage){
    // カメラ取得
    BaseCamera* camera = SEED::GetCamera();

    // xz平面におけるカメラの位置をステージの中心に設定
    Vector2i stageSize = stage->GetStageSize();
    Vector2 stageCenter = Vector2((float)stageSize.x, (float)-stageSize.y) * kBlockSize_ * 0.5f;
    camera->SetTranslation(Vector3(stageCenter.x, stage->GetCameraHeight(), stageCenter.y));
    camera->UpdateMatrix();
}

Vector2i StageManager::SelectAddressByMouse(){
    // カメラ取得
    BaseCamera* camera = SEED::GetCamera();

    // カメラから発射されるレイとの交差判定を取る平面
    static Quad plane = {
        { -1000.0f,0.0f,-1000.0f },
        { 1000.0f,0.0f,-1000.0f },
        { 1000.0f,0.0f,1000.0f },
        { -1000.0f,0.0f,1000.0f }
    };

    // レイの交差位置を取得
    Line ray = camera->GetRay(Input::GetMousePosition());
    CollisionData data = Collision::Quad::Line(plane, ray);

    // 交差位置をアドレスに変換
    Vector2i address = Vector2i(
        static_cast<int32_t>((data.hitPos.value().x + kBlockSize_ * 0.5f) / kBlockSize_),
        static_cast<int32_t>((-data.hitPos.value().z + kBlockSize_ * 0.5f) / kBlockSize_)
    );

    // クランプする
    address.x = std::clamp(address.x, 0, editStage_->GetStageSize().x);
    address.y = std::clamp(address.y, 0, editStage_->GetStageSize().y);

    return address;
}

Stage* StageManager::GetCurrentStage() const{
    if(isEditMode_){
        return editStage_.get();
    } else{
        return stages_[currentStageNo_].get();
    }
}

/////////////////////////////////////////////////////////////////
// ステージの読み込み
/////////////////////////////////////////////////////////////////
void StageManager::LoadStages(){
    stages_.clear();
    filePaths_.clear();

    // ディレクトリ内にあるjsonファイルを探し、ステージを読み込む
    for(const auto& entry : std::filesystem::directory_iterator(directoryPath_)){
        if(entry.is_regular_file() && entry.path().extension() == ".json"){
            std::string filePath = entry.path().string();

            // editStage.jsonは読み込まない
            if(filePath != directoryPath_ + "editStage.json"){
                stages_.emplace_back(std::make_unique<Stage>());
                stages_.back()->Initialize(filePath);
            } else{
                editStage_->Initialize(filePath);
                SetCameraPosition(editStage_.get());
            }

            filePaths_.push_back(filePath);
        }
    }

    // ステージ番号順にソート
    std::sort(stages_.begin(), stages_.end(), [](const std::unique_ptr<Stage>& a, const std::unique_ptr<Stage>& b){
        return a->GetStageNo() < b->GetStageNo();
    });
}


void StageManager::LoadStageToEditor(const std::string& filePath){
    editStage_->Initialize(filePath);
    SetCameraPosition(editStage_.get());
}

/////////////////////////////////////////////////////////////////
// ステージの出力
/////////////////////////////////////////////////////////////////
void StageManager::OutputStage(){
    for(auto& stage : stages_){
        stage->OutputStage();
    }

    editStage_->OutputStage();
}

void StageManager::OutputStage(Stage* stage){
    stage->OutputStage();
}

