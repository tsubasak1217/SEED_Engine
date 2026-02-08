#include "LivelyEffectsOwner.h"
#include <numbers>

LivelyEffectsOwner::LivelyEffectsOwner(){

    nlohmann::json json = SEED::Methods::File::GetJson("Resources/Jsons/Settings/LivelyEffectConfig.json");

    emitTimer_.Initialize(json.value("emitInterval", 1.0f));
    numEmitEvery_ = json.value("numEmitEvery", 12);
    emitRangeRadius_ = json.value("emitRangeRadius", 10.0f);
    endScale_ = json.value("endScale", 0.1f);
    emitBorder_ = json.value("emitBorder", 0.2f);
    baseDepth_ = json.value("baseDepth", 100.0f);
    moveDistance_ = json.value("moveDistance", 1000.0f);
    moveTime_ = json.value("moveTime", 5.0f);
    rotateSpeed_ = json.value("rotateSpeed", (std::numbers::pi_v<float> *2.0f) * 0.25f);
}

/// <summary>
/// 更新処理
/// </summary>
void LivelyEffectsOwner::Update(){
    emitTimer_.Update(1.0f, true);
    curRotateAngle_ += rotateSpeed_ * SEED::ClockManager::DeltaTime();

    if(emitTimer_.IsLoopedNow()){

        // エフェクトを発生させる
        for(int32_t i = 0; i < numEmitEvery_; ++i){

            float angle = curRotateAngle_ + (std::numbers::pi_v<float> *2.0f / numEmitEvery_) * i;
            Vector2 dir = { std::cosf(angle),sinf(angle) };
            float dot = SEED::Methods::Math::Dot(Vector2(0.0f, 1.0f), dir);

            // ボーダー外なら出現させない(上下)
            if(std::fabsf(dot) > emitBorder_){
                continue;
            }
            

            // エフェクトオブジェクトを生成
            LivelyEffect effect;
            SEED::Hierarchy* hierarchy = SEED::GameSystem::GetInstance()->GetScene()->GetHierarchy();
            effect.parentObj_ = hierarchy->CreateEmptyObject();
            effect.effectObj_ = hierarchy->LoadObject("PlayScene/Effects/livelyEffect.prefab");
            effect.effectObj_->SetParent(effect.parentObj_);


            // トランスフォームを初期化
            SEED::Transform transform;
            transform.translate = {
                emitRangeRadius_ * dir.x,
                emitRangeRadius_ * dir.y,
                baseDepth_
            };

            effect.effectObj_->localTransform_ = transform;
            effect.parentObj_->SetWorldTranslate({ 0.0f,0.0f,baseDepth_ });

            // タイマーの初期化
            effect.moveTimer_.Initialize(moveTime_);

            // リストに追加
            effects_.emplace_back(effect);
        }
    }


    // 時間の経過処理
    for(auto it = effects_.begin(); it != effects_.end(); it++){
        it->moveTimer_.Update();
        // 移動量を計算
        float progress = it->moveTimer_.GetProgress();
        it->parentObj_->localTransform_.translate = {
            0.0f,
            0.0f,
            baseDepth_ + moveDistance_ * progress
        };

        // scale更新
        it->parentObj_->localTransform_.scale = Vector3(1.0f - (1.0f - endScale_) * progress);

        // 行列更新
        it->parentObj_->UpdateMatrix(true);
    }
}


/// <summary>
/// フレーム終了時処理
/// </summary>
void LivelyEffectsOwner::EndFrame(){

    // 終了したエフェクトを削除
    SEED::Hierarchy* hierarchy = SEED::GameSystem::GetInstance()->GetScene()->GetHierarchy();
    effects_.remove_if([&](const LivelyEffect& effect){
        if(effect.moveTimer_.IsFinished()){
            // オブジェクトを削除
            hierarchy->EraseObject(effect.effectObj_);
            hierarchy->EraseObject(effect.parentObj_);
            return true;
        }
        return false;
    });
}


/// <summary>
/// 編集処理
/// </summary>
void LivelyEffectsOwner::Edit(){
    SEED::ImFunc::CustomBegin("盛り上がりエフェクト設定", SEED::MoveOnly_TitleBar);
    {
        ImGui::DragFloat("出現間隔", &emitTimer_.duration, 0.01f, 0.1f, 10.0f);
        ImGui::SliderInt("出現数", &numEmitEvery_, 1, 64);
        ImGui::DragFloat("出現半径", &emitRangeRadius_, 0.01f, 0.1f, 1000.0f);
        ImGui::DragFloat("出現ボーダー(上下)", &emitBorder_, 0.01f, 0.0f, 1.0f);
        ImGui::DragFloat("最終拡大率", &endScale_, 0.01f, 0.01f, 1.0f);
        ImGui::DragFloat("出現するz軸の距離", &baseDepth_, 0.01f, 0.0f, 100.0f);
        ImGui::DragFloat("z軸移動距離", &moveDistance_, 0.1f, 0.0f, 2000.0f);
        ImGui::DragFloat("終点までの移動時間", &moveTime_, 0.01f, 0.1f, 20.0f);
        ImGui::DragFloat("回転速度", &rotateSpeed_, 0.01f, 0.0f, std::numbers::pi_v<float> *4.0f);

        if(ImGui::Button("設定を保存")){
            nlohmann::json json;
            json["emitInterval"] = emitTimer_.duration;
            json["numEmitEvery"] = numEmitEvery_;
            json["emitRangeRadius"] = emitRangeRadius_;
            json["endScale"] = endScale_;
            json["emitBorder"] = emitBorder_;
            json["baseDepth"] = baseDepth_;
            json["moveDistance"] = moveDistance_;
            json["moveTime"] = moveTime_;
            json["rotateSpeed"] = rotateSpeed_;
            json["endScale"] = endScale_;
            SEED::Methods::File::CreateJsonFile("Resources/Jsons/Settings/LivelyEffectConfig.json", json);
        }

        ImGui::End();
    }
}