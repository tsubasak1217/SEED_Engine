#include "Player.h"

/////////////////////////////////////////////////////////////////
//          初期化・終了処理
/////////////////////////////////////////////////////////////////
Player::Player(){
    player_ = MakeEqualQuad2D(radius_, { 1.0f,0.0f,0.0f,1.0f });
    player_.translate = { 640.0f,500.0f };
}

Player::~Player(){}


/////////////////////////////////////////////////////////////////
//          更新
/////////////////////////////////////////////////////////////////
void Player::Update(){

    Move();
    Shoot();

    for(auto& bullet : bullets_){
        bullet->Update();
    }

    // 死んだ弾の削除
    bullets_.remove_if([](Bullet* bullet){
        if(!bullet->GetIsAlive()){
            delete bullet;
            bullet = nullptr;
            return true;
        } else{
            return false;
        };
    });
}


/////////////////////////////////////////////////////////////////
//          描画
/////////////////////////////////////////////////////////////////
void Player::Draw(){
    SEED::DrawQuad2D(player_);

    for(auto& bullet : bullets_){
        bullet->Draw();
    }
}


/////////////////////////////////////////////////////////////////
//          弾の発射
/////////////////////////////////////////////////////////////////
void Player::Shoot(){

    // クールタイムの更新
    coolTime_ -= ClockManager::DeltaTime();

    // スペースキーが押されたら
    if(Input::IsPressKey(DIK_SPACE)){

        // クールタイムが終わっていなかったら処理しない
        if(coolTime_ > 0.0f){ return; }

        // 弾の追加
        auto& bullet = bullets_.emplace_back(new Bullet());
        bullet->SetPosition(player_.translate);
        bullet->SetMoveDirection({ 0.0f,-1.0f });
        bullet->SetSpeed(10.0f);

        // クールタイムの初期化
        coolTime_ = kCoolTime;
    }
}

/////////////////////////////////////////////////////////////////
//          移動
/////////////////////////////////////////////////////////////////
void Player::Move(){
    Vector2 direction = { float(Input::IsPressKey(DIK_D) - Input::IsPressKey(DIK_A)),0.0f };
    player_.translate += direction * speed_;
}
