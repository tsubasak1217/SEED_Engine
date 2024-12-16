#include <GameState_Play.h>

// local
#include <SEED.h>
#include <includes.h>
#include <ShapeMath.h>
#include <Environment.h>
#include "Easing.h"

// scene,state
#include <SceneManager.h>
#include <Scene_Clear.h>
#include <Scene_Game.h>
#include <GameState_Exit.h>
#include <GameState_Enter.h>

// external
#include <ImGui.h>
#include <random>
#include <cmath>
#include <algorithm>



/*==================================================================================================*/
/*                                      コンストラクタ：デストラクタ                                    */
/*==================================================================================================*/

GameState_Play::GameState_Play(Scene_Game* pScene) : State_Base(pScene){
    player_ = std::make_unique<Player>();

    for(int32_t i = 0; i < 10; i++){
        enemies_.push_back(std::make_unique<Enemy>());
    }
}


void GameState_Play::Initialize(){}


/*------------------------デストラクタ---------------------------*/

GameState_Play::~GameState_Play(){}


/*==================================================================================================*/
/*                                             更新処理                                              */
/*==================================================================================================*/


////////////////////////////////////////////////////////////////////////////
//                          全部まとめた更新処理                             //
////////////////////////////////////////////////////////////////////////////

void GameState_Play::Update(){

    // プレイヤーの更新
    player_->Update();

    // 敵の更新
    for(auto& enemy : enemies_){
        enemy->Update();
    }

    // 当たり判定
    for(auto& enemy : enemies_){
        auto enemyCollider = enemy->GetColliders();
        for(int32_t i = 0; i < player_->GetBulletSize(); i++){
            auto bullet = player_->GetBullet(i);
            const auto& bulletCollider = bullet->GetColliders();
            for(auto& enemyCol : enemyCollider){
                for(auto& bulletCol : bulletCollider){
                    // 二つの当たり判定の距離を求める
                    float dif = MyMath::Length(enemyCol.GetPosition() - bulletCol.GetPosition());
                    float sum = enemyCol.GetRadius() + bulletCol.GetRadius();
                    if(dif < sum){
                        enemy->SetIsAlive(false);
                        bullet->SetIsAlive(false);
                    }
                }
            }
        }
    }

    // 死んでいる敵の削除
    enemies_.remove_if([](const std::unique_ptr<Enemy>& enemy){
        return !enemy->GetIsAlive();
    });

    // クリア判定
    if(enemies_.empty()){
        pScene_->SetNextScene(new Scene_Clear());
    }
}


/*==================================================================================================*/
/*                                             描画処理                                              */
/*==================================================================================================*/


void GameState_Play::Draw(){

    // プレイヤーの描画
    player_->Draw();

    // 敵の描画
    for(auto& enemy : enemies_){
        enemy->Draw();
    }
}



/*==================================================================================================*/
/*                                            その他関数                                              */
/*==================================================================================================*/
