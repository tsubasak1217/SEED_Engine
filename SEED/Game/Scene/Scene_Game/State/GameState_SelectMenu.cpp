#include "GameState_SelectMenu.h"
#include <SEED/Source/Basic/Scene/Scene_Base.h>
#include <Game/Config/PlaySettings.h>

///////////////////////////////////////////////////////////////////////////////
// コンストラクタ・デストラクタ
///////////////////////////////////////////////////////////////////////////////
GameState_SelectMenu::GameState_SelectMenu(){
}

GameState_SelectMenu::GameState_SelectMenu(Scene_Base* pScene){
    pScene_ = pScene;
}

GameState_SelectMenu::~GameState_SelectMenu(){
}

///////////////////////////////////////////////////////////////////////////////
// 初期化・終了処理
///////////////////////////////////////////////////////////////////////////////
void GameState_SelectMenu::Initialize(){
    // ポーズUIの読み込み
    menuItemsParent_ = pScene_->GetHierarchy()->LoadObject2D("SelectScene/menuUIs.prefab");

    // メニュー項目
    menus_[Option] = menuItemsParent_->GetChild("optionUI");
    menus_[BackToTitle] = menuItemsParent_->GetChild("backTitleUI");

    // ポーズ背景
    backSpriteObj_ = menuItemsParent_->GetChild("bg");

    // タイマー
    menuTransitionTimer_.Initialize(1.0f);
    optionPageTimer_.Initialize(0.5f);

    // スケーリング用タイマー&色初期化
    for(int32_t i = 0; i < menuItemScalingTimers_.size(); i++){
        // 選択中の項目だけ処理を変える
        if(i != selectedIndex_){
            // 通常初期化
            menuItemScalingTimers_[i].Initialize(0.2f);
            menus_[i]->masterColor_ = Color(0.4f, 0.4f, 0.4f, 1.0f);
        } else{
            // デフォルトでスケーリングを完了させておく
            menuItemScalingTimers_[i].Initialize(0.2f, 0.2f);
            menus_[i]->masterColor_ = Color(1.0f, 1.0f, 1.0f, 1.0f);
        }
    }
}

void GameState_SelectMenu::Finalize(){
}

///////////////////////////////////////////////////////////////////////////////
// 更新処理
///////////////////////////////////////////////////////////////////////////////
void GameState_SelectMenu::Update(){

    // メニュー項目選択
    SelectMenuItem();

    // メニュー項目スケーリング用のタイマー更新
    for(int32_t i = 0; i < menuItemScalingTimers_.size(); i++){

        // 選択中かどうかで更新処理を変える
        if(i != selectedIndex_){
            menuItemScalingTimers_[i].Update(-1.0f);
        } else{
            menuItemScalingTimers_[i].Update(1.0f);
        }

        // スケーリング反映
        float scale = 0.9f + 0.1f * menuItemScalingTimers_[i].GetEase(Easing::OutBack);
        menus_[i]->aditionalTransform_.scale = Vector2(scale, scale);
    }

    // ヘルプテキスト更新
    UpdateHelpTexts();

    // ポーズタイマーの更新
    if(isExit_){
        // ポーズ解除中
        menuTransitionTimer_.Update(-1.0f);
    } else{
        // ポーズ中
        menuTransitionTimer_.Update(1.0f);
    }


    // optionページ更新
    if(isInOptionPage_){
        optionPageTimer_.Update(1.0f);
        UpdateOptionItems();

    } else{
        optionPageTimer_.Update(-1.0f);
    }

    // 追加変形反映
    ApplyAdditionalTransform();
}


///////////////////////////////////////////////////////////////////////////////
// 描画処理
///////////////////////////////////////////////////////////////////////////////
void GameState_SelectMenu::Draw(){
}

///////////////////////////////////////////////////////////////////////////////
// フレーム開始時処理
///////////////////////////////////////////////////////////////////////////////
void GameState_SelectMenu::BeginFrame(){
}


///////////////////////////////////////////////////////////////////////////////
// フレーム終了時処理
///////////////////////////////////////////////////////////////////////////////
void GameState_SelectMenu::EndFrame(){
}


///////////////////////////////////////////////////////////////////////////////
// コライダーを渡す処理
///////////////////////////////////////////////////////////////////////////////
void GameState_SelectMenu::HandOverColliders(){
}


//////////////////////////////////////////////////////////////////////////////
// ステート管理処理
//////////////////////////////////////////////////////////////////////////////
void GameState_SelectMenu::ManageState(){
    // ポーズ中
    if(!isExit_){
        if(menuTransitionTimer_.GetProgress() > 0.5f){
            if(Input::IsTriggerKey(DIK_ESCAPE)){

                if(!isInOptionPage_){
                    isExit_ = true;
                    // 透明にしていく
                    backSpriteObj_->GetComponent<ColorControlComponent>()->RevercePlay();

                    // メニュー項目も逆再生で画面外へ
                    for(int32_t i = 0; i < menus_.size(); i++){
                        menus_[i]->GetComponent<Routine2DComponent>()->RevercePlay();
                    }
                } else{
                    // optionページを閉じる
                    isInOptionPage_ = false;
                    ReleaseOptionItems();
                }

                return;
            }
        }

        if(!isInOptionPage_){
            // スペースキーで決定
            if(Input::IsTriggerKey(DIK_SPACE)){

                // 選択項目に応じた処理
                switch(selectedIndex_){
                case Option:// 設定画面を表示
                    isInOptionPage_ = true;
                    LoadOptionItems();
                    break;

                case BackToTitle:// タイトルに戻る
                    pScene_->ChangeScene("Title");
                    return;

                default:
                    break;
                }
            }
        }


    } else{// ポーズ終了中

        // タイマーが0になったら
        if(menuTransitionTimer_.GetProgress() == 0.0f){
            // 再びゲームに戻る
            ReleaseOptionItems();
            pScene_->EraseFromHierarchy(menuItemsParent_);
            pScene_->EndEvent();
        }
    }
}


//////////////////////////////////////////////////////////////////////////////
// メニュー項目選択
//////////////////////////////////////////////////////////////////////////////
void GameState_SelectMenu::SelectMenuItem(){

    // 上下の入力取得
    int inputDir = Input::IsTriggerKey({ DIK_S,DIK_D,DIK_DOWN,DIK_RIGHT }) - Input::IsTriggerKey({ DIK_W,DIK_A,DIK_UP,DIK_LEFT });

    // 選択インデックス更新
    selectedIndex_ += inputDir;
    selectedIndex_ = MyFunc::Spiral(selectedIndex_, 0, SelectMenuItemCount - 1);

    if(inputDir != 0){
        // 選択中の項目だけ色を変える
        for(int32_t i = 0; i < menus_.size(); i++){

            if(i != selectedIndex_){
                menus_[i]->masterColor_ = Color(0.4f, 0.4f, 0.4f, 1.0f);
            } else{
                menus_[i]->masterColor_ = Color(1.0f, 1.0f, 1.0f, 1.0f);
            }
        }
    }
}

//////////////////////////////////////////////////////////////////////////////
// ヘルプテキスト更新(メニューアイコンの横の補足説明)
//////////////////////////////////////////////////////////////////////////////
void GameState_SelectMenu::UpdateHelpTexts(){

    for(int32_t i = 0; i < menus_.size(); i++){
        TextBox2D& helpText = menus_[i]->GetComponent<UIComponent>()->GetText(0);
        Sprite& underLineSprite = menus_[i]->GetComponent<UIComponent>()->GetSprite(1);

        // 選択中
        helpText.textDisplayRate = menuItemScalingTimers_[i].GetProgress();
        underLineSprite.color.value.w = menuItemScalingTimers_[i].GetProgress();
    }
}


//////////////////////////////////////////////////////////////////////////////
// スライダー値初期化
//////////////////////////////////////////////////////////////////////////////
void GameState_SelectMenu::InitializeSliderValues(){

    // スライダーの値の範囲設定
    sliderValueRanges_[NotesSpeed] = Range1D(0.5f, 2.0f);
    sliderValueRanges_[JudgeOffset] = Range1D(-0.3f, 0.3f);
    sliderValueRanges_[DisplayOffset] = Range1D(-0.3f, 0.3f);
    sliderValueRanges_[AnswerOffset] = Range1D(-0.3f, 0.3f);
    sliderValueRanges_[MouseSensitivity] = Range1D(0.5f, 2.0f);

    // 現在の値を取得
    sliderValues_[NotesSpeed] = PlaySettings::GetInstance()->GetNoteSpeedRate();
    sliderValues_[JudgeOffset] = PlaySettings::GetInstance()->GetOffsetJudge();
    sliderValues_[DisplayOffset] = PlaySettings::GetInstance()->GetOffsetView();
    sliderValues_[AnswerOffset] = PlaySettings::GetInstance()->GetOffsetAnswerSE();
    sliderValues_[MouseSensitivity] = PlaySettings::GetInstance()->GetCursorSenstivity();

    // スライダーのt値を設定
    for(int32_t i = 0; i < sliderValues_.size(); i++){
        float minValue = sliderValueRanges_[(OptionPageItem)i].min;
        float maxValue = sliderValueRanges_[(OptionPageItem)i].max;
        slidersT_[i] = (sliderValues_[i] - minValue) / (maxValue - minValue);
        slidersT_[i] = std::clamp(slidersT_[i], 0.0f, 1.0f);
    }

    // タイマー初期値設定
    sliderScalingTimer_.Reset();
    for(int32_t i = 0; i < optionItemTimers_.size(); i++){
        // 選択中の項目だけタイマーを進めておき、他はリセット
        if(i == currentOptionItem_){
            optionItemTimers_[i].ToEnd();
        } else{
            optionItemTimers_[i].Reset();
        }
    }
}

//////////////////////////////////////////////////////////////////////////////
// option関連
//////////////////////////////////////////////////////////////////////////////
void GameState_SelectMenu::LoadOptionItems(){

    // 初期選択項目をノーツ速度に設定
    currentOptionItem_ = NotesSpeed;

    // すでに読み込んでいるなら処理しない
    if(isLoadOptionItems_){
        optionTexts_[DetailText]->text = sliderDetailTexts_[currentOptionItem_];
        InitializeSliderValues();
        UpdateHeaders();
        UpdateSlider();
        return;
    }

    optionPageParent_ = pScene_->GetHierarchy()->LoadObject2D("SelectScene/optionPage.prefab");
    optionPageParent_->aditionalTransform_.scale = Vector2(0.0f);
    optionPageParent_->UpdateMatrix();

    // オプション項目
    optionItems_[NotesSpeed] = optionPageParent_->GetChild("notesSpeed");
    optionItems_[JudgeOffset] = optionPageParent_->GetChild("judgeOffset");
    optionItems_[DisplayOffset] = optionPageParent_->GetChild("displayOffset");
    optionItems_[AnswerOffset] = optionPageParent_->GetChild("answerOffset");
    optionItems_[MouseSensitivity] = optionPageParent_->GetChild("mouseSensitivity");
    optionItems_[OkButton] = optionPageParent_->GetChild("ok");

    // スライダー関連
    sliderObj_ = optionPageParent_->GetChild("slider");
    sliderSprites_[LeftPoint] = &sliderObj_->GetComponent<UIComponent>()->GetSprite("left");
    sliderSprites_[MidPoint] = &sliderObj_->GetComponent<UIComponent>()->GetSprite("mid");
    sliderSprites_[RightPoint] = &sliderObj_->GetComponent<UIComponent>()->GetSprite("right");
    sliderSprites_[Body] = &sliderObj_->GetComponent<UIComponent>()->GetSprite("body");

    // テキスト関連
    sliderDetailTexts_[NotesSpeed] = "ノーツの動く速さを調整します。";
    sliderDetailTexts_[JudgeOffset] = "判定のタイミングを調整します。\n音や見た目は合っているけど\n判定がずれて感じる人におすすめ!";
    sliderDetailTexts_[DisplayOffset] = "ノーツの表示タイミングを調整します。\n判定も見た目に連動して動きます。";
    sliderDetailTexts_[AnswerOffset] = "アンサー音の再生タイミングを調整します。\nノーツの音が見た目とずれて聞こえる人におすすめ!\n(判定はそのままです)";
    sliderDetailTexts_[MouseSensitivity] = "マウスの感度を調整します。\nマウス操作がしづらいと感じる人におすすめ!";

    optionTexts_[ValueText] = &sliderObj_->GetComponent<UIComponent>()->GetText(0);
    optionTexts_[DetailText] = &optionPageParent_->GetChild("detail")->GetComponent<UIComponent>()->GetText(0);
    optionTexts_[DetailText]->text = sliderDetailTexts_[currentOptionItem_];

    // コライダー関連
    mouseColliderObj_ = pScene_->GetHierarchy()->LoadObject2D("SelectScene/cursorColliderObj.prefab");
    sliderColliderObj_ = optionPageParent_->GetChild("sliderColliderObj");

    // タイマー
    sliderScalingTimer_.Initialize(0.2f);
    for(int32_t i = 0; i < optionItems_.size(); i++){
        optionItemTimers_[i].Initialize(0.2f);
        optionItemHoverTimers_[i].Initialize(0.2f);
    }

    // スライダー値初期化
    InitializeSliderValues();

    // ヘッダー更新
    UpdateHeaders();

    // 読み込み完了フラグON
    isLoadOptionItems_ = true;
}


///////////////////////////////////////////////////////////////////////////////
// オプション項目解放
///////////////////////////////////////////////////////////////////////////////
void GameState_SelectMenu::ReleaseOptionItems(){
    auto* hierarchy = pScene_->GetHierarchy();
    hierarchy->EraseObject(optionPageParent_);
    hierarchy->EraseObject(mouseColliderObj_);
}


///////////////////////////////////////////////////////////////////////////////
// オプション項目更新
///////////////////////////////////////////////////////////////////////////////
void GameState_SelectMenu::UpdateOptionItems(){

    // カーソル位置更新
    Vector2 mousePos = Input::GetMousePosition();
    mouseColliderObj_->SetWorldTranslate(mousePos);
    mouseColliderObj_->UpdateMatrix();

    // 押している項目の確認
    CheckPress();

    // ヘッダー更新
    UpdateHeaders();

    // スライダー更新
    UpdateSlider();

    // テキストを時間に応じて出現させる
    if(currentOptionItem_ != OkButton){
        optionTexts_[DetailText]->textDisplayRate = optionItemTimers_[currentOptionItem_].GetProgress();
    }
}


///////////////////////////////////////////////////////////////////////////////
// オプション項目の押下確認
///////////////////////////////////////////////////////////////////////////////
void GameState_SelectMenu::CheckPress(){

    // optionページが表示されていないなら処理しない
    if(optionPageTimer_.GetProgress() != 1.0f){
        return;
    }

    for(int32_t i = 0; i < optionItems_.size(); i++){

        // カーソルがコライダー範囲内かつクリックされたら
        if(optionItems_[i]->GetIsCollideAny()){
            if(Input::IsTriggerMouse(MOUSE_BUTTON::LEFT)){
                if((OptionPageItem)i != currentOptionItem_){

                    // 選択項目を更新
                    currentOptionItem_ = (OptionPageItem)i;

                    // 文字列の更新
                    if(i < OkButton){
                        optionTexts_[DetailText]->text = sliderDetailTexts_[currentOptionItem_];

                    } else{// OKボタンを押した場合
                        isInOptionPage_ = false;
                    }
                }
            }

            // ホバータイマーを加算
            optionItemHoverTimers_[i].Update(1.0f);
        } else{
            // ホバータイマーを減らす
            optionItemHoverTimers_[i].Update(-1.0f);
        }
    }

    // 選択状況に応じてタイマーを更新
    for(int32_t i = 0; i < optionItems_.size(); i++){
        if(i == (int)currentOptionItem_){
            optionItemTimers_[i].Update();
        } else{
            optionItemTimers_[i].Update(-1.0f);
        }
    }

}

///////////////////////////////////////////////////////////////////////////////
// ヘッダー(オプション名など)の更新
///////////////////////////////////////////////////////////////////////////////
void GameState_SelectMenu::UpdateHeaders(){
    for(int32_t i = 0; i < optionItems_.size(); i++){

        if(i != OkButton){
            TextBox2D& headerText = optionItems_[i]->GetComponent<UIComponent>()->GetText(0);
            float ease = optionItemTimers_[i].GetEase(Easing::OutBack);
            float hoverEase = optionItemHoverTimers_[i].GetEase(Easing::OutBack);
            float baseScale = 0.8f + (0.2f * ease) + (0.1f * hoverEase);

            // 色とスケール更新
            if(i == (int)currentOptionItem_){
                // 選択中のものは黄色っぽく
                headerText.color = Color(1.0f, 1.0f, 0.0f, 0.1f + (0.9f + ease));
            } else{
                // 選択されていないものは灰色
                headerText.color = Color(1.0f, 1.0f, 1.0f, 0.1f + (0.9f + ease));
            }

            optionItems_[i]->aditionalTransform_.scale = Vector2(baseScale);

        } else{
            // okButtonはスケールのみ
            float ease = optionItemTimers_[i].GetEase(Easing::OutBack);
            float hoverEase = optionItemHoverTimers_[i].GetEase(Easing::OutBack);
            float baseScale = 0.8f + (0.2f * ease) + (0.1f * hoverEase);
            optionItems_[i]->aditionalTransform_.scale = Vector2(baseScale);

        }

        // 行列更新
        optionItems_[i]->UpdateMatrix();
    }
}

///////////////////////////////////////////////////////////////////////////////
// スライダーの更新
///////////////////////////////////////////////////////////////////////////////
void GameState_SelectMenu::UpdateSlider(){

    // okNuttonにスライダーは存在しない
    if(currentOptionItem_ == OkButton){
        return;
    }

    // スライダー左右座標の更新
    sliderLeftPos_ = sliderObj_->GetWorldTranslate() - Vector2(sliderSprites_[Body]->size.x, 0.0f);
    sliderRightPos_ = sliderObj_->GetWorldTranslate() + Vector2(sliderSprites_[Body]->size.x, 0.0f);

    // スライダーをドラッグ中かどうか
    if(Input::IsPressMouse(MOUSE_BUTTON::LEFT)){
        if(sliderColliderObj_->GetIsCollideAny()){
            if(Input::IsTriggerMouse(MOUSE_BUTTON::LEFT)){
                isDraggingSlider_ = true;
            }
        }

    } else{// マウスボタンが離されたらドラッグ終了
        isDraggingSlider_ = false;
    }

    // スライダーをドラッグ中なら
    if(isDraggingSlider_){
        // マウス位置に応じてtを更新
        Vector2 mousePos = Input::GetMousePosition();
        float t = (mousePos.x - sliderLeftPos_.x) / (sliderRightPos_.x - sliderLeftPos_.x);
        t = std::clamp(t, 0.0f, 1.0f);

        // スライダー値更新
        slidersT_[(int)currentOptionItem_] = t;
    }

    // スケール用タイマー更新
    if(sliderColliderObj_->GetIsCollideAny() or isDraggingSlider_){
        sliderScalingTimer_.Update(1.0f);
    } else{
        sliderScalingTimer_.Update(-1.0f);
    }

    // 状態に応じて見た目を更新
    float scale = 1.0f + 0.2f * sliderScalingTimer_.GetEase(Easing::OutBack);
    sliderColliderObj_->aditionalTransform_.scale = Vector2(scale);
    sliderColliderObj_->UpdateMatrix();

    // スライダー本体の長さ更新
    sliderSprites_[Body]->transform.scale.x = (slidersT_[(int)currentOptionItem_] - 0.5f) / 0.5f;


    // ノーツ速度とマウス感度は中央のポイントおよびbodyを非表示にする
    if(currentOptionItem_ == NotesSpeed or currentOptionItem_ == MouseSensitivity){
        sliderSprites_[Body]->color.value.w = 0.0f;
        sliderSprites_[MidPoint]->color.value.w = 0.0f;
    } else{
        // スライダー本体の色更新
        if(sliderSprites_[Body]->transform.scale.x > 0.0f){
            sliderSprites_[Body]->color = Color(0.0f, 1.0f, 0.0f, 1.0f);
        } else{
            sliderSprites_[Body]->color = Color(1.0f, 0.0f, 0.0f, 1.0f);
        }
    }

    // スライダー値を反映
    ApplyOptionSettings();

    // スライダー値テキスト更新(小数点2桁まで)
    ApplyToValueText();
}


///////////////////////////////////////////////////////////////////////////////
// 追加変形適用
///////////////////////////////////////////////////////////////////////////////
void GameState_SelectMenu::ApplyAdditionalTransform(){

    float ease = optionPageTimer_.GetEase(Easing::InOutExpo);
    for(int32_t i = 0; i < menus_.size(); i++){
        menus_[i]->aditionalTransform_.scale *= Vector2(1.0f - ease);
        menus_[i]->UpdateMatrix();
    }

    if(optionPageParent_){
        optionPageParent_->aditionalTransform_.scale = Vector2(ease);
        optionPageParent_->UpdateMatrix();
    }
}

///////////////////////////////////////////////////////////////////////////////
// オプション設定適用
///////////////////////////////////////////////////////////////////////////////
void GameState_SelectMenu::ApplyOptionSettings(){

    // 値を計算
    Range1D valueRange = sliderValueRanges_[(int)currentOptionItem_];
    float value = MyMath::Lerp(valueRange.min, valueRange.max, slidersT_[(int)currentOptionItem_]);

    // 小数点2桁までに丸める(正:切り捨て 負:切り上げ)
    if(value >= 0.0f){
        value = std::floor(value * 100.0f) / 100.0f;
    } else{
        value = std::ceil(value * 100.0f) / 100.0f;
        if(value == -0.0f){
            value = 0.0f;// -0.0fを防止
        }
    }

    sliderValues_[(int)currentOptionItem_] = value;

    // スライダー位置更新
    sliderColliderObj_->SetWorldTranslate(
        Vector2(
            MyMath::Lerp(sliderLeftPos_.x, sliderRightPos_.x, slidersT_[(int)currentOptionItem_]),
            sliderColliderObj_->GetWorldTranslate().y
        )
    );

    sliderColliderObj_->UpdateMatrix();


    // PlaySettingsに反映(小数点2桁まで)
    switch(currentOptionItem_){
    case NotesSpeed:
        PlaySettings::GetInstance()->SetNoteSpeedRate(value);
        break;
    case JudgeOffset:
        PlaySettings::GetInstance()->SetOffsetJudge(value);
        break;
    case DisplayOffset:
        PlaySettings::GetInstance()->SetOffsetView(value);
        break;
    case AnswerOffset:
        PlaySettings::GetInstance()->SetOffsetAnswerSE(value);
        break;
    case MouseSensitivity:
        PlaySettings::GetInstance()->SetCursorSenstivity(value);
        break;
    default:
        break;
    }
}

///////////////////////////////////////////////////////////////////////////////
// スライダー値をテキストに適用
///////////////////////////////////////////////////////////////////////////////
void GameState_SelectMenu::ApplyToValueText(){

    auto* t = optionTexts_[ValueText];
    float value = sliderValues_[(int)currentOptionItem_];
    t->text.clear();

    // 先頭に付与する文字列を設定
    if(currentOptionItem_ == NotesSpeed or currentOptionItem_ == MouseSensitivity){
        t->text = "x";

    } else{
        if(value > 0.0f){
            t->text = "+";
        }
    }

    // 小数点2桁までを末尾に追加
    std::string valueStr = std::to_string(value);
    valueStr = valueStr.substr(0, valueStr.find(".") + 3);
    t->text += valueStr;
}
