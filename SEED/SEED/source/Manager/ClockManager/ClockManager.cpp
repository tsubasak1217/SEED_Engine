#include "ClockManager.h"
#include <thread>

/////////////////////////////////////////////////
//      static変数初期化
/////////////////////////////////////////////////

ClockManager* ClockManager::instance_ = nullptr;
const float ClockManager::kFrameTime_ = 1.0f / 60.0f;

/////////////////////////////////////////////////
//      コンストラクタ・デストラクタ
/////////////////////////////////////////////////

ClockManager::ClockManager(){}
ClockManager::~ClockManager(){
    delete instance_;
    instance_ = nullptr;
}

/////////////////////////////////////////////////
//      初期化関数
/////////////////////////////////////////////////

void ClockManager::Initialize(){
    instance_ = GetInstance();
    instance_->currentTime_ = std::chrono::high_resolution_clock::now();
    instance_->preTime_ = instance_->currentTime_;
}



/////////////////////////////////////////////////
//      更新処理
/////////////////////////////////////////////////

void ClockManager::BeginFrame(){
    instance_->preTime_ = instance_->currentTime_;
}

void ClockManager::EndFrame(){

    instance_->currentTime_ = std::chrono::high_resolution_clock::now();
    std::chrono::duration<float> elapsedTime = instance_->currentTime_ - instance_->preTime_;


    // フレーム内で経過した時間を取得
    instance_->deltaTime_ = elapsedTime.count();

    // 累計時間を加算
    instance_->totalTime_ += instance_->deltaTime_;

    // 時間のレートを計算
    instance_->timeRate_ = instance_->deltaTime_ / kFrameTime_;

    // FPSを計算
    instance_->fps_ = 1.0f / instance_->deltaTime_;

    if(instance_->isStop_ or instance_->isRestartNow_){
        instance_->preTime_ = instance_->currentTime_;
        instance_->deltaTime_ = 0.0f;
        instance_->timeRate_ = 0.0f;
        instance_->fps_ = 0.0f;
    }

    instance_->isRestartNow_ = false;
    instance_->isStop_ = false;
}

void ClockManager::Update(){}


/////////////////////////////////////////////////
//      インスタンス取得
/////////////////////////////////////////////////

ClockManager* ClockManager::GetInstance(){

    if(!instance_){ instance_ = new ClockManager; }
    return instance_;
}

/////////////////////////////////////////////////
//      その他関数
/////////////////////////////////////////////////

void ClockManager::AddNewCount(const std::string& name){
    name;
}
