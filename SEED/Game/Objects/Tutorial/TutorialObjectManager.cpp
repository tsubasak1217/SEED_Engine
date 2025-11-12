#include "TutorialObjectManager.h"
#include <Game/GameSystem.h>
#include <Environment/Environment.h>

//////////////////////////////////////////////////////////////////////////////
// コンストラクタ・デストラクタ
//////////////////////////////////////////////////////////////////////////////
TutorialObjectManager::TutorialObjectManager(){
}

TutorialObjectManager::TutorialObjectManager(const Timer* pSongPlayTimer){
    pSongPlayTimer_ = pSongPlayTimer;
}

TutorialObjectManager::~TutorialObjectManager(){
}


//////////////////////////////////////////////////////////////////////////////
// 初期化処理
//////////////////////////////////////////////////////////////////////////////
void TutorialObjectManager::Initialize(){
    // ビデオファイルパスの設定
    tutorials_[TutorialName::Tap].tutorialVideoFilePath = "Tutorials/tap.mp4";
    tutorials_[TutorialName::Hold].tutorialVideoFilePath = "Tutorials/hold.mp4";
    tutorials_[TutorialName::Wheel].tutorialVideoFilePath = "Tutorials/wheel.mp4";
    tutorials_[TutorialName::Frame].tutorialVideoFilePath = "Tutorials/frame.mp4";

    // 開始時間の設定
    tutorials_[TutorialName::Tap].startTime = 4.0f;
    tutorials_[TutorialName::Hold].startTime = 30.0f;
    tutorials_[TutorialName::Wheel].startTime = 54.0f;
    tutorials_[TutorialName::Frame].startTime = 79.0f;

    // ビデオ表示時間の設定
    float tutorialDuration = 8.0f;
    tutorials_[TutorialName::Tap].tutorialTimer.Initialize(tutorialDuration + 2.0f);
    tutorials_[TutorialName::Hold].tutorialTimer.Initialize(tutorialDuration);
    tutorials_[TutorialName::Wheel].tutorialTimer.Initialize(tutorialDuration);
    tutorials_[TutorialName::Frame].tutorialTimer.Initialize(tutorialDuration);

    // テキストの設定
    tutorials_[TutorialName::Tap].text = "タップノーツ";
    tutorials_[TutorialName::Hold].text = "ホールドノーツ";
    tutorials_[TutorialName::Wheel].text = "ホイールノーツ";
    tutorials_[TutorialName::Frame].text = "フレームノーツ";

    // 説明テキストの設定
    tutorials_[TutorialName::Tap].explainText =
        "左のレーンから順番に\nA,S,D,F,Space\nマウスの左クリックでもOK!\n真ん中にノーツが重なるタイミングでボタンを押してみよう!";
    tutorials_[TutorialName::Hold].explainText =
        "ノーツの終端が通り過ぎるまでボタンを離さずに押し続けよう!";
    tutorials_[TutorialName::Wheel].explainText =
        "ノーツが真ん中に重なるタイミングで\nピンク色は奥、水色は手前にホイールを回そう！";
    tutorials_[TutorialName::Frame].explainText =
        "真ん中から出てくる黄色いノーツが画面端に重なるタイミングで\nマウスをノーツの方向に振ってみよう！";

    // ビデオの読み込み
    for(int i = 0; i < (int)TutorialName::kTutorialCount; i++){
        // ビデオプレイヤーの作成
        tutorials_[i].videoPlayer = std::make_unique<VideoPlayer>(tutorials_[i].tutorialVideoFilePath, false);

        // ビデオのサイズに応じてQuadを設定
        Vector2 videoSize = tutorials_[i].videoPlayer->GetVideoSize();
        float aspectRatio = videoSize.y / videoSize.x;

        // X値基準でQuadを作成(チュートリアル動画は横長なので)
        float videoWidth = kWindowSize.x * 0.8f; // 画面幅の80%に設定
        tutorials_[i].videoQuad = MakeQuad2D(Vector2(1.0f, aspectRatio) * videoWidth);
        tutorials_[i].videoQuad.translate = kWindowCenter; // 画面中央に配置
        tutorials_[i].videoQuad.lightingType = LIGHTINGTYPE_NONE; // ライティングを無効化
        tutorials_[i].videoQuad.isApplyViewMat = false; // ビュー行列を適用しない
        tutorials_[i].videoQuad.layer = 5; // 手前に描画
    }

    // 読み込み改善のために先に一度読み込んですぐ削除
    Hierarchy* hierarchy = GameSystem::GetScene()->GetHierarchy();
    hierarchy->EraseObject(hierarchy->LoadObject2D("PlayScene/Tutorial/tutorialText.prefab"));
    hierarchy->EraseObject(hierarchy->LoadObject2D("PlayScene/Tutorial/tutorialText2.prefab"));
}


//////////////////////////////////////////////////////////////////////////////
// 更新処理
//////////////////////////////////////////////////////////////////////////////
void TutorialObjectManager::Update(){

    // 曲再生タイマーが設定されていない場合は何もしない
    if(!pSongPlayTimer_){
        return;
    }

    // 時間を満たしているか確認
    for(int i = 0; i < (int)TutorialName::kTutorialCount; i++){

        // ビデオの矩形情報を更新
        tutorials_[i].videoQuad.scale = Vector2(tutorials_[i].videoScalingTimer.GetEase(Easing::OutBack));

        // 終了している場合はスキップ
        if(tutorials_[i].tutorialTimer.IsFinished()){
            tutorials_[i].isPlaying = false;
            tutorials_[i].videoScalingTimer.Update(-1.0f);

            if(tutorials_[i].videoScalingTimer.IsReturnNow()){
                Hierarchy* hierarchy = GameSystem::GetScene()->GetHierarchy();
                hierarchy->EraseObject(tutorials_[i].tutorialTitleObject);
                hierarchy->EraseObject(tutorials_[i].tutorialTextObject);
                tutorials_[i].videoPlayer->Unload();
            }

            continue;
        }

        if(tutorials_[i].isPlaying){
            // タイマーの更新
            tutorials_[i].tutorialTimer.Update();

            if(tutorials_[i].tutorialTimer.currentTime > TutorialData::scalingStartTime){

                if(tutorials_[i].videoScalingTimer.GetProgress() == 0.0f){
                    // 下部のテキストオブジェクトを読み込む
                    Hierarchy* hierarchy = GameSystem::GetScene()->GetHierarchy();
                    tutorials_[i].tutorialTextObject = hierarchy->LoadObject2D("PlayScene/Tutorial/tutorialText2.prefab");
                    tutorials_[i].tutorialTextObject->GetComponent<UIComponent>()->GetText(0).text = tutorials_[i].explainText;
                }


                // スケーリングタイマーの更新
                tutorials_[i].videoScalingTimer.Update();
            }


        } else{

            // 再生していない場合、開始時間になったら再生
            if(pSongPlayTimer_->currentTime >= tutorials_[i].startTime){
                tutorials_[i].videoPlayer->Play(0.0f,1.0f,true);
                tutorials_[i].isPlaying = true;

                // テキストオブジェクトを読み込む
                Hierarchy* hierarchy = GameSystem::GetScene()->GetHierarchy();
                tutorials_[i].tutorialTitleObject = hierarchy->LoadObject2D("PlayScene/Tutorial/tutorialText.prefab");
                GameObject2D* text = tutorials_[i].tutorialTitleObject->GetChild("Text");
                text->GetComponent<UIComponent>()->GetText(0).text = tutorials_[i].text;
                text->Update();
            }
        }
    }
}


//////////////////////////////////////////////////////////////////////////////
// 描画処理
//////////////////////////////////////////////////////////////////////////////
void TutorialObjectManager::Draw(){
    // 再生中のチュートリアルを描画
    for(int i = 0; i < (int)TutorialName::kTutorialCount; i++){
        if(tutorials_[i].tutorialTimer.GetProgress() != 0.0f){
            tutorials_[i].videoPlayer->Draw(tutorials_[i].videoQuad);
        }
    }
}


//////////////////////////////////////////////////////////////////////////////
// 終了処理
//////////////////////////////////////////////////////////////////////////////
void TutorialObjectManager::Finalize(){
}