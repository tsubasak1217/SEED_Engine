#include "AnimCurve2DComponent.h"
#include <SEED/Source/SEED.h>

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
// コンストラクタ
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
AnimCurve2DComponent::AnimCurve2DComponent(GameObject2D* pOwner, const std::string& tagName)
    : IComponent(pOwner, tagName){

    if(tagName == ""){
        componentTag_ = "AnimCurve2D_ID:" + std::to_string(componentID_);
    }

    if(!isTextureLoaded_){
        textureIDs_["playIcon"] = TextureManager::GetImGuiTexture("../../SEED/EngineResources/Textures/play2.png");
        textureIDs_["pauseIcon"] = TextureManager::GetImGuiTexture("../../SEED/EngineResources/Textures/play.png");
        isTextureLoaded_ = true;
    }

    // タイマー初期化
    timer_.Initialize(1.0f);

#ifdef _DEBUG
    // spriteの初期化
    isDebugItemVisible_ = true;
    debugPointSprite_ = Sprite("DefaultAssets/symmetryORE1.png");
    debugPointSprite_.size = { 64.0f,64.0f };
    debugPointSprite_.anchorPoint = { 0.5f,0.5f };
    debugPointSprite_.layer = -100;
#endif // _DEBUG
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
// フレーム開始時処理
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
void AnimCurve2DComponent::BeginFrame(){
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 更新処理
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
void AnimCurve2DComponent::Update(){

    if(controlPoints_.size() >= 2){
        // 補間方法に応じて制御点の補間
        Transform2D result;
        float t = timer_.GetEase(easingType_);
        switch(interpolationType_){
        case InterpolationType::LINEAR:
        {
            result = MyFunc::Interpolate(controlPoints_, t);
            break;
        }
        case InterpolationType::CATMULLROM:
        {
            result = MyFunc::CatmullRomInterpolate(controlPoints_, t);
            break;
        }
        default:
            break;
        }

        // オーナーに反映
        owner_.owner2D->SetWorldScale(result.scale);
        owner_.owner2D->SetWorldRotate(result.rotate);
        owner_.owner2D->SetWorldTranslate(result.translate);
    }

    // 再生中なら更新
    if(isPlaying_){
        // タイマー更新
        timer_.Update();

        // 必要であればループ処理
        if(timer_.IsFinished()){
            if(isLoop_){
                timer_.Reset();
                timer_.currentTime = timer_.overtime;
            }
        }
    }
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 描画処理
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
void AnimCurve2DComponent::Draw(){
    // デバッグ用に制御点と補間曲線を描画
#ifdef _DEBUG

    // 条件を満たしていなければ描画しない
    if(!isDebugItemVisible_){ return; }

    // 制御点にデバッグ用のスプライトを描画
    for(auto& point : controlPoints_){
        debugPointSprite_.transform = point;
        SEED::DrawSprite(debugPointSprite_);
    }

    // 制御点が2つ未満ならライン描画しない
    if(controlPoints_.size() < 2){ return; }
    
    // 制御点の座標リストを作成
    std::vector<Vector2> points;
    for(auto& point : controlPoints_){
        points.push_back(point.translate);
    }

    // ラインを描画
    switch(interpolationType_){
    case InterpolationType::LINEAR:
        for(int i = 0; i < controlPoints_.size() - 1; i++){
            SEED::DrawLine2D(controlPoints_[i].translate, controlPoints_[i + 1].translate, { 0.0f,0.0f,1.0f,1.0f });
        }
        break;
    case InterpolationType::CATMULLROM:
        SEED::DrawSpline(points, 8, { 0.0f,0.0f,1.0f,1.0f });
        break;
    default:
        break;
    }
#endif
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
// フレーム終了時処理
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
void AnimCurve2DComponent::EndFrame(){
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 初期化・終了処理
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
void AnimCurve2DComponent::Finalize(){
}


//////////////////////////////////////////////////////////////////////////////////////////////////////////////
// GUI編集
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
void AnimCurve2DComponent::EditGUI(){
#ifdef _DEBUG

    ImGui::Indent();
    static std::string label;
    std::vector<bool>removeFlags;

    // 各制御点の編集
    ImGui::SeparatorText("制御点一覧");
    for(int32_t i = 0; i < controlPoints_.size(); i++){

        bool removeFlag = false;
        label = "ControlPoint" + std::to_string(i);
        if(ImGui::CollapsingHeader(label.c_str())){
            ImGui::Indent();

            // ポインタをUINTに変換して一意なIDを生成
            std::string ptrStr = std::to_string(reinterpret_cast<uintptr_t>(&controlPoints_[i]));

            // トランスフォームの編集
            ImGui::SeparatorText("トランスフォーム");
            label = "座標##" + ptrStr;
            ImGui::DragFloat2(label.c_str(), &controlPoints_[i].translate.x);
            label = "回転##" + ptrStr;
            ImGui::DragFloat(label.c_str(), &controlPoints_[i].rotate, 0.05f);
            label = "スケール##" + ptrStr;
            ImGui::DragFloat2(label.c_str(), &controlPoints_[i].scale.x, 0.05f);

            // 削除ボタン
            ImGui::SeparatorText("削除");
            label = "削除##" + ptrStr;
            if(ImGui::Button(label.c_str())){
                removeFlag = true;
            }

            ImGui::Unindent();
        }

        // 削除フラグを保存
        removeFlags.push_back(removeFlag);
    }


    // 制御点追加ボタン
    ImGui::SeparatorText("制御点の追加");
    if(ImGui::Button("追加")){
        // 追加する制御点の初期値を設定
        Transform2D newPoint;
        if(controlPoints_.empty()){
            newPoint = owner_.owner2D->GetWorldTransform();
        } else{
            newPoint = controlPoints_.back();
        }
        // 追加
        controlPoints_.push_back(newPoint);
    }

    // 設定
    ImGui::SeparatorText("設定");
    ImGui::DragFloat("総時間", &timer_.duration, 0.1f, 0.0f, 1000.0f);
    if(isPlaying_){
        if(ImGui::ImageButton("再生ボタン", textureIDs_["playIcon"], { 20,20 })){
            Pause();
        }
    } else{
        if(ImGui::ImageButton("再生ボタン", textureIDs_["pauseIcon"], { 20,20 })){
            if(timer_.IsFinished()){
                timer_.Reset();
            }
            Play();
        }
    }
    ImGui::SameLine();
    ImGui::SliderFloat("現在の時間", &timer_.currentTime, 0.0f, timer_.duration);
    ImFunc::Combo("補間方法", interpolationType_, { "LINEAR","CATMULLROM" });
    ImFunc::Combo("イージング補間関数", easingType_, Easing::names, IM_ARRAYSIZE(Easing::names));
    ImGui::Checkbox("デフォルトで静止するか", &defaultPaused_);
    ImGui::Checkbox("ループするか", &isLoop_);
    ImGui::Checkbox("ラインのデバッグ表示", &isDebugItemVisible_);

    ImGui::Unindent();

    // 削除フラグが立っている制御点を削除
    for(int32_t i = (int32_t)removeFlags.size() - 1; i >= 0; i--){
        if(removeFlags[i]){
            controlPoints_.erase(controlPoints_.begin() + i);
        }
    }

    // コントロールポイントをGizmoに登録
    for(auto& point : controlPoints_){
        ImGuiManager::RegisterGuizmoItem(&point);
    }

#endif // _DEBUG
}


//////////////////////////////////////////////////////////////////////////////////////////////////////////////
// JSON読み込み・書き出し
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
void AnimCurve2DComponent::LoadFromJson(const nlohmann::json& jsonData){
    IComponent::LoadFromJson(jsonData);

    // 制御点の読み込み
    for(const auto& point : jsonData["controlPoints"]){
        controlPoints_.push_back(point);
    }

    // その他設定の読み込み
    interpolationType_ = static_cast<InterpolationType>(jsonData.value("interpolationType", 0));
    easingType_ = static_cast<Easing::Type>(jsonData.value("easingType", 0));
    isLoop_ = jsonData.value("isLoop", false);
    defaultPaused_ = jsonData.value("defaultPaused", false);
    timer_.duration = jsonData.value("duration", 1.0f);
    timer_.Reset();

    // 再生状態の設定
    if(!defaultPaused_){
        isPlaying_ = true;
    }
}

nlohmann::json AnimCurve2DComponent::GetJsonData() const{
    nlohmann::json jsonData;
    jsonData["componentType"] = "AnimCurve2D";
    jsonData.update(IComponent::GetJsonData());

    // 制御点
    for(const auto& point : controlPoints_){
        jsonData["controlPoints"].push_back(point);
    }

    // その他設定
    jsonData["interpolationType"] = static_cast<int>(interpolationType_);
    jsonData["easingType"] = static_cast<int>(easingType_);
    jsonData["isLoop"] = isLoop_;
    jsonData["defaultPaused"] = defaultPaused_;
    jsonData["duration"] = timer_.duration;

    return jsonData;
}
