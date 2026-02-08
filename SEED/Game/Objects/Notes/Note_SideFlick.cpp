#include "Note_SideFlick.h"

Note_SideFlick::Note_SideFlick() : Note_Base(){
}

Note_SideFlick::~Note_SideFlick(){
}

void Note_SideFlick::Update(){
}

void Note_SideFlick::Draw(float currentTime, float appearLength){
    Note_Base::Draw(currentTime,appearLength);
}

Judgement::Evaluation Note_SideFlick::Judge(float curTime){

    // 時間差を計算
    float signedDif = curTime - time_;
    float dif = std::fabsf(signedDif);

    // 入力情報を取得
    static auto input = PlayerInput::GetInstance();
    SEED::GeneralEnum::LR flickDirection = input->GetSideFlickDirection();
    bool isFlick = input->GetIsSideFlick();

    // フリックしていないなら、判定しない
    if(!isFlick){
        return Judgement::Evaluation::MISS;
    }

    // フリックの方向が自身のレーンと一致しているか
    if(flickDirection != flickDirection_){
        return Judgement::Evaluation::MISS;
    }

    // ノーツの判定(判定はゆるめにしてパーフェクトかミスだけにする)
    if(dif > Judgement::GetInstance()->GetJudgeTime(Judgement::Evaluation::GOOD)){
        // MISS
        return Judgement::Evaluation::MISS;
    } else{
        // PERFECT
        return Judgement::Evaluation::PERFECT;
    }
}

nlohmann::json Note_SideFlick::ToJson(){
    nlohmann::json json = Note_Base::ToJson();
    json["noteType"] = "sideFlick";
    json["flickDirection"] = static_cast<int>(flickDirection_);
    return json;
}

void Note_SideFlick::FromJson(const nlohmann::json& json){
    Note_Base::FromJson(json);
    flickDirection_ = static_cast<SEED::GeneralEnum::LR>(json["flickDirection"]);
}

#ifdef _DEBUG
void Note_SideFlick::Edit(){
}
#endif // _DEBUG
