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
    inputHandler_ = new InputHandler();
    inputHandler_->AssignMoveLeftCommandToPressA();
    inputHandler_->AssignMoveRightCommandToPressD();

    player_ = std::make_unique<Player>();
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

    command_ = inputHandler_->HandleInput();

    if(command_){
        command_->Execute(player_.get());
    }

    // プレイヤーの更新
    player_->Update();

}


/*==================================================================================================*/
/*                                             描画処理                                              */
/*==================================================================================================*/


void GameState_Play::Draw(){

    // プレイヤーの描画
    player_->Draw();

}



/*==================================================================================================*/
/*                                            その他関数                                              */
/*==================================================================================================*/
