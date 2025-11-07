#include "UIComponent.h"

//////////////////////////////////////////////////////////////////////////
// コンストラクタ
//////////////////////////////////////////////////////////////////////////
UIComponent::UIComponent(GameObject2D* pOwner, const std::string& tagName)
    : IComponent(pOwner, tagName){

    if(tagName == ""){
        componentTag_ = "UI_ID:" + std::to_string(componentID_);
    }
}


//////////////////////////////////////////////////////////////////////////
// フレーム開始時の処理
//////////////////////////////////////////////////////////////////////////
void UIComponent::BeginFrame(){

}

//////////////////////////////////////////////////////////////////////////
// 更新処理
//////////////////////////////////////////////////////////////////////////
void UIComponent::Update(){

    // UVスクロール処理
    for(auto& spritePair : sprites_){
        auto& spriteItem = spritePair.second;

        // UVスクロールが有効でなければスキップ
        if(!spriteItem.isUVScroll){
            continue;
        }

        // UVスクロールタイプごとの処理
        switch(spriteItem.uvScrollType){
        case ScrollType::Normal:
            // 通常スクロール
            spriteItem.finalUVTransform.translate += spriteItem.uvScrollValue * ClockManager::DeltaTime();

            break;

        case ScrollType::StepImage:
            // 一定時間ごとに指定値をスクロール
            spriteItem.stepTimer.Update();
            if(spriteItem.stepTimer.IsFinished()){

                // ステップを加算
                spriteItem.stepTimer.Reset();
                spriteItem.curColmnCount++;
                spriteItem.totalStepCount++;

                // 一定ステップ数を超えたら初期化
                if(spriteItem.totalStepCount >= spriteItem.maxStepCount){
                    spriteItem.totalStepCount = 0;
                    spriteItem.curColmnCount = 0;
                    spriteItem.finalUVTransform = spriteItem.originalUVTransform;

                } else{
                    // 列数が最大値を超えたら改行
                    if(spriteItem.curColmnCount >= spriteItem.maxColmnCount){
                        spriteItem.curColmnCount = 0;
                        spriteItem.finalUVTransform.translate.y += spriteItem.uvScrollValue.y;
                        spriteItem.finalUVTransform.translate.x = spriteItem.originalUVTransform.translate.x;
                    
                    } else{
                        spriteItem.finalUVTransform.translate.x += spriteItem.uvScrollValue.x;
                    }
                }
            }

            break;

        default:
            break;
        }
    }

}

//////////////////////////////////////////////////////////////////////////
// 描画処理
//////////////////////////////////////////////////////////////////////////
void UIComponent::Draw(){
    // スプライトの描画
    for(auto& spritePair : sprites_){
        // UVTransformを最終値に更新
        spritePair.second.sprite.uvTransform = spritePair.second.finalUVTransform;
        // 描画
        spritePair.second.sprite.Draw(masterColor_ * owner_.owner2D->masterColor_);
        // 描画後に元のUVTransformに戻す
        spritePair.second.sprite.uvTransform = spritePair.second.originalUVTransform;
    }

    // テキストの描画
    for(auto& textPair : texts_){
        textPair.second.Draw(masterColor_ * owner_.owner2D->masterColor_);
    }
}

//////////////////////////////////////////////////////////////////////////
// フレーム終了時の処理
//////////////////////////////////////////////////////////////////////////
void UIComponent::EndFrame(){
}


//////////////////////////////////////////////////////////////////////////
// 終了処理
//////////////////////////////////////////////////////////////////////////
void UIComponent::Finalize(){
}

//////////////////////////////////////////////////////////////////////////
// GUI編集
//////////////////////////////////////////////////////////////////////////
void UIComponent::EditGUI(){
#ifdef _DEBUG
    {
        ImGui::Indent();
        std::string idStr = "##" + std::to_string(componentID_);

        // スプライト
        {
            ImGui::SeparatorText("Sprites" + idStr);

            // スプライトの追加
            if(ImGui::Button("スプライトの追加" + idStr)){
                auto& sprite = sprites_.emplace_back("sprite" + std::to_string(spriteCount_++), Sprite());
                sprite.second.sprite.parentMat = owner_.owner2D->GetWorldMatPtr();
            }

            {
                ImGui::Indent();
                // スプライト一覧
                for(size_t i = 0; i < sprites_.size(); i++){

                    // ポインタをハッシュにする
                    std::string ptrStr = MyFunc::PtrToStr(&sprites_[i].second);
                    std::string ptrHash = "##" + ptrStr;

                    // スプライトの削除
                    if(ImGui::Button("削除" + ptrHash)){
                        sprites_.erase(sprites_.begin() + i);
                        break;
                    }

                    // スプライトの編集
                    ImGui::SameLine();
                    auto& spritePair = sprites_[i];
                    if(ImGui::CollapsingHeader(spritePair.first + idStr)){
                        ImGui::Indent();

                        // 名前の編集
                        ImFunc::InputText("スプライト名" + ptrHash, spritePair.first);
                        // スプライトの編集
                        spritePair.second.sprite.Edit(ptrStr);

                        // UVTransformに変更があったら初期値に
                        if(spritePair.second.sprite.uvTransform != spritePair.second.originalUVTransform){
                            spritePair.second.originalUVTransform = spritePair.second.sprite.uvTransform;
                            spritePair.second.finalUVTransform = spritePair.second.sprite.uvTransform;
                        }

                        // UVスクロール設定の編集
                        EditUVScroll(int32_t(i));

                        ImGui::Unindent();
                    }
                }
                ImGui::Unindent();
            }
        }

        // テキスト
        {
            ImGui::SeparatorText("Texts" + idStr);

            // テキストの追加
            if(ImGui::Button("テキストの追加" + idStr)){
                auto& text = texts_.emplace_back("text" + std::to_string(textCount_++), TextBox2D("テキスト"));
                text.second.parentMat = owner_.owner2D->GetWorldMatPtr();
            }

            {
                ImGui::Indent();
                // テキスト一覧
                for(size_t i = 0; i < texts_.size(); i++){

                    // ポインタをハッシュにする
                    std::string ptrStr = MyFunc::PtrToStr(&texts_[i].second);
                    std::string ptrHash = "##" + ptrStr;

                    // テキストの削除
                    if(ImGui::Button("削除" + ptrHash)){
                        texts_.erase(texts_.begin() + i);
                        break;
                    }

                    // テキストの編集
                    auto& textPair = texts_[i];
                    ImGui::SameLine();
                    if(ImGui::CollapsingHeader(textPair.first + idStr)){
                        ImGui::Indent();

                        // 名前の編集
                        ImFunc::InputText("テキスト名" + ptrHash, textPair.first);

                        // テキストの編集
                        textPair.second.Edit(ptrStr);
                        ImGui::Unindent();
                    }
                }
                ImGui::Unindent();
            }
        }
        ImGui::Unindent();
    }
#endif
}

//////////////////////////////////////////////////////////////////////////
// スプライトの取得
//////////////////////////////////////////////////////////////////////////
Sprite& UIComponent::GetSprite(size_t index){
    if(index >= sprites_.size()){
        throw std::out_of_range("UIComponent::GetSprite(): index out of range");
    }
    return sprites_[index].second.sprite;
}

Sprite& UIComponent::GetSprite(const std::string& name){
    for(auto& spritePair : sprites_){
        if(spritePair.first == name){
            return spritePair.second.sprite;
        }
    }
    throw std::out_of_range("UIComponent::GetSprite(): name not found");
}

//////////////////////////////////////////////////////////////////////////
// テキストの取得
//////////////////////////////////////////////////////////////////////////
TextBox2D& UIComponent::GetText(size_t index){
    if(index >= texts_.size()){
        throw std::out_of_range("UIComponent::GetText(): index out of range");
    }
    return texts_[index].second;
}

TextBox2D& UIComponent::GetText(const std::string& name){
    for(auto& textPair : texts_){
        if(textPair.first == name){
            return textPair.second;
        }
    }
    throw std::out_of_range("UIComponent::GetText(): name not found");
}




//////////////////////////////////////////////////////////////////////////
// Jsonデータの読み込み
//////////////////////////////////////////////////////////////////////////
void UIComponent::LoadFromJson(const nlohmann::json& jsonData){
    IComponent::LoadFromJson(jsonData);

    // sprite情報
    sprites_.clear();
    if(jsonData.contains("sprites")){
        for(auto& spriteJson : jsonData["sprites"]){
            SpriteItems spriteItem;
            Sprite sprite;
            sprite.FromJson(spriteJson);
            sprite.parentMat = owner_.owner2D->GetWorldMatPtr();
            spriteItem.sprite = sprite;

            // UVスクロール情報
            spriteItem.isUVScroll = spriteJson.value("isUVScroll", false);
            spriteItem.uvScrollType = static_cast<ScrollType>(spriteJson.value("uvScrollType", 0));
            spriteItem.uvScrollValue = spriteJson.value("uvScrollValue", Vector2(0.0f));
            spriteItem.maxStepCount = spriteJson.value("maxStepCount", 1);
            spriteItem.maxColmnCount = spriteJson.value("maxColmnCount", 1);
            spriteItem.stepTimer.duration = spriteJson.value("stepTime", 0.1f);

            // UVTransformの初期値を保存
            spriteItem.originalUVTransform = spriteItem.sprite.uvTransform;
            spriteItem.finalUVTransform = spriteItem.sprite.uvTransform;

            // 追加
            sprites_.emplace_back(spriteJson["name"], spriteItem);
        }
    }

    // text情報
    texts_.clear();
    if(jsonData.contains("texts")){
        for(auto& textJson : jsonData["texts"]){
            TextBox2D text;
            text.LoadFromJson(textJson);
            text.parentMat = owner_.owner2D->GetWorldMatPtr();
            texts_.emplace_back(textJson["name"], text);
            TextSystem::GetInstance()->LoadFont(text.fontName);
        }
    }
}


//////////////////////////////////////////////////////////////////////////
// Jsonデータの出力
//////////////////////////////////////////////////////////////////////////
nlohmann::json UIComponent::GetJsonData() const{
    nlohmann::ordered_json jsonData;
    jsonData["componentType"] = "UI";
    jsonData.update(IComponent::GetJsonData());

    // sprite情報
    for(auto& spritePair : sprites_){
        jsonData["sprites"].push_back(spritePair.second.sprite.ToJson());
        jsonData["sprites"].back()["name"] = spritePair.first;
        // uvScroll情報
        jsonData["sprites"].back()["isUVScroll"] = spritePair.second.isUVScroll;
        jsonData["sprites"].back()["uvScrollType"] = int32_t(spritePair.second.uvScrollType);
        jsonData["sprites"].back()["uvScrollValue"] = spritePair.second.uvScrollValue;
        jsonData["sprites"].back()["maxStepCount"] = spritePair.second.maxStepCount;
        jsonData["sprites"].back()["maxColmnCount"] = spritePair.second.maxColmnCount;
        jsonData["sprites"].back()["stepTime"] = spritePair.second.stepTimer.duration;
    }

    // text情報
    for(auto& textPair : texts_){
        jsonData["texts"].push_back(textPair.second.GetJsonData());
        jsonData["texts"].back()["name"] = textPair.first;
    }

    return jsonData;
}

//////////////////////////////////////////////////////////////////////////
// UVスクロール設定編集
//////////////////////////////////////////////////////////////////////////
void UIComponent::EditUVScroll(int32_t index){
    if(index < 0 || index >= static_cast<int32_t>(sprites_.size())){
        return;
    }
    auto& spriteItem = sprites_[index].second;
    std::string tag = "##" + MyFunc::PtrToStr(&spriteItem);

    // UVスクロール設定
    if(ImGui::CollapsingHeader("UVスクロール" + tag)){

        ImGui::Checkbox("UVスクロールをするか" + tag, &spriteItem.isUVScroll);

        if(spriteItem.isUVScroll){
            ImFunc::Combo("スクロールタイプ" + tag, spriteItem.uvScrollType, { "Normal", "Step" });
            ImFunc::HelpTip("Normal: 通常スクロール\nStep: 一定時間ごとに指定値をスクロール(連番画像など)");

            ImGui::DragFloat2("スクロール速度" + tag, &spriteItem.uvScrollValue.x,0.01f);

            // ステップスクロール時の設定
            if(spriteItem.uvScrollType == ScrollType::StepImage){
                ImGui::DragFloat("何秒に一回スクロールするか" + tag, &spriteItem.stepTimer.duration, 0.01f, 0.01f, 10.0f);
                ImGui::DragInt("列数" + tag, &spriteItem.maxColmnCount, 1, 1, 100);
                ImGui::DragInt("総コマ数" + tag, &spriteItem.maxStepCount, 1, 1, 1000);
            }
        }
    }
}
