#include "AnswerLane.h"
#include <SEED/Source/Manager/ClockManager/ClockManager.h>
#include <SEED/Lib/Functions/Easing.h>
#include <SEED/Source/SEED.h>

void AnswerLane::Update(){

    // ノーツを処理していた場合
    if(isTapNote){
        evaluationLeftTime = kVisibleTime;
        isTapNote = false;
    }

    // 押したら出現させる
    if(isTrigger){
        isTrigger = false;
        isPress = true;
    }

    if(isRelease){
        isPress = false;
        isRelease = false;
    }

    // 押したら横幅を広げて色を濃くする
    if(isPress){
        leftTime += ClockManager::DeltaTime() * 2.0f;// 離して消えるときより早くする
        leftTime = std::clamp(leftTime, 0.0f, kVisibleTime);

        // 媒介変数の更新
        float t = leftTime / kVisibleTime;
        float ease = EaseOutBack(t);

        // スケールの変更
        tri.scale.x = baseScale * ease;// 横幅を広げる

        // 色の変更
        tri.color.value.w = t;// 色を濃くする

    } else{

        if(leftTime > 0.0f){
            // 時間を減少させる
            leftTime -= ClockManager::DeltaTime();
            leftTime = std::clamp(leftTime, 0.0f, kVisibleTime);

            // 媒介変数の更新
            float t = leftTime / kVisibleTime;

            // 色の更新
            tri.color.value.w = t;

        } else{
            // 判定が終わったら、色を完全に消す
            tri.color.value.w = 0.0f;
        }
    }

    // 判定結果用の更新
    if(evaluationLeftTime > 0.0f){
        evaluationLeftTime -= ClockManager::DeltaTime();
        evaluationLeftTime = std::clamp(evaluationLeftTime, 0.0f, kVisibleTime);
        // 媒介変数の更新
        float t = evaluationLeftTime / kVisibleTime;
        // 色の更新
        evaluationPolygon.color.value.w = t;
    }
}

// 判定の矩形を描画する
void AnswerLane::Draw(){
    SEED::Instance::DrawTriangle(tri);
    SEED::Instance::DrawTriangle(evaluationPolygon);
}
