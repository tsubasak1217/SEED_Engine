#include "Note_Tap.h"

Note_Tap::Note_Tap() : Note_Base(){
    noteType_ = NoteType::Tap;// ノーツの種類をタップに設定
    noteQuad_.get()->color = { 1.0f, 1.0f, 1.0f, 1.0f };// 色を白に
    noteQuad_.get()->lightingType = LIGHTINGTYPE_NONE;// ライティングを無効に
}

Note_Tap::~Note_Tap(){
}

void Note_Tap::Update(){
}

void Note_Tap::Draw(float currentTime, float appearLength){
    static Quad noteRect;
    float timeRatio = (time_ - currentTime) / appearLength;

    // 描画用の矩形を計算
    noteRect = PlayField::GetInstance()->GetNoteQuad(timeRatio, lane_, layer_,0.015f * (4.0f / appearLength));

    // 頂点設定
    for(int i = 0; i < 4; i++){
        noteQuad_->localVertex[i] = noteRect.localVertex[i];
        noteQuad_->localVertex[i].z += zOffset_;
    }

    // 描画
    Note_Base::Draw(currentTime, appearLength);
}

////////////////////////////////////////////////////////////////////
// タップノーツの判定
////////////////////////////////////////////////////////////////////
Judgement::Evaluation Note_Tap::Judge(float curTime){

    // 時間差を計算
    float signedDif = curTime - time_;
    float dif = std::fabsf(signedDif);

    // 入力情報を取得
    static auto input = PlayerInput::GetInstance();
    bool isTap = input->GetIsTap();

    // タップしていないなら、判定しない
    if(!isTap){
        return Judgement::Evaluation::NONE;
    }

    // 押したレーンを取得
    auto& tapLane = input->GetTapLane();

    // 自身のレーンと押したレーンに含まれているか
    uint32_t lane = lane_ % PlayField::kKeyCount_;

    if(tapLane.find(lane) != tapLane.end()){
        // ノーツの判定
        if(dif > Judgement::GetInstance()->GetJudgeTime(Judgement::Evaluation::GOOD)){
            // MISS
            return Judgement::Evaluation::MISS;

        } else if(dif > Judgement::GetInstance()->GetJudgeTime(Judgement::Evaluation::GREAT)){
            // GOOD
            return Judgement::Evaluation::GOOD;

        } else if(dif > Judgement::GetInstance()->GetJudgeTime(Judgement::Evaluation::PERFECT)){
            // GREAT
            return Judgement::Evaluation::GREAT;

        } else{
            // PERFECT
            return Judgement::Evaluation::PERFECT;
        }
    }

    // 押したレーンに含まれていないなら、MISS
    return Judgement::Evaluation::MISS;
}

nlohmann::json Note_Tap::ToJson(){
    nlohmann::json json = Note_Base::ToJson();
    json["noteType"] = "tap"; // タイプを指定
    return json;
}

void Note_Tap::FromJson(const nlohmann::json& json){
    Note_Base::FromJson(json);

    if(isExtraNote_){
        noteQuad_->GH = TextureManager::LoadTexture("Notes/tapNoteEx.png");
    } else{
        noteQuad_->GH = TextureManager::LoadTexture("Notes/tapNote.png");
    }
}

#ifdef _DEBUG
void Note_Tap::Edit(){
    Note_Base::Edit();
}
#endif // _DEBUG
