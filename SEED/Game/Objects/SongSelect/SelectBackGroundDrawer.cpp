#include "SelectBackGroundDrawer.h"
#include <vector>
#include <SEED/Lib/Tensor/Vector2.h>
#include <Environment/Environment.h>
#include <SEED/Lib/Structs/Timer.h>
#include <SEED/Lib/Shapes/Quad.h>
#include <SEED/Lib/Functions/ShapeMath.h>
#include <SEED/Source/SEED.h>
#include <SEED/Lib/Functions/Easing.h>
#include <Game/GameSystem.h>

SelectBackGroundDrawer::SelectBackGroundDrawer(){

}

void SelectBackGroundDrawer::Initialize(){
    // 色オブジェクトから色を取得
    auto* hierarchy = GameSystem::GetScene()->GetHierarchy();
    bgColorListObj_ = hierarchy->LoadObject("SelectScene/bgColorList.prefab");
    auto& colorList = bgColorListObj_->GetComponent<ColorListComponent>()->GetColors();
    for(int i = 0; i < (int)TrackDifficulty::kMaxDifficulty; i++){
        if(colorList.size() <= i){ break; }
        clearColors_[i] = colorList[i];
    }
}

void SelectBackGroundDrawer::Update(){

    auto& colorList = bgColorListObj_->GetComponent<ColorListComponent>()->GetColors();
    for(int i = 0; i < (int)TrackDifficulty::kMaxDifficulty; i++){
        if(colorList.size() <= i){ break; }
        clearColors_[i] = colorList[i];
    }

    if(Input::IsTriggerKey(DIK_G)){
        isGrooveStart_ = true;
    }
}

void SelectBackGroundDrawer::Draw(){

    DrawScrollingBackground();
    DrawGrooveObjects();

    if(isGrooveStart_){
        isGrooveStart_ = false; // グルーヴ開始フラグをリセット
    }
}

///////////////////////////////////////////////////////////////////////////////////
// スクロールする背景の描画
///////////////////////////////////////////////////////////////////////////////////
void SelectBackGroundDrawer::DrawScrollingBackground(){

    struct GrooveObj{
        Vector2 position; // 位置
        Quad2D quad; // 描画用のQuad
    };

    static float kGridSize = 120.0f;
    static std::vector<std::vector<GrooveObj>> scrollObjects;
    static Vector2 kItemListSize;
    static std::list<Timer> grooveTimers;
    static float kGrooveTime = 0.6f; // グルーヴの時間
    static float kVisivleTimeRate = 0.4f;
    static bool isInitialized = false;

    // スクロール
    static Vector2 scrollDir = MyMath::Normalize({ -1.0f,1.0f });// スクロール方向(左下)
    static float scrollSpeed = 20.0f; // スクロール速度
    static Quad2D backQuad = MakeQuad2D(kWindowSize); // 背景のQuad

    // 初期化がまだならグリッドの位置を計算
    if(!isInitialized){

        Vector2 center = kWindowCenter;
        backQuad.translate = center; // 背景Quadの位置を中央に設定
        backQuad.drawLocation = DrawLocation::Back; // 背景Quadの描画位置を設定
        backQuad.layer = 0;
        backQuad.isApplyViewMat = false;
        backQuad.color = clearColors_[(int)currentDifficulty];

        // 格納可能な縦横数を計算
        int rows = int((kWindowSize.y * 0.5f) / kGridSize) * 2 + 3;
        int cols = int((kWindowSize.x * 0.5f) / kGridSize) * 2 + 3;
        scrollObjects.resize(rows);

        // グリッドの位置を計算して格納
        for(int i = 0; i < rows; i++){
            for(int j = 0; j < cols; j++){
                // グリッドの位置を計算
                GrooveObj obj;
                obj.position = center + Vector2((j - cols / 2) * kGridSize, (i - rows / 2) * kGridSize);
                obj.quad = MakeQuad2D(kGridSize * 0.8f);
                obj.quad.translate = obj.position;
                obj.quad.color = MyFunc::RandomColor({ 0x314072FF, 0xe94c76FF, 0x6c3f82FF, 0xede690FF }); // ランダムな色
                obj.quad.layer = 1;
                obj.quad.isApplyViewMat = false;
                obj.quad.drawLocation = DrawLocation::Back;
                scrollObjects[i].push_back(obj);
            }
        }

        kItemListSize = {
            std::fabsf(scrollObjects[0].back().position.x - scrollObjects[0][0].position.x) + kGridSize,
            std::fabsf(scrollObjects.back()[0].position.y - scrollObjects[0][0].position.y) + kGridSize
        };

        isInitialized = true; // 初期化フラグを立てる
    }

    // グルーブフラグがあればタイマーを追加
    if(isGrooveStart_){
        grooveTimers.emplace_back(Timer(kGrooveTime));
    }

    // グルーブオブジェクトの位置を更新
    static Vector2 minPos = { -kGridSize * 0.5f, -kGridSize * 0.5f };
    static Vector2 maxPos = { kWindowSize.x + kGridSize * 0.5f, kWindowSize.y + kGridSize * 0.5f };
    static Range2D exsistRange = { minPos, maxPos };
    for(int i = 0; i < scrollObjects.size(); i++){
        for(int j = 0; j < scrollObjects[i].size(); j++){

            auto& quad = scrollObjects[i][j].quad;
            quad.translate += scrollDir * scrollSpeed * ClockManager::DeltaTime();

            // 範囲外に出たら位置を反転
            if(!MyFunc::IsContain(exsistRange, quad.translate)){

                Vector2 outVec = quad.translate - kWindowCenter;

                // 進行方向に出た場合にのみ位置を調整
                if(MyMath::Dot(outVec, scrollDir) < 0.0f){ continue; }

                // x座標の調整
                if(quad.translate.x < minPos.x || quad.translate.x > maxPos.x){
                    quad.translate.x += kItemListSize.x * (outVec.x < 0.0f ? 1.0f : -1.0f);
                }

                // y座標の調整
                if(quad.translate.y < minPos.y || quad.translate.y > maxPos.y){
                    quad.translate.y += kItemListSize.y * (outVec.y < 0.0f ? 1.0f : -1.0f);
                }
            }
        }
    }

    // グルーブオブジェクトの更新
    for(int i = 0; i < scrollObjects.size(); i++){
        for(int j = 0; j < scrollObjects[i].size(); j++){

            for(auto& timer : grooveTimers){

                float t = 1.0f - timer.GetProgress();
                float ease = EaseOutBack(t);
                float scale = 1.0f + 0.1f * ease;

                // パラメータの更新
                scrollObjects[i][j].quad.scale = { scale,scale };
            }

            // 描画
            SEED::DrawQuad2D(scrollObjects[i][j].quad);
        }
    }


    // グルーヴのタイマーを更新
    for(auto& timer : grooveTimers){
        timer.Update();
    }

    // 終了したタイマーの削除
    grooveTimers.remove_if([](const Timer& timer){
        return timer.IsFinished();
    });


    {// 色の更新

        Color aimColor = clearColors_[(int)currentDifficulty].ToHSVA();
        Color curColor = MyMath::RGB_to_HSV(backQuad.color.value);
        static float lerpRate = 0.2f;

        // 色相補間（最短距離補間）
        float h1 = curColor.value.x;
        float h2 = aimColor.value.x;
        float dh = h2 - h1;

        // -0.5〜0.5の範囲に収めて最短経路を取る
        if(dh > 0.5f) dh -= 1.0f;
        if(dh < -0.5f) dh += 1.0f;

        float newHue = h1 + dh * lerpRate;
        // 範囲を [0, 1) に戻す
        if(newHue < 0.0f) newHue += 1.0f;
        if(newHue >= 1.0f) newHue -= 1.0f;

        // 他の成分は普通に補間
        float newS = curColor.value.y + (aimColor.value.y - curColor.value.y) * lerpRate;
        float newV = curColor.value.z + (aimColor.value.z - curColor.value.z) * lerpRate;
        float newA = curColor.value.w + (aimColor.value.w - curColor.value.w) * lerpRate;

        Color nextColor(newHue, newS, newV, newA);
        backQuad.color = MyMath::HSV_to_RGB(nextColor.value);
    }

    SEED::DrawQuad2D(backQuad); // 背景Quadを描画
}

///////////////////////////////////////////////////////////////////////////////////
// リズムに乗って動くものの描画
///////////////////////////////////////////////////////////////////////////////////
void SelectBackGroundDrawer::DrawGrooveObjects(){

    struct GrooveObj{
        Vector2 position; // 位置
        float time; // 時間
        Quad2D quad; // 描画用のQuad
    };

    static float kGridSize = 60.0f;
    static std::vector<std::vector<GrooveObj>> grooveObjects;
    static std::list<Timer> grooveTimers;
    static float kGrooveTime = 0.6f; // グルーヴの時間
    static float kVisivleTimeRate = 0.4f;
    static bool isInitialized = false;

    // 初期化がまだならグリッドの位置を計算
    if(!isInitialized){
        Vector2 center = kWindowCenter;

        // 格納可能な縦横数を計算
        int rows = int((kWindowSize.y * 0.5f) / kGridSize) * 2 + 1;
        int cols = int((kWindowSize.x * 0.5f) / kGridSize) * 2 + 3;
        grooveObjects.resize(rows);

        // グリッドの位置を計算して格納
        for(int i = 0; i < rows; i++){
            for(int j = 0; j < cols; j++){
                // グリッドの位置を計算
                GrooveObj obj;
                obj.position = center + Vector2((j - cols / 2) * kGridSize, (i - rows / 2) * kGridSize);
                obj.time = (MyMath::Length(center - obj.position) / (kWindowSize.x * 1.0f)) * kGrooveTime; // 時間を計算
                obj.quad = MakeQuad2D(kGridSize);
                obj.quad.translate = obj.position;
                obj.quad.GH = TextureManager::LoadTexture("DefaultAssets/ellipse.png");
                obj.quad.layer = 2;
                obj.quad.drawLocation = DrawLocation::Back;
                obj.quad.blendMode = BlendMode::ADD;
                grooveObjects[i].push_back(obj);
            }
        }

        isInitialized = true; // 初期化フラグを立てる
    }

    // グルーブフラグがあればタイマーを追加
    if(isGrooveStart_){
        grooveTimers.emplace_back(Timer(kGrooveTime));
    }

    // グルーブオブジェクトの更新
    float timeRange = kGrooveTime * kVisivleTimeRate * 0.5f;
    for(auto& timer : grooveTimers){
        for(int i = 0; i < grooveObjects.size(); i++){
            for(int j = 0; j < grooveObjects[i].size(); j++){

                float timeDist = std::fabsf(timer.currentTime - grooveObjects[i][j].time);

                // 描画範囲外
                if(timeDist > timeRange){
                    continue;
                }

                // パラメータの更新
                float t = 1.0f - 1.0f * (timeDist / timeRange);
                float ease = EaseOutQuad(t);
                grooveObjects[i][j].quad.scale = { ease,ease };
                grooveObjects[i][j].quad.color = MyMath::HSV_to_RGB(grooveObjects[i][j].time / (kGrooveTime * 0.2f), 1.0f, 1.0f, 0.1f);

                // 描画
                SEED::DrawQuad2D(grooveObjects[i][j].quad);
            }
        }
    }


    // グルーヴのタイマーを更新
    for(auto& timer : grooveTimers){
        timer.Update();
    }

    // 終了したタイマーの削除
    grooveTimers.remove_if([](const Timer& timer){
        return timer.IsFinished();
    });

}
