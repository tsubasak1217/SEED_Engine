#include "Shadow.h"

///stl
//algorithm
#include <algorithm>

//lcoal
//stage
#include "../Manager/Stage.h"

//math
#include "MyMath.h"

Shadow::Shadow(BaseObject* _host)
    :host_(_host){}

Shadow::~Shadow(){}

void Shadow::Initialize(){
    model_ = std::make_unique<Model>("shadowPlane.obj");
    model_->translate_ = host_->GetWorldTranslate();
    model_->scale_ = {1.5f,1.5f,1.5f};
    model_->blendMode_ = BlendMode::NORMAL;
}

static const Vector3 blockSize = {2.5f,2.5,2.5};

void Shadow::Update(Stage* currentStage){
    if(currentStage->GetObjects().empty()){
        model_->translate_ = host_->GetWorldTranslate();
        model_->translate_.y = -1000.0f;
        return;
    }

    Vector3 hostPos = host_->GetWorldTranslate();
    Vector2 hostPosXZ = {hostPos.x,hostPos.z};
    model_->translate_ = hostPos;
    model_->translate_.y = -1000.0f;

    Vector3 objectTranslate;
    Vector2 objectTranslateXZ;
    Vector2 nearestPos;
    for(auto& object : currentStage->GetObjects()){
        objectTranslate = object->GetWorldTranslate();
        //ホストより上にあるオブジェクトは スキップ
        if(hostPos.y - objectTranslate.y <= -0.3f){
            continue;
        }
        objectTranslateXZ = {objectTranslate.x,objectTranslate.z};

        // XZ 平面で 当たり判定
        nearestPos = {
            std::clamp(hostPosXZ.x,objectTranslateXZ.x - blockSize.x,objectTranslateXZ.x + blockSize.x),
            std::clamp(hostPosXZ.y,objectTranslateXZ.y - blockSize.y,objectTranslateXZ.y + blockSize.y)
        };

        if(MyMath::LengthSq(nearestPos - hostPosXZ) <= 0.001f/*仮決め*/){
            // 範囲内なら
            // 一番高さが近いオブジェクトの位置を取得
            model_->translate_.y = (std::max)(model_->translate_.y,objectTranslate.y + 0.1f);
        }
    }

    model_->Update();
}

void Shadow::Draw(){
    model_->Draw();
}
