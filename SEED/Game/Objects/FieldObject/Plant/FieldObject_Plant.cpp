#include "FieldObject_Plant.h"

FieldObject_Plant::FieldObject_Plant(){
    className_ = "FieldObject_Plant";
    name_ = "Plant";
    // モデルの初期化
    model_ = std::make_unique<Model>("FieldObject/Plant.obj");
    model_->isRotateWithQuaternion_ = false;
    model_->cullMode = D3D12_CULL_MODE_NONE;
    model_->meshColor_[0] = MyMath::FloatColor(0x00af58ff);
    // コライダー関連の初期化
    colliderEditor_ = std::make_unique<ColliderEditor>(className_, this);
    InitColliders(ObjectType::Area);
    // 全般の初期化
    FieldObject::Initialize();
}


void FieldObject_Plant::Initialize(){
}

void FieldObject_Plant::Update(){

    // 花の数が変わったら追加
    if(preFlowerVolume_ != flowerVolume_){
        if(isBloomFlower_){

            flowerModel_.clear();
            for(int i = 0; i < flowerVolume_; i++){
                flowerModel_.push_back(std::make_unique<Model>("Assets/cube.obj"));
                float randScale = MyFunc::Random(0.1f, 0.3f);
                flowerModel_[i]->isRotateWithQuaternion_ = false;
                flowerModel_[i]->cullMode = D3D12_CULL_MODE_NONE;
                flowerModel_[i]->translate_ = (model_->GetWorldTranslate() + Vector3(0.0f, 1.0f * model_->scale_.y, 0.0f))+ (MyFunc::RandomVector() * model_->scale_);
                flowerModel_[i]->scale_ = { randScale,randScale,randScale };
                flowerModel_[i]->color_ = flowerColor_;
                flowerModel_[i]->UpdateMatrix();
            }
        }
    }
    preFlowerVolume_ = flowerVolume_;

    // 基本の更新
    FieldObject::Update();
}

void FieldObject_Plant::Draw(){
    FieldObject::Draw();
    // 花の描画
    for(auto& flower : flowerModel_){
        flower->Draw();
    }
}