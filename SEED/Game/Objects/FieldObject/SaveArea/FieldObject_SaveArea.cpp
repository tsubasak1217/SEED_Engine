#include "FieldObject_SaveArea.h"
#include "StageManager.h"
#include "Scene_Game/Scene_Game.h"
#include  "../GameSystem.h"

////////////////////////////////////////////////////////////////////////
// static変数
////////////////////////////////////////////////////////////////////////
const std::string FieldObject_SaveArea::jsonPath_ = "resources/jsons/StageSaveDatas/";

////////////////////////////////////////////////////////////////////////
// コンストラクタ
////////////////////////////////////////////////////////////////////////
FieldObject_SaveArea::FieldObject_SaveArea(){
    // 名前関連の初期化
    className_ = "FieldObject_SaveArea";
    name_ = "SaveArea";
    // モデルの初期化
    std::string path = "FieldObject/" + name_ + ".obj";
    model_ = std::make_unique<Model>(path);
    model_->isRotateWithQuaternion_ = false;
    model_->color_.w = 0.2f;
    model_->blendMode_ = BlendMode::ADD;
    // コライダー関連の初期化
    colliderEditor_ = std::make_unique<ColliderEditor>(className_, this);
    InitColliders(ObjectType::Area);
    // 全般の初期化
    FieldObject::Initialize();
}

////////////////////////////////////////////////////////////////////////
// 初期化関数
////////////////////////////////////////////////////////////////////////
void FieldObject_SaveArea::Initialize(){
}

////////////////////////////////////////////////////////////////////////
// 描画関数
////////////////////////////////////////////////////////////////////////
void FieldObject_SaveArea::Draw(){
#ifdef _DEBUG
    model_->color_.w = 0.2f;
    FieldObject::Draw();
#endif // _DEBUG
}

////////////////////////////////////////////////////////////////////////
// フレーム開始処理
////////////////////////////////////////////////////////////////////////
void FieldObject_SaveArea::BeginFrame(){
    FieldObject::BeginFrame();
    preIsCollidePlayer_ = isCollidePlayer_;
    isCollidePlayer_ = false;
}

////////////////////////////////////////////////////////////////////////
// 衝突処理
////////////////////////////////////////////////////////////////////////
void FieldObject_SaveArea::OnCollision( BaseObject* other, ObjectType objectType){
    if(objectType == ObjectType::Player){
        isCollidePlayer_ = true;

        // 衝突時
        if(!preIsCollidePlayer_ && isCollidePlayer_){
            Stage* stage = StageManager::GetCurrentStage();
            int32_t stageNo = stage->GetStageNo() + 1;
            std::string filePath = jsonPath_ + "stage_" + std::to_string(stageNo) + ".json";

            // セーブ
            if(Scene_Game* scene = dynamic_cast<Scene_Game*>(GameSystem::GetScene())){

                // 地形のセーブ
                if(FieldEditor* editor = scene->GetFieldEditor()){
                    Player* player = dynamic_cast<Player*>(other);
                    editor->SaveToJson(filePath, stageNo, true, player);
                }

                // 敵のセーブ
                EnemyManager* enemyEditor = stage->GetEnemyManager();
                enemyEditor->SaveEnemies(true);
            }
        }
    }
}
