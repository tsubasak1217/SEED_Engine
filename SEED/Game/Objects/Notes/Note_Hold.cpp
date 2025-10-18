#include "Note_Hold.h"

Note_Hold::Note_Hold() : Note_Base(){
    // ノーツの種類をホールドに設定
    noteType_ = NoteType::Hold;

    // ホールドノーツのテクスチャの設定
    textureGHs_[0] = TextureManager::LoadTexture("Notes/holdNote_Head.png");
    textureGHs_[1] = TextureManager::LoadTexture("Notes/holdNote_Head.png");
    textureGHs_[2] = TextureManager::LoadTexture("Notes/holdNote_Body.png");

    // ホールドノーツの色の設定
    noteColors_[0] = { 1.0f, 1.0f, 0.0f, 1.0f };
    noteColors_[1] = { 1.0f, 1.0f, 0.0f, 1.0f };
    noteColors_[2] = { 1.0f, 1.0f, 1.0f, 1.0f };

    // ライティングを無効に
    noteQuad_.get()->lightingType = LIGHTINGTYPE_NONE;
}

Note_Hold::~Note_Hold(){

}

void Note_Hold::Update(){
}

/////////////////////////////////////////////////////////////////////
// ホールドノーツの描画
/////////////////////////////////////////////////////////////////////
void Note_Hold::Draw(float currentTime, float appearLength){

    static Quad noteRect[3];
    float timeRatio[2] = { (time_ - currentTime) / appearLength,((time_ + kHoldTime_) - currentTime) / appearLength };

    // 描画用の矩形を計算
    noteRect[0] = PlayField::GetInstance()->GetNoteQuad(timeRatio[0], lane_, layer_, 0.01f);
    noteRect[1] = PlayField::GetInstance()->GetNoteQuad(timeRatio[1], lane_, layer_, 0.01f);

    // 始点から終点までの間の部分の頂点
    noteRect[2].localVertex[0] = noteRect[1].localVertex[2];
    noteRect[2].localVertex[1] = noteRect[1].localVertex[3];
    noteRect[2].localVertex[2] = noteRect[0].localVertex[0];
    noteRect[2].localVertex[3] = noteRect[0].localVertex[1];

    // すべての矩形を描画
    for(int i = 0; i < 3; i++){
        // 頂点設定
        for(int j = 0; j < 4; j++){
            noteQuad_->localVertex[j] = noteRect[i].localVertex[j];
            noteQuad_->localVertex[j].z += zOffset_ * 4;// zファイティング防止
        }

        // テクスチャと色の設定
        noteQuad_->GH = textureGHs_[i];
        noteQuad_->color = noteColors_[i];

        // 押していない場合に色を暗くする(頭が判定ラインを超えていない場合はそのまま)
        if(headEvaluation_ == Judgement::Evaluation::NONE){
            bool isTrigger = PlayerInput::GetInstance()->GetIsTap(lane_);
            if(!isTrigger && timeRatio[0] < 0.0f){
                noteQuad_->color.value.w *= 0.25f;
            }
        } else{
            if(isReleased_){
                if(!isHold_){
                    noteQuad_->color.value.w *= 0.25f;
                }
            }
        }

        // 描画
        Note_Base::Draw(currentTime, appearLength);
    }
}

/////////////////////////////////////////////////////////////////////
// ホールドノーツの判定
/////////////////////////////////////////////////////////////////////
Judgement::Evaluation Note_Hold::Judge(float dif){

    // 入力情報を取得
    uint32_t lane = lane_ % PlayField::kKeyCount_;
    static auto input = PlayerInput::GetInstance();

    // 判定時間の取得
    static float judgeTime[Judgement::kEvaluationCount] = {
        Judgement::GetInstance()->GetJudgeTime(Judgement::Evaluation::PERFECT),
        Judgement::GetInstance()->GetJudgeTime(Judgement::Evaluation::GREAT),
        Judgement::GetInstance()->GetJudgeTime(Judgement::Evaluation::GOOD),
        Judgement::GetInstance()->GetJudgeTime(Judgement::Evaluation::MISS)
    };

    // 自身のレーンが押されているか
    isHold_ = input->GetIsPress(lane);
    if(isHold_){
        isReleased_ = !Input::IsPressMouse(MOUSE_BUTTON::LEFT);
    }

    if(Input::IsReleaseMouse(MOUSE_BUTTON::LEFT)){
        isReleased_ = true;
    }

    // 頭をまだ押していない場合の判定
    if(headEvaluation_ == Judgement::Evaluation::NONE){

        // ホールドしていないなら、判定しない
        if(!isHold_){
            releaseTime_ += ClockManager::DeltaTime();
            return Judgement::Evaluation::NONE;

        } else{// ホールドしている

            // ホールドしているが、まだ押していない
            bool isTrigger = input->GetIsTap(lane);
            if(!isTrigger){
                return Judgement::Evaluation::NONE;
            }

            // ノーツの判定
            if(dif > judgeTime[Judgement::Evaluation::GOOD]){
                // MISS
                return headEvaluation_ = Judgement::Evaluation::MISS;

            } else if(dif > judgeTime[Judgement::Evaluation::GREAT]){
                // GOOD
                return headEvaluation_ = Judgement::Evaluation::GOOD;

            } else if(dif > judgeTime[Judgement::Evaluation::PERFECT]){
                // GREAT
                return headEvaluation_ = Judgement::Evaluation::GREAT;

            } else{
                // PERFECT
                return headEvaluation_ = Judgement::Evaluation::PERFECT;
            }
        }

    } else{// 頭をもう押している場合

        if(isReleased_){
            if(!isHold_){
                // 離している時間を加算
                releaseTime_ += ClockManager::DeltaTime();
            }
        }

        return Judgement::Evaluation::NONE;
    }
}

/////////////////////////////////////////////////////////////////////
// ホールドノーツの終点の判定
/////////////////////////////////////////////////////////////////////
Judgement::Evaluation Note_Hold::JudgeHoldEnd(){

    // 
    static float rate = 5.0f;

    // 判定時間の取得
    static float judgeTime[Judgement::kEvaluationCount] = {
        Judgement::GetInstance()->GetJudgeTime(Judgement::Evaluation::PERFECT) * rate,
        Judgement::GetInstance()->GetJudgeTime(Judgement::Evaluation::GREAT) * rate,
        Judgement::GetInstance()->GetJudgeTime(Judgement::Evaluation::GOOD) * rate,
        Judgement::GetInstance()->GetJudgeTime(Judgement::Evaluation::MISS) * rate
    };

    // 押していないないなら、ミス
    if(headEvaluation_ == Judgement::Evaluation::NONE){
        return Judgement::Evaluation::MISS;
    }

    // 押していなかった時間に応じて判定を行う(離す判定はゆるく設けるため、少しでも押していたら最低でもGOOD判定)
    if(releaseTime_ < judgeTime[Judgement::Evaluation::PERFECT]){
        return Judgement::Evaluation::PERFECT;
    } else if(releaseTime_ < judgeTime[Judgement::Evaluation::GREAT]){
        return Judgement::Evaluation::GREAT;
    } else{
        return Judgement::Evaluation::GOOD;
    }
}

// ホールドノーツのJSON変換
nlohmann::json Note_Hold::ToJson(){
    nlohmann::json j = Note_Base::ToJson();
    // ホールドノーツの情報を追加
    j["noteType"] = "Hold"; // ノーツの種類を追加
    j["holdTime"] = kHoldTime_;
    return j;
}

// jsonからホールドノーツの情報を読み込む
void Note_Hold::FromJson(const nlohmann::json& json){
    // 基本情報の読み込み
    Note_Base::FromJson(json);
    // ホールドノーツの情報の読み込み
    kHoldTime_ = json["holdTime"];
}

#ifdef _DEBUG
void Note_Hold::Edit(){
    // 基本情報の編集
    Note_Base::Edit();
    ImGui::Separator();
    // ホールドノーツの情報の編集
    ImGui::DragFloat("ホールドの長さ", &kHoldTime_, 0.01f, 0.1f,FLT_MAX, "%.3f", ImGuiSliderFlags_AlwaysClamp);
}
#endif // _DEBUG
