#include "Note_Warning.h"

Note_Warning::Note_Warning() : Note_Base(){
    noteType_ = NoteType::Warning;
    noteQuad_.get()->color = { 1.0f, 1.0f, 1.0f, 1.0f };// 色を白に
    noteQuad_.get()->lightingType = LIGHTINGTYPE_NONE;// ライティングを無効に

    noteQuad_->GH = TextureManager::LoadTexture("Notes/wheel_aura.png");
}

Note_Warning::~Note_Warning(){
}

void Note_Warning::Update(){
}

void Note_Warning::Draw(float currentTime, float appearLength){
    if(!start_){ return; }

    // 描画用の矩形を計算
    static Quad noteRect;
    noteRect = PlayField::GetInstance()->GetNoteQuad(1.0f, lane_, layer_, 0.5f);

    // 頂点設定
    for(int j = 0; j < 4; j++){
        noteQuad_->localVertex[j] = noteRect.localVertex[j];
        noteQuad_->localVertex[j].z += zOffset_ * 4;// zファイティング防止
    }

    // 媒介変数
    //float t = timer_.GetProgress();
    //float sin = std::sin(timer_.currentTime * (float)std::numbers::pi * 2.0f);
    //float sin2 = std::sin(t * (float)std::numbers::pi);
    float alpha = 1.0f;//(0.5f + 0.5f * sin) * sin2;

    // quadの色などの設定
    noteQuad_->color = { 1.0f, 0.0f, 0.0f, alpha };

    // 描画
    Note_Base::Draw(currentTime, appearLength);

    // 時間の更新
    timer_.Update();
}

////////////////////////////////////////////////////////////////////
// タップノーツの判定
////////////////////////////////////////////////////////////////////
Judgement::Evaluation Note_Warning::Judge(float dif){
    dif;
    return Judgement::Evaluation::NONE;
}

nlohmann::json Note_Warning::ToJson(){
    nlohmann::json json = Note_Base::ToJson();
    json["noteType"] = "warning"; // タイプを指定
    json["duration"] = duration_;
    return json;
}

void Note_Warning::FromJson(const nlohmann::json& json){
    Note_Base::FromJson(json);
    duration_ = json["duration"];
    timer_.Initialize(duration_);
}

#ifdef _DEBUG
void Note_Warning::Edit(){
    Note_Base::Edit();
    ImGui::DragFloat("Duration", &duration_, 0.1f, 0.0f);
}
#endif // _DEBUG
