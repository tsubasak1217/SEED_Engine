#include "EmitterBase.h"
#include <SEED/Source/Manager/ClockManager/ClockManager.h>
#include <SEED/Source/Manager/TextureManager/TextureManager.h>
#include <SEED/Source/Manager/ParticleManager/CPUParticle/ParticleManager.h>


EmitterBase::EmitterBase(){
    colors_.push_back(Color());
    totalTime_ = interval_;
    // タグの生成
    CreateTag();
}

////////////////////////////////////////////////////////
// 更新関数
////////////////////////////////////////////////////////
void EmitterBase::Update(){

    if(!isAlive_){
        return;
    }

    // 初期化時にあらかじめ指定時間分更新しておく場合の処理
    if(initUpdateTime_ > 0.0f){
        // 初期化時にあらかじめ指定時間分更新しておく
        while(initUpdateTime_ > 0.0f){
            totalTime_ += 1.0f / 60.0f;
            if(totalTime_ >= interval_){
                totalTime_ = 0.0f;
                emitCount_++;
                // アクティブな時だけ発生依頼を出す
                if(isActive_){
                    // ここでは直接出す
                    emitOrder_ = true;
                    ParticleManager::Emit(this);
                    emitOrder_ = false;
                }

                // 無限発生の場合はこのまま処理を続ける
                if(emitType_ == EmitType::kInfinite){
                    continue;
                }

                // カスタム発生 or 1回発生の場合は最大発生回数に達したら非アクティブにする
                if(emitCount_ >= kMaxEmitCount_ or emitType_ == EmitType::kOnce){
                    isAlive_ = false;
                    emitCount_ = 0;
                    break;
                }
            }
            initUpdateTime_ -= 1.0f / 60.0f;
        }

    } else{// 通常の更新処理

        totalTime_ += ClockManager::DeltaTime();

        if(totalTime_ >= interval_){
            totalTime_ = 0.0f;
            emitCount_++;

            // アクティブな時だけ発生依頼を出す
            if(isActive_){
                emitOrder_ = true;
            }

            // 無限発生の場合はこのまま処理を続ける
            if(emitType_ == EmitType::kInfinite){
                return;
            }

            // カスタム発生 or 1回発生の場合は最大発生回数に達したら非アクティブにする
            if(emitCount_ >= kMaxEmitCount_ or emitType_ == EmitType::kOnce){

                isAlive_ = false;
                emitCount_ = 0;
                return;
            }
        }
    }
}


#ifdef _DEBUG
////////////////////////////////////////////////////////////
// カーブパラメーターの編集
////////////////////////////////////////////////////////////
void EmitterBase::EditCurves(){

    if(ImGui::CollapsingHeader("スケールカーブ" + idTag_)){
        scaleCurve_.Edit();
    }
    if(ImGui::CollapsingHeader("移動速度カーブ" + idTag_)){
        velocityCurve_.Edit();
    }
    if(ImGui::CollapsingHeader("回転速度カーブ" + idTag_)){
        rotateCurve_.Edit();
    }
    if(ImGui::CollapsingHeader("色カーブ" + idTag_)){
        colorCurve_.Edit();
        ImFunc::Combo("色の制御方法##" + idTag_, colorMode_, { "RGBA","HSVA" });
    }
}

////////////////////////////////////////////////////////////
// 全般の情報
////////////////////////////////////////////////////////////
void EmitterBase::EditGeneral(){
    ImFunc::Combo("発生タイプ" + idTag_, emitType_, { "一度のみ","ずっと","指定回数" });
    if(emitType_ == EmitType::kCustom){
        ImGui::DragInt("発生回数" + idTag_, &kMaxEmitCount_, 1);
    }
}


////////////////////////////////////////////////////////////
// 頻度などの情報
////////////////////////////////////////////////////////////
void EmitterBase::EditFrequency(){
    ImGui::DragFloat("発生間隔", &interval_, 0.01f, 0.0f);
    ImGui::DragInt("一度に発生する数", &numEmitEvery_, 1, 0, 100);
}

#endif // _DEBUG

////////////////////////////////////////////////////////////
// タグの作成
////////////////////////////////////////////////////////////
void EmitterBase::CreateTag(){
    idTag_ = "##" + MyFunc::PtrToStr(this);
}


///////////////////////////////////////////////////////////
// 出力
///////////////////////////////////////////////////////////
nlohmann::json EmitterBase::ExportToJson(){
    nlohmann::json j;

    // 全般の情報
    j["emitType"] = (int)emitType_;
    j["blendMode"] = (int)blendMode_;
    j["initUpdateTime"] = initUpdateTime_;

    // パラメータ
    j["gravity"] = gravity_;

    // フラグ類
    j["isUseGravity"] = isUseGravity_;
    j["isUseRotate"] = isUseRotate_;
    j["isRotateRandomInit"] = isRoteteRandomInit_;
    j["isSetEndPosition"] = isSetGoalPosition_;

    // カーブ
    j["scaleCurve"] = scaleCurve_.ToJson();
    j["colorCurve"] = colorCurve_.ToJson();
    j["rotateCurve"] = rotateCurve_.ToJson();
    j["velosityCurve"] = velocityCurve_.ToJson();

    // 発生頻度などの情報
    j["interval"] = interval_;
    j["numEmitEvery"] = numEmitEvery_;
    j["kMaxEmitCount"] = kMaxEmitCount_;

    // 色の情報
    for(auto& color : colors_){
        j["colors"].push_back({ color.value.x, color.value.y, color.value.z, color.value.w });
    }

    // テクスチャの情報
    for(auto& textureHandle : texturePaths_){
        j["textureHandles"].push_back(textureHandle);
    }

    return j;
}

///////////////////////////////////////////////////////////
// 読み込み
///////////////////////////////////////////////////////////
void EmitterBase::LoadFromJson(const nlohmann::json& j){

    // 全般の情報
    emitType_ = (EmitType)j.value("emitType",0);
    blendMode_ = (BlendMode)j.value("blendMode",3);
    initUpdateTime_ = j.value("initUpdateTime", 0.0f);

    // パラメータ
    gravity_ = j.value("gravity",0.0f);

    // フラグ類
    isUseGravity_ = j.value("isUseGravity", false);
    isUseRotate_ = j.value("isUseRotate", false);
    isRoteteRandomInit_ = j.value("isRotateRandomInit", false);
    isSetGoalPosition_ = j.value("isSetEndPosition", false);

    // カーブ
    if(j.contains("scaleCurve")){
        scaleCurve_.FromJson(j["scaleCurve"]);
    }
    if(j.contains("colorCurve")){
        colorCurve_.FromJson(j["colorCurve"]);
    }
    if(j.contains("rotateCurve")){
        rotateCurve_.FromJson(j["rotateCurve"]);
    }
    if(j.contains("velosityCurve")){
        velocityCurve_.FromJson(j["velosityCurve"]);
    }

    // 発生頻度などの情報
    interval_ = j.value("interval",0.1f);
    numEmitEvery_ = j.value("numEmitEvery",1);
    kMaxEmitCount_ = j.value("kMaxEmitCount",5);

    // 色の情報
    colors_.clear();
    if(j.contains("colors")){
        for(auto& color : j["colors"]){
            colors_.emplace_back();
            colors_.back() = color;
        }
    } else{
        colors_.emplace_back(Color());
    }

    // テクスチャの情報
    texturePaths_.clear();
    if(j.contains("textureHandles")){
        for(auto& textureHandle : j["textureHandles"]){
            texturePaths_.push_back(textureHandle);
            // テクスチャのセットに追加
            textureDict_[textureHandle] = TextureManager::GetImGuiTexture(textureHandle);
        }
    } else{
        texturePaths_.push_back("[Engine]white1x1.png");
    }

}