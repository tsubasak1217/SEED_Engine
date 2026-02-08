#include "Routine2DComponent.h"
#include <SEED/Source/SEED.h>

namespace SEED{
    //////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // コンストラクタ
    //////////////////////////////////////////////////////////////////////////////////////////////////////////////
    Routine2DComponent::Routine2DComponent(GameObject2D* pOwner, const std::string& tagName)
        : IComponent(pOwner, tagName){

        if(tagName == ""){
            componentTag_ = "Routine2D_ID:" + std::to_string(componentID_);
        }

        // タイマー初期化
        timer_.Initialize(3.0f);

        // 制御点初期化(始点と終点)
        controlPoints_.emplace_back(pOwner->GetWorldTransform(), 0.0f);
        controlPoints_.emplace_back(pOwner->GetWorldTransform(), 1.0f);

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
    void Routine2DComponent::BeginFrame(){
    }

    //////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // 更新処理
    //////////////////////////////////////////////////////////////////////////////////////////////////////////////
    void Routine2DComponent::Update(){

        if(controlPoints_.size() >= 2){
            // 補間方法に応じて制御点の補間
            Transform2D result;
            float t = timer_.GetEase(easingType_);

            // どの区間にいるかを特定
            int section = 0;
            for(int i = 0; i < (int)controlPoints_.size() - 1; i++){
                if(t >= controlPoints_[i].second && t <= controlPoints_[i + 1].second){
                    section = i;
                    break;
                }
            }

            // 区間内の相対的t
            float sectionStart = controlPoints_[section].second;
            float sectionEnd = controlPoints_[section + 1].second;
            float sectionLength = sectionEnd - sectionStart;
            float sectionT = (t - sectionStart) / sectionLength;

            // この時点で全体のどの位置にいるかを計算
            float timePerSection = 1.0f / float(controlPoints_.size() - 1);
            float uniformT = (section * timePerSection) + (sectionT * timePerSection);
            uniformT = std::clamp(uniformT, 0.0f, 1.0f);

            // transformを一時配列に入れる
            std::vector<Transform2D> transforms;
            for(auto& point : controlPoints_){
                transforms.emplace_back(point.first);
            }

            switch(interpolationType_){
            case Enums::InterpolationType::LINEAR:
            {
                result = Methods::SRT::Interpolate(transforms, uniformT);
                break;
            }
            case Enums::InterpolationType::CATMULLROM:
            {
                result = Methods::SRT::CatmullRomInterpolate(transforms, uniformT, isConnectEdge_);
                break;
            }
            default:
                break;
            }


            // オーナーに反映
            if(!disableScale_){
                owner_.owner2D->SetWorldScale(result.scale);
            }
            if(!disableRotate_){
                owner_.owner2D->SetWorldRotate(result.rotate);
            }
            if(!disableTranslate_){
                owner_.owner2D->SetWorldTranslate(result.translate);
            }
        }

        // 再生中なら更新
        if(isPlaying_){
            // タイマー更新
            timer_.Update(timeScale_, isLoop_);
        }
    }

    //////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // 描画処理
    //////////////////////////////////////////////////////////////////////////////////////////////////////////////
    void Routine2DComponent::Draw(){
        // デバッグ用に制御点と補間曲線を描画
    #ifdef _DEBUG

        // 条件を満たしていなければ描画しない
        if(!isDebugItemVisible_){ return; }
        if(!isEditting_){ return; }

        // 制御点にデバッグ用のスプライトを描画
        for(auto& point : controlPoints_){
            debugPointSprite_.transform = point.first;
            SEED::Instance::DrawSprite(debugPointSprite_);
        }

        // 制御点が2つ未満ならライン描画しない
        if(controlPoints_.size() < 2){ return; }

        // 制御点の座標リストを作成
        std::vector<Vector2> points;
        for(auto& point : controlPoints_){
            points.push_back(point.first.translate);
        }

        // ラインを描画
        switch(interpolationType_){
        case Enums::InterpolationType::LINEAR:
            for(int i = 0; i < controlPoints_.size() - 1; i++){
                SEED::Instance::DrawLine2D(controlPoints_[i].first.translate, controlPoints_[i + 1].first.translate, { 0.0f,0.0f,1.0f,1.0f });
            }
            break;
        case Enums::InterpolationType::CATMULLROM:
            SEED::Instance::DrawSpline(points, 8, { 0.0f,0.0f,1.0f,1.0f });
            break;
        default:
            break;
        }

        // falseに初期化
        isEditting_ = false;

    #endif
    }

    //////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // フレーム終了時処理
    //////////////////////////////////////////////////////////////////////////////////////////////////////////////
    void Routine2DComponent::EndFrame(){
    }

    //////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // 初期化・終了処理
    //////////////////////////////////////////////////////////////////////////////////////////////////////////////
    void Routine2DComponent::Finalize(){
    }


    //////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // GUI編集
    //////////////////////////////////////////////////////////////////////////////////////////////////////////////
    void Routine2DComponent::EditGUI(){
    #ifdef _DEBUG
        // 編集を開始
        isEditting_ = true;
        ImGui::Indent();

        // タイムライン表示
        TimelineView();

        // 制御点のドラッグ
        DragPoint();

        // 制御点のトランスフォームの編集
        EditTransform();

        // 設定の編集
        EditSettings();

        // popupメニュー
        PopupMenu();

        // コントロールポイントをGizmoに登録
        if(edittingIdx_ != -1 && edittingIdx_ < controlPoints_.size()){
            ImGuiManager::RegisterGuizmoItem(&controlPoints_[edittingIdx_].first);
        }

        ImGui::Unindent();
    #endif // _DEBUG
    }



    //////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // 内部編集関数
    //////////////////////////////////////////////////////////////////////////////////////////////////////////////
#ifdef _DEBUG
// タイムライン表示 ==============================================================
    void Routine2DComponent::TimelineView(){
        // 横線を表示してその上に制御点の●を表示
        ImGui::SeparatorText("タイムライン");

        // 必要な変数の計算
        float addButtonWidth = 30.0f;
        float lrPadding = 10.0f;
        float offset = 30.0f;
        float lineWidth = ImGui::GetContentRegionAvail().x - offset - (addButtonWidth + lrPadding * 2.0f);
        lineWidth = (std::min)(lineWidth, 350.0f);// 最大350px
        ImVec2 lineStart = ImGui::GetCursorScreenPos();
        ImVec2 lineEnd = lineStart + ImVec2(lineWidth, 0.0f);
        timelineMinMaxX_ = ImVec2(lineStart.x, lineEnd.x);

        // 横線を描画
        auto* drawList = ImGui::GetWindowDrawList();
        drawList->AddLine(lineStart, lineEnd, IM_COL32(255, 255, 255, 255), 2.0f);

        // タイムラインの右側に制御点追加ボタンを設置
        ImGui::SetCursorScreenPos(lineEnd + ImVec2(lrPadding, -10.0f));
        if(ImGui::Button("制御点を追加")){
            AddPoint(timer_.GetProgress());
        }

        // 制御点を描画
        float pointRadius = 5.0f;
        ImVec2 pointSize = ImVec2(pointRadius * 2.0f, pointRadius * 2.0f);
        int32_t hoveredIdx = -1;
        for(int i = 0; i < controlPoints_.size(); i++){

            // 制御点の描画位置を計算
            float t = controlPoints_[i].second;
            ImVec2 pointPos = ImVec2(Methods::Math::Lerp(lineStart.x, lineEnd.x, t), lineStart.y);
            ImVec2 hoverMin = pointPos - pointSize * 2.0f;
            ImVec2 hoverMax = pointPos + pointSize * 2.0f;

            // ホバーしている際の処理
            if(ImGui::IsMouseHoveringRect(hoverMin, hoverMax)){
                hoveredIdx = i;

                // クリックしたら選択
                if(!isDragging_){
                    if(ImGui::IsMouseClicked(ImGuiMouseButton_Left)){
                        edittingIdx_ = i;
                        // 両端の制御点はドラッグ不可
                        if(i != 0 && i != controlPoints_.size() - 1){
                            isDragging_ = true;
                            clickedMousePos_ = ImGui::GetIO().MousePos;
                            originalPointPos_ = pointPos;
                        }
                    } else if(ImGui::IsMouseClicked(ImGuiMouseButton_Right)){
                        // ポップアップメニュー表示
                        edittingIdx_ = i;
                        ImGui::OpenPopup("PointContextMenu");
                    }
                }
            }

            // 制御点に円を描画
            bool isFocused = (edittingIdx_ == i) || (hoveredIdx == i);
            drawList->AddCircleFilled(
                pointPos, pointRadius,
                isFocused ? IM_COL32(255, 255, 0, 255) : IM_COL32(127, 127, 0, 255)
            );
        }

        // ツールチップ表示
        if(hoveredIdx != -1 or isDragging_){
            int idx = (hoveredIdx != -1) ? hoveredIdx : edittingIdx_;
            ImGui::BeginTooltip();
            ImGui::Text("制御点%d", idx);
            ImGui::Text("時間: %.2f", controlPoints_[idx].second * timer_.duration);
            ImGui::EndTooltip();
        }

        // 現在の時間の場所に縦線を引く
        float currentT = timer_.GetProgress();
        currentT = std::clamp(currentT, 0.0f, 1.0f);
        ImVec2 currentPos = ImVec2(Methods::Math::Lerp(lineStart.x, lineEnd.x, currentT), lineStart.y);
        drawList->AddLine(
            ImVec2(currentPos.x, lineStart.y - 10.0f),
            ImVec2(currentPos.x, lineStart.y + 10.0f),
            IM_COL32(0, 255, 255, 255), 2.0f
        );

        // カーソル位置の更新
        ImGui::SetCursorScreenPos(lineStart + ImVec2(0.0f, 20));
        ImGui::DragFloat("総時間", &timer_.duration, 0.1f, 0.0f, 1000.0f);
        if(ImFunc::PlayBar("現在の時間", timer_)){
            isPlaying_ = !timer_.isStop;
        }
    }

    // 制御点ドラッグ =============================================================
    void Routine2DComponent::DragPoint(){

        // クリックが離されたらドラッグ終了
        if(!ImGui::IsMouseDown(ImGuiMouseButton_Left)){
            isDragging_ = false;
        }

        // ドラッグ中でなければ終了
        if(!isDragging_){ return; }
        if(edittingIdx_ == -1){ return; }

        // マウスの移動量を取得
        ImVec2 mouseDelta = ImGui::GetIO().MousePos - clickedMousePos_;
        if(mouseDelta.x == 0.0f){ return; }

        // 新しい時間値を計算
        float newX = originalPointPos_.x + mouseDelta.x;
        float len = newX - timelineMinMaxX_.x;
        float t = len / (timelineMinMaxX_.y - timelineMinMaxX_.x);

        // tを区間内に収める
        float minT, maxT;
        // 両端は固定値
        if(edittingIdx_ == 0){
            minT = 0.0f;
            maxT = 0.0f;
        } else if(edittingIdx_ == controlPoints_.size() - 1){
            minT = 1.0f;
            maxT = 1.0f;
        } else{
            minT = controlPoints_[edittingIdx_ - 1].second + 0.01f;
            maxT = controlPoints_[edittingIdx_ + 1].second - 0.01f;
        }

        t = std::clamp(t, minT, maxT);
        controlPoints_[edittingIdx_].second = t;
    }

    // トランスフォーム編集 =============================================================
    void Routine2DComponent::EditTransform(){

        // 変形の無効化オプション
        ImGui::SeparatorText("ルーチンの無効化オプション");
        ImGui::Checkbox("移動を無効化", &disableTranslate_);
        ImGui::Checkbox("回転を無効化", &disableRotate_);
        ImGui::Checkbox("スケールを無効化", &disableScale_);

        if(edittingIdx_ != -1){
            // トランスフォームの編集
            std::string tag = "##" + std::to_string(reinterpret_cast<uintptr_t>(&controlPoints_[edittingIdx_]));
            ImGui::SeparatorText("制御点" + std::to_string(edittingIdx_));
            if(!disableTranslate_){
                ImGui::DragFloat2("座標" + tag, &controlPoints_[edittingIdx_].first.translate.x);
            }
            if(!disableRotate_){
                ImGui::DragFloat("回転" + tag, &controlPoints_[edittingIdx_].first.rotate, 0.05f);
            }
            if(!disableScale_){
                ImGui::DragFloat2("スケール" + tag, &controlPoints_[edittingIdx_].first.scale.x, 0.05f);
            }
        }
    }

    // 設定編集 =============================================================
    void Routine2DComponent::EditSettings(){
        // 設定
        ImGui::SeparatorText("設定");
        ImFunc::Combo("補間方法", interpolationType_, { "LINEAR","CATMULLROM" });
        ImFunc::Combo("イージング補間関数", easingType_, Methods::Easing::names, IM_ARRAYSIZE(Methods::Easing::names));
        ImGui::Checkbox("デフォルトで静止するか", &defaultPaused_);
        ImGui::Checkbox("ループするか", &isLoop_);
        ImGui::Checkbox("両端を接続するか", &isConnectEdge_);
        ImGui::Checkbox("ラインのデバッグ表示", &isDebugItemVisible_);
    }

    // ポップアップメニュー =============================================================
    void Routine2DComponent::PopupMenu(){
        if(ImGui::BeginPopup("PointContextMenu")){
            // 削除(両端の制御点は削除できないようにしておく)
            if(edittingIdx_ != 0 && edittingIdx_ != controlPoints_.size() - 1){
                if(ImGui::MenuItem("削除")){
                    controlPoints_.erase(controlPoints_.begin() + edittingIdx_);
                    edittingIdx_ = -1;
                    ImGui::CloseCurrentPopup();
                }
            }

            // 複製
            if(ImGui::MenuItem("複製")){
                controlPoints_.insert(controlPoints_.begin() + edittingIdx_ + 1, controlPoints_[edittingIdx_]);
                edittingIdx_ = -1;
                ImGui::CloseCurrentPopup();
            }

            ImGui::EndPopup();
        }
    }

    // 制御点追加 =============================================================
    void Routine2DComponent::AddPoint(float time){

        // 制御点の座標リストを作成
        std::vector<Transform2D> transforms;
        for(auto& point : controlPoints_){
            transforms.push_back(point.first);
        }

        // 補間方法に応じて制御点の補間
        Transform2D result;
        switch(interpolationType_){
        case Enums::InterpolationType::LINEAR:
        {
            result = Methods::SRT::Interpolate(transforms, time);
            break;
        }
        case Enums::InterpolationType::CATMULLROM:
        {
            result = Methods::SRT::CatmullRomInterpolate(transforms, time);
            break;
        }
        default:
            break;
        }

        // 新しい制御点を追加
        controlPoints_.emplace_back(result, time);

        // 時間でソート
        std::sort(controlPoints_.begin(), controlPoints_.end(),
            [](const RoutinePoint2D& a, const RoutinePoint2D& b){
            return a.second < b.second;
        });
    }

#endif // _DEBUG


    //////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // アクセサ
    //////////////////////////////////////////////////////////////////////////////////////////////////////////////
    const Transform2D& Routine2DComponent::GetControlPoint(size_t index) const{
        static Transform2D emptyTransform;
        if(index >= controlPoints_.size()){
            return emptyTransform;
        }
        return controlPoints_[index].first;
    }

    //////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // JSON読み込み・書き出し
    //////////////////////////////////////////////////////////////////////////////////////////////////////////////
    void Routine2DComponent::LoadFromJson(const nlohmann::json& jsonData){
        IComponent::LoadFromJson(jsonData);

        // 制御点の読み込み
        if(jsonData.contains("controlPoints")){
            controlPoints_.clear();
            for(const auto& point : jsonData["controlPoints"]){
                controlPoints_.push_back({ point,0.0f });
            }
        }

        // 時間情報の読み込み
        if(jsonData.contains("timePoints")){
            int idx = 0;
            for(const auto& point : jsonData["timePoints"]){
                controlPoints_[idx++].second = point;
            }
        } else{
            // 時間情報がなければ均等に割り振る
            int pointCount = int(controlPoints_.size());
            for(int i = 0; i < pointCount; i++){
                controlPoints_[i].second = float(i) / float(pointCount - 1);
            }
        }

        // 無効化設定の読み込み
        disableTranslate_ = jsonData.value("disableTranslate", false);
        disableRotate_ = jsonData.value("disableRotate", false);
        disableScale_ = jsonData.value("disableScale", false);

        // その他設定の読み込み
        interpolationType_ = static_cast<Enums::InterpolationType>(jsonData.value("interpolationType", 0));
        easingType_ = static_cast<Methods::Easing::Type>(jsonData.value("easingType", 0));
        isLoop_ = jsonData.value("isLoop", false);
        isConnectEdge_ = jsonData.value("isConnectEdge", false);
        defaultPaused_ = jsonData.value("defaultPaused", false);
        timer_.duration = jsonData.value("duration", 1.0f);
        timer_.Reset();

        // 再生状態の設定
        if(!defaultPaused_){
            isPlaying_ = true;
        }
    }

    nlohmann::json Routine2DComponent::GetJsonData() const{
        nlohmann::json jsonData;
        jsonData["componentType"] = "Routine2D";
        jsonData.update(IComponent::GetJsonData());

        // 制御点
        for(const auto& point : controlPoints_){
            jsonData["controlPoints"].push_back(point.first);
        }

        // 時間情報
        for(const auto& point : controlPoints_){
            jsonData["timePoints"].push_back(point.second);
        }

        // 無効化設定
        jsonData["disableTranslate"] = disableTranslate_;
        jsonData["disableRotate"] = disableRotate_;
        jsonData["disableScale"] = disableScale_;

        // その他設定
        jsonData["interpolationType"] = static_cast<int>(interpolationType_);
        jsonData["easingType"] = static_cast<int>(easingType_);
        jsonData["isLoop"] = isLoop_;
        jsonData["isConnectEdge"] = isConnectEdge_;
        jsonData["defaultPaused"] = defaultPaused_;
        jsonData["duration"] = timer_.duration;

        return jsonData;
    }
}