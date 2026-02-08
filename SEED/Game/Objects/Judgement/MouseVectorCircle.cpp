#include "MouseVectorCircle.h"
#include <Game/Objects/Judgement/PlayerInput.h>
#include <Environment/Environment.h>

//////////////////////////////////////////////////////////////////////////////
// 初期化
//////////////////////////////////////////////////////////////////////////////
void MouseVectorCircle::Initialize(){

    // オブジェクトの読み込み
    SEED::Hierarchy* hierarchy = SEED::GameSystem::GetScene()->GetHierarchy();
    circleObj_ = hierarchy->LoadObject2D("PlayScene/MouseVectorCircle.prefab");
    cursorObj_ = hierarchy->LoadObject2D("PlayScene/MouseVectorCursor.prefab");

    // 色の初期化
    circleObj_->masterColor_ = SEED::Color(0.0f, 1.0f, 0.25f, 1.0f);

    // 初期スケールの設定
    circleObj_->SetWorldScale(Vector2(0.0f));
    cursorObj_->SetWorldScale(Vector2(0.0f));
    circleObj_->UpdateMatrix();
    cursorObj_->UpdateMatrix();

    // スケーリング用タイマーの初期化
    scalingTimer_.Initialize(0.4f);
}

//////////////////////////////////////////////////////////////////////////////
// 更新
//////////////////////////////////////////////////////////////////////////////
void MouseVectorCircle::Update(){

    // 近くにㇾクトフリックノーツがあればスケーリングを開始する
    if(pNotesData_){
        float curTime = pNotesData_->GetCurMusicTime();
        if(pNotesData_->SearchNoteByTime(curTime, curTime + searchTimeWidth_, NoteType::RectFlick)){
            isScalingUp_ = true;
        } else{
            isScalingUp_ = false;
        }
    }

    // スケーリング用のタイマーの更新
    if(isScalingUp_){
        scalingTimer_.Update();
    } else{
        scalingTimer_.Update(-1.0f);
    }

    if(circleObj_){
        // スケーリングの適用
        float ease = scalingTimer_.GetEase(SEED::Methods::Easing::Type::Out_Back);
        circleObj_->SetWorldScale(Vector2(ease));

        // マウスベクトルと長さを取得
        Vector2 mouseVec = SEED::Input::GetMouseVector();
        float deadZone = PlayerInput::GetInstance()->GetFlickDeadZone();// 無視するデッドゾーンの大きさ
        float len = mouseVec.Length();

        // 長さに応じた処理
        if(len >= deadZone){
            // フリック判定の際は緑っぽい色に
            circleObj_->masterColor_ = SEED::Color(0.0f, 1.0f, 0.25f, 1.0f);

        } else{

            // 破棄デッドゾーン内ならベクトルを0にする
            if(len == 0.0f){
                mouseVec = prevMouseVec_;
            }

            // 黄色に徐々に戻す
            circleObj_->masterColor_.AddHue(-2.0f * SEED::ClockManager::DeltaTime(), true);
        }


        // カーソル位置更新
        if(cursorObj_){
            float lenRate = std::clamp(len / deadZone, 0.0f, 1.0f);
            Vector2 targetVec = mouseVec.Normalized() * radius * lenRate;

            // ラープ係数
            float lerpRate;
            if(len > deadZone){
                lerpRate = 20.0f * SEED::ClockManager::DeltaTime();
            } else{
                lerpRate = 5.0f * SEED::ClockManager::DeltaTime();
            }

            // 前フレームの位置からターゲットへ補間
            prevMouseVec_ = SEED::Methods::Math::Lerp(prevMouseVec_, targetVec, lerpRate);

            // カーソルの位置を更新
            cursorObj_->SetWorldTranslate(kWindowCenter + prevMouseVec_ * ease);

            // scaleも更新
            cursorObj_->SetWorldScale(Vector2(ease));
        }
    }

}

void MouseVectorCircle::SetNotesData(const NotesData* pNotesData){
    pNotesData_ = pNotesData;
}
