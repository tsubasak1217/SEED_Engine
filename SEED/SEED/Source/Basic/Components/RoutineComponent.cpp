#include "RoutineComponent.h"
#include <SEED/Source/Basic/Object/GameObject.h>
#include <SEED/Source/SEED.h>

/////////////////////////////////////////////////////////////////////////
// コンストラクタ
/////////////////////////////////////////////////////////////////////////
RoutineComponent::RoutineComponent(GameObject* pOwner, const std::string& tagName)
    : IComponent(pOwner, tagName){
    if(tagName == ""){
        componentTag_ = "Routine_ID:" + std::to_string(componentID_);
    }
}

/////////////////////////////////////////////////////////////////////////
// 初期化処理
/////////////////////////////////////////////////////////////////////////
void RoutineComponent::Initialize(){

}

/////////////////////////////////////////////////////////////////////////
// フレーム開始時の処理
/////////////////////////////////////////////////////////////////////////
void RoutineComponent::BeginFrame(){

}

/////////////////////////////////////////////////////////////////////////
// 更新処理
/////////////////////////////////////////////////////////////////////////
void RoutineComponent::Update(){

    if(!isActive_ || points_.empty() || duration_ == 0.0f){
        return; // ルーチンが無効または制御点がない場合は何もしない
    }

    // 制御点の更新処理
    currentTime_ += ClockManager::DeltaTime() * speedRate_;
    if(isLoop_){
        currentTime_ = MyFunc::Spiral(currentTime_, 0.0f, duration_); // ループする場合は範囲をくりかえす
    } else{
        currentTime_ = std::clamp(currentTime_, 0.0f, duration_); // ループしない場合は時間を制限
    }

    // ownerのトランスフォームを設定
    std::vector<Vector3>translates;
    for(const auto& point : points_){
        translates.push_back(point.translate);
    }

    if(isConnectLoop_ && translates.size() > 1){
        translates.push_back(translates.front()); // ループ接続する場合は最初の点を最後に追加
    }

    float t = currentTime_ / duration_;
    Vector3 nextTranslate = MyMath::CatmullRomPosition(translates, t);

    // tから現在の区間を求める
    if(!isConnectLoop_){
        auto it = points_.begin();
        int index = static_cast<int>(t * (points_.size() - 1));
        std::advance(it, index);

        // 区間から回転とスケールの補完
        if(it != points_.end() && points_.size() > 1){
            float divisionEvery = 1.0f / points_.size();
            float t2 = (t - (index * divisionEvery)) / divisionEvery;
            owner_->SetLocalTranslate(nextTranslate);
            owner_->SetLocalRotate(Quaternion::Slerp(it->rotate, std::next(it)->rotate, t2));
            owner_->SetLocalScale(MyMath::Lerp(it->scale, std::next(it)->scale, t2));
        } else{
            // 最後の制御点に到達した場合
            owner_->SetLocalTranslate(points_.back().translate);
            owner_->SetLocalRotate(points_.back().rotate);
            owner_->SetLocalScale(points_.back().scale);
        }

    } else{
        std::vector<Vector3> scales;
        std::vector<Quaternion> rotates;

        for(const auto& point : points_){
            scales.push_back(point.scale);
            rotates.push_back(point.rotate);
        }

        if(scales.size() > 1){
            // ループ接続する場合は最初の点を最後に追加
            scales.push_back(scales.front());
            rotates.push_back(rotates.front());

            // 補完してループ接続
            float divisionEvery = 1.0f / (points_.size() - 1);
            float t2 = (t - std::floor(t)) / divisionEvery; // ループ接続のための補完値
            int index = static_cast<int>(t / divisionEvery);

            if(index < scales.size() - 1){
                // 通常の補完
                owner_->SetLocalTranslate(nextTranslate);
                owner_->SetLocalRotate(Quaternion::Slerp(rotates[index], rotates[index + 1], t2));
                owner_->SetLocalScale(MyMath::Lerp(scales[index], scales[index + 1], t2));
            } else{
                // 最後の制御点に到達した場合
                owner_->SetLocalTranslate(points_.back().translate);
                owner_->SetLocalRotate(points_.back().rotate);
                owner_->SetLocalScale(points_.back().scale);
            }
        }
    }

    if(isRotateByDirection_){
        // 方向に応じて回転する場合
        Vector3 moveVec = nextTranslate - owner_->GetPrePos();
        Vector3 eulerRotate = MyFunc::CalcRotateVec(moveVec);
        owner_->SetLocalRotate(Quaternion::ToQuaternion(eulerRotate));
    }
}


/////////////////////////////////////////////////////////////////////////
// 描画処理
/////////////////////////////////////////////////////////////////////////
void RoutineComponent::Draw(){
}


/////////////////////////////////////////////////////////////////////////
// コンストラクタ
/////////////////////////////////////////////////////////////////////////
void RoutineComponent::EndFrame(){

}


/////////////////////////////////////////////////////////////////////////
// フレーム終了時の処理
/////////////////////////////////////////////////////////////////////////
void RoutineComponent::Finalize(){
}


/////////////////////////////////////////////////////////////////////////
// GUI上での編集処理
/////////////////////////////////////////////////////////////////////////
void RoutineComponent::EditGUI(){
#ifdef _DEBUG
    ImGui::Indent();

    ImGui::Checkbox("有効", &isActive_);
    ImGui::Checkbox("ループするか", &isLoop_);
    if(isLoop_){
        ImGui::Checkbox("ループの接続を行うか", &isConnectLoop_);
    }

    ImGui::DragFloat("総時間", &duration_, 0.1f, 0.0f);
    ImGui::DragFloat("速度倍率", &speedRate_, 0.1f, 0.1f);
    ImGui::Checkbox("移動方向に応じて自動回転するか", &isRotateByDirection_);

    if(ImGui::CollapsingHeader("制御点の管理")){
        ImGui::Indent();

        static float insertLocation = 1.0f;
        if(ImGui::SliderFloat("挿入位置", &insertLocation, 0.0f, 1.0f, "%.2f")){
            std::vector<Vector3>tmpTranslates;
            for(const auto& point : points_){
                tmpTranslates.push_back(point.translate);
            }

            // 挿入予定位置の計算
            Vector3 insertPos = MyMath::CatmullRomPosition(tmpTranslates, insertLocation);
            SEED::DrawSphere(insertPos, 3.0f, 6, { 1.0f,0.0f,0.0f,1.0f });
        }
        
        if(ImGui::Button("制御点を挿入")){
            if(points_.empty()){
                // 挿入位置に新しい制御点を追加
                Transform newPoint;
                points_.emplace_back(newPoint);
            } else{
                // 挿入位置に新しい制御点を追加
                auto it = points_.begin();
                std::advance(it, static_cast<int>(insertLocation * points_.size()));
                Transform newPoint;

                // 補完して新しい制御点を挿入
                if(it == points_.end()){
                    // 最後の位置に挿入
                    newPoint = points_.back();
                } else{
                    // 挿入位置の前後の制御点を取得
                    auto prevIt = (it == points_.begin()) ? it : std::prev(it);
                    newPoint.translate = (prevIt->translate + it->translate) * 0.5f;
                    newPoint.rotate = Quaternion::Slerp(prevIt->rotate, it->rotate, 0.5f);
                    newPoint.scale = (prevIt->scale + it->scale) * 0.5f;
                }

                points_.insert(it, newPoint);
            }

        }

        int idx = 0;
        std::vector<Vector3>translates;
        for(auto& point : points_){
            // ギズモに登録
            Matrix4x4 basePointMat = TranslateMatrix(points_.front().translate);
            ImGuiManager::RegisterGuizmoItem(&point,basePointMat);
            static std::string label;

            // 制御点の削除ボタン
            label = "削除##" + std::to_string(idx);
            if(ImGui::Button(label.c_str())){
                points_.erase(std::next(points_.begin(), idx));
                break; // 削除後はループを抜ける
            }

            // 制御点の折り畳み編集ヘッダー
            label = "制御点##" + std::to_string(idx);
            ImGui::SameLine();
            bool isOpen = ImGui::CollapsingHeader(label.c_str());


            // 制御点の編集
            if(isOpen){
                ImGui::Indent();

                label = "位置##" + std::to_string(idx);
                ImGui::DragFloat3(label.c_str(), &point.translate.x, 0.01f);

                label = "回転##" + std::to_string(idx);
                Vector3 eulerRotate = point.rotate.ToEuler();
                if(ImGui::DragFloat3(label.c_str(), &eulerRotate.x, 0.01f)){
                    point.rotate = Quaternion::ToQuaternion(eulerRotate);
                }

                label = "スケール##" + std::to_string(idx);
                ImGui::DragFloat3(label.c_str(), &point.scale.x, 0.01f);
                ImGui::Unindent();
            }

            translates.push_back(point.translate);
            idx++;
        }

        // 制御点の描画
        if(isConnectLoop_ && translates.size() > 1){
            translates.push_back(translates.front()); // ループ接続する場合は最初の点を最後に追加
        }

        SEED::DrawSpline(translates, 8, Vector4(0.0f, 1.0f, 1.0f, 1.0f));

        ImGui::Unindent();
    }

    ImGui::Unindent();
#endif // _DEBUG
}


/////////////////////////////////////////////////////////////////////////
// jsonへの書き出し
/////////////////////////////////////////////////////////////////////////
nlohmann::json RoutineComponent::GetJsonData() const{
    nlohmann::ordered_json jsonData;
    jsonData["componentType"] = "Routine";
    jsonData.update(IComponent::GetJsonData());

    for(const auto& point : points_){
        jsonData["points"].push_back(point);
    }

    jsonData["isActive"] = isActive_;
    jsonData["duration"] = duration_;
    jsonData["speedRate"] = speedRate_;
    jsonData["isLoop"] = isLoop_;
    jsonData["isConnectLoop"] = isConnectLoop_;
    jsonData["isRotateByDirection"] = isRotateByDirection_;

    return jsonData;
}


/////////////////////////////////////////////////////////////////////////
// jsonからの読み込み処理
/////////////////////////////////////////////////////////////////////////
void RoutineComponent::LoadFromJson(const nlohmann::json& jsonData){

    IComponent::LoadFromJson(jsonData);

    // 制御点の読み込み
    if(jsonData.contains("points")){
        for(const auto& pointData : jsonData["points"]){
            points_.emplace_back(pointData);
        }
    }

    // その他のプロパティの読み込み
    if(jsonData.contains("isActive")){
        isActive_ = jsonData["isActive"].get<bool>();
    }
    if(jsonData.contains("duration")){
        duration_ = jsonData["duration"].get<float>();
    }
    if(jsonData.contains("speedRate")){
        speedRate_ = jsonData["speedRate"].get<float>();
    }
    if(jsonData.contains("isLoop")){
        isLoop_ = jsonData["isLoop"].get<bool>();
    }
    if(jsonData.contains("isConnectLoop")){
        isConnectLoop_ = jsonData["isConnectLoop"].get<bool>();
    }
    if(jsonData.contains("isRotateByDirection")){
        isRotateByDirection_ = jsonData["isRotateByDirection"].get<bool>();
    }
}
