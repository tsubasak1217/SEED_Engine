#include "Stage.h"
#include <Game/Objects/Stage/StageManager.h>

// blocks
#include <Game/Objects/Stage/Blocks/Block_Wall.h>
#include <Game/Objects/Stage/Blocks/Block_Box.h>
#include <Game/Objects/Stage/Blocks/Block_Toge.h>
#include <Game/Objects/Actor/Player/Player.h>

// SEED
#include <SEED/Source/SEED.h>

//////////////////////////////////////////////////////////////
// コンストラクタ・デストラクタ
//////////////////////////////////////////////////////////////
Stage::Stage(){
}

Stage::~Stage(){
}

//////////////////////////////////////////////////////////////
// 初期化
//////////////////////////////////////////////////////////////
void Stage::Initialize(){
    Reset();
}

void Stage::Initialize(const std::string& filePath){
    Reset();
    filePath_ = filePath;
    LoadStage(filePath_);
}

//////////////////////////////////////////////////////////////
// フレームの開始時処理
//////////////////////////////////////////////////////////////
void Stage::BeginFrame(){
    for(auto& block : blocks_){
        if(block){
            block->BeginFrame();
        }
    }
}

//////////////////////////////////////////////////////////////
// フレームの終了時処理
//////////////////////////////////////////////////////////////
void Stage::EndFrame(){

    // ステージの状態の解決
    if(isCharacterMoved_){
        SolveStage();
        isCharacterMoved_ = false;
    }

    // isAliveがfalseのブロックを削除
    for(auto it = blocks_.begin(); it != blocks_.end();){
        if((*it)->GetIsAlive() == false){
            blockMap_[(*it)->address_.y][(*it)->address_.x] = std::nullopt;
            
            // collisionMapからも削除
            Vector2i address = (*it)->address_;
            for(auto it2 = collisionMap_[address.y][address.x].begin(); it2 != collisionMap_[address.y][address.x].end();){
                if(!blockMap_[address.y][address.x].has_value()){ break; }
                if(blockMap_[address.y][address.x].value().first == *it2){
                    it2 = collisionMap_[address.y][address.x].erase(it2);
                } else{
                    ++it2;
                }
            }

            it = blocks_.erase(it);
        } else{
            ++it;
        }
    }

    // ステージ情報を保存
    preBlockMap_ = blockMap_;
}

//////////////////////////////////////////////////////////////
// 更新
//////////////////////////////////////////////////////////////
void Stage::Update(){
    for(auto& block : blocks_){
        if(block){
            block->Update();
        }
    }
}

//////////////////////////////////////////////////////////////
// 描画
//////////////////////////////////////////////////////////////
void Stage::Draw(){
    for(auto& block : blocks_){
        if(block){
            block->Draw();
        }
    }

#ifdef _DEBUG
    DrawStageArea();
#endif // _DEBUG

}

//////////////////////////////////////////////////////////////
// リセット
//////////////////////////////////////////////////////////////
void Stage::Reset(){
    blocks_.clear();
    ResizeStage();
}


//////////////////////////////////////////////////////////////
// マトリックスの更新
//////////////////////////////////////////////////////////////
void Stage::UpdateMatrix(){
    for(auto& block : blocks_){
        if(block){
            block->UpdateMatrix();
        }
    }
}

//////////////////////////////////////////////////////////////
// ステージの範囲を描画
//////////////////////////////////////////////////////////////
void Stage::DrawStageArea(){
    static Vector2 center;
    static Vector2 size;
    static Vector2 LT, RB;
    static float heightOffset = 0.1f;

    // ステージの範囲を計算
    center = Vector2((float)stageSize_.x - 1, (float)-(stageSize_.y - 1))
        * StageManager::GetInstance()->GetBlockSize() * 0.5f;
    size = Vector2((float)stageSize_.x, (float)-stageSize_.y)
        * StageManager::GetInstance()->GetBlockSize();

    // 左上、右下の座標を計算
    LT = center - size * 0.5f;
    RB = center + size * 0.5f;

    // マス目表示
    Vector3 LR[2] = { {LT.x,heightOffset,LT.y},{RB.x,heightOffset,LT.y} };
    Vector3 TB[2] = { {LT.x,heightOffset,LT.y},{LT.x,heightOffset,RB.y} };

    for(int32_t x = 0; x < stageSize_.y + 1; ++x){
        SEED::DrawLine(
            LR[0] + Vector3(0.0f, 0.0f, (float)x * -StageManager::GetInstance()->GetBlockSize()),
            LR[1] + Vector3(0.0f, 0.0f, (float)x * -StageManager::GetInstance()->GetBlockSize()),
            { 1.0f,1.0f,0.0f,1.0f }
        );
    }

    for(int32_t y = 0; y < stageSize_.x + 1; ++y){
        SEED::DrawLine(
            TB[0] + Vector3((float)y * StageManager::GetInstance()->GetBlockSize(), 0.0f, 0.0f),
            TB[1] + Vector3((float)y * StageManager::GetInstance()->GetBlockSize(), 0.0f, 0.0f),
            { 1.0f,1.0f,0.0f,1.0f }
        );
    }

}

//////////////////////////////////////////////////////////////
// プレイヤーが移動を要求
//////////////////////////////////////////////////////////////
bool Stage::RequestPlayerMove(const Vector2i& address, Block_Base* pBlock){

    // 範囲外の場合はfalse
    if(address.x < 0 || address.x >= stageSize_.x ||
        address.y < 0 || address.y >= stageSize_.y){
        return false;
    }

    // ブロックが存在しない場合はtrue
    if(blockMap_[address.y][address.x] == std::nullopt){
        // プレイヤーの座標を更新
        MoveBlock(pBlock->address_, address);
        return true;

    } else{

        // 壁ブロックの場合はfalse
        if(blockMap_[address.y][address.x].value().first->blockType_ == BlockType::Wall){
            return false;

        } else if(blockMap_[address.y][address.x].value().first->blockType_ == BlockType::Box){
            Vector2i direction = address - pBlock->address_;
            Vector2i nextAddress = address + direction;

            // 次のアドレスが範囲外の場合はfalse
            if(nextAddress.x < 0 || nextAddress.x >= stageSize_.x ||
                nextAddress.y < 0 || nextAddress.y >= stageSize_.y){
                return false;
            }

            // 次のアドレスにブロックが存在する場合はfalse
            if(blockMap_[nextAddress.y][nextAddress.x]){
                return false;
            }

            // 1つならBoxを移動して動けるのでtrue(入れ替える)
            MoveBlock(address, nextAddress);
            // プレイヤーの座標を更新
            MoveBlock(pBlock->address_, address);

            return true;

        } else{
            // プレイヤーの座標を更新
            MoveBlock(pBlock->address_, address);
            return true;
        }
    }
}

//////////////////////////////////////////////////////////////
// ブロックの挿入を試みる
//////////////////////////////////////////////////////////////
void Stage::TryInsertBlock(const Vector2i& address, BlockType blockType){
    // 範囲外の場合は挿入しない
    if(address.x < 0 || address.x >= stageSize_.x ||
        address.y < 0 || address.y >= stageSize_.y){
        return;
    }

    // ブロックの種類によって生成
    std::unique_ptr<Block_Base> newBlock = nullptr;
    if(blockType == BlockType::Wall){
        newBlock = std::make_unique<Block_Wall>();

    } else if(blockType == BlockType::Box){
        newBlock = std::make_unique<Block_Box>();

    } else if(blockType == BlockType::Player){

        // すでにプレイヤーが存在する場合は消してから挿入
        for(auto& block : blocks_){
            if(block && block->blockType_ == BlockType::Player){
                block.reset();
            }
        }

        newBlock = std::make_unique<Player>();
    } else if(blockType == BlockType::Toge){
        newBlock = std::make_unique<Block_Toge>();
    }

    newBlock->SetAddress(address);
    newBlock->Initialize();

    // 既にブロックが存在する場合は上書き
    if(blockMap_[address.y][address.x].has_value()){
        size_t index = blockMap_[address.y][address.x].value().second;
        blocks_[index].reset();
        blocks_[index] = std::move(newBlock);
        blockMap_[address.y][address.x].value().first = blocks_[index].get();
        collisionMap_[address.y][address.x].clear();
        collisionMap_[address.y][address.x].front() = blocks_[index].get();

    } else{
        // ブロックが存在しない場合は新規追加
        blocks_.emplace_back(std::move(newBlock));
        blockMap_[address.y][address.x] = std::make_pair(blocks_.back().get(), blocks_.size() - 1);
        collisionMap_[address.y][address.x].clear();
        collisionMap_[address.y][address.x].emplace_back(blocks_.back().get());
    }
}

//////////////////////////////////////////////////////////////
// ブロックの削除
//////////////////////////////////////////////////////////////
void Stage::DeleteBlock(const Vector2i& address){
    // 範囲外の場合は削除しない
    if(address.x < 0 || address.x >= stageSize_.x ||
        address.y < 0 || address.y >= stageSize_.y){
        return;
    }
    // ブロックが存在する場合は削除
    if(blockMap_[address.y][address.x].has_value()){

        blockMap_[address.y][address.x].reset();
        collisionMap_[address.y][address.x].clear();

        // block本体を削除
        for(auto it = blocks_.begin(); it != blocks_.end();){
            if((*it)->address_ == address){
                it = blocks_.erase(it);
            } else{
                ++it;
            }
        }
    }
}

//////////////////////////////////////////////////////////////
// ステージの読み込み
//////////////////////////////////////////////////////////////
void Stage::LoadStage(const std::string& filePath){

    blocks_.clear();
    blockMap_.clear();
    collisionMap_.clear();
    nlohmann::json json;
    std::ifstream ifs(filePath);

    if(ifs.fail()){
        std::cerr << "File do not exist." << std::endl;
        return;
    }

    ifs >> json;
    ifs.close();

    // ステージ番号の読み込み
    if(json.contains("stageNo")){
        stageNo_ = json["stageNo"];
    }

    // ステージのサイズを読み込み
    if(json.contains("stageSize")){
        stageSize_ = json["stageSize"];
        ResizeStage();
    }

    // カメラの高さを読み込み
    if(json.contains("cameraHeight")){
        cameraHeight_ = json["cameraHeight"];
    }

    // ブロックの読み込み
    if(json.contains("blocks")){
        for(auto& block : json["blocks"]){
            int32_t blockType = block["type"];
            Vector2i address = block["address"];

            // ブロックの種類によって生成
            if(blockType == (int32_t)BlockType::Wall){
                blocks_.emplace_back(std::make_unique<Block_Wall>());

            } else if(blockType == (int32_t)BlockType::Box){
                blocks_.emplace_back(std::make_unique<Block_Box>());

            } else if(blockType == (int32_t)BlockType::Player){
                blocks_.emplace_back(std::make_unique<Player>());

            } else if(blockType == (int32_t)BlockType::Toge){
                blocks_.emplace_back(std::make_unique<Block_Toge>());
            }

            blocks_.back()->Load(block);
            blockMap_[address.y][address.x] = std::make_pair(blocks_.back().get(), blocks_.size() - 1);
            collisionMap_[address.y][address.x].emplace_back(blocks_.back().get());
        }
    }
}

//////////////////////////////////////////////////////////////
// ステージの出力
//////////////////////////////////////////////////////////////
void Stage::OutputStage(){
    nlohmann::json j;
    j["stageNo"] = stageNo_;
    j["stageSize"] = stageSize_;
    j["cameraHeight"] = cameraHeight_;
    for(auto& block : blocks_){
        if(block){
            nlohmann::json blockJson;
            blockJson["type"] = (int32_t)block->blockType_;
            blockJson["address"] = block->address_;
            block->Output(blockJson);
            j["blocks"].push_back(blockJson);
        }
    }

    if(stageNo_ != -1){
        std::ofstream ofs("Resources/jsons/stages/stage" + std::to_string(stageNo_) + ".json");
        ofs << j.dump(4);
    } else{
        std::ofstream ofs("Resources/jsons/stages/editStage.json");
        ofs << j.dump(4);
    }

}

//////////////////////////////////////////////////////////////
// ステージのリサイズ
//////////////////////////////////////////////////////////////
void Stage::ResizeStage(){
    // ブロックの配列数を確保
    blockMap_.resize(stageSize_.y);
    for(auto& block : blockMap_){
        block.resize(stageSize_.x);
    }

    // 衝突判定用の配列を確保
    collisionMap_.resize(stageSize_.y);
    for(auto& block : collisionMap_){
        block.resize(stageSize_.x);
    }
}

//////////////////////////////////////////////////////////////
// ステージの状態の解決
//////////////////////////////////////////////////////////////
void Stage::SolveStage(){

    // プレイヤーが移動した場合のみ解決
    if(!isCharacterMoved_){
        return;
    }

    // 重なりを解決
    for(int32_t y = 0; y < stageSize_.y; ++y){
        for(int32_t x = 0; x < stageSize_.x; ++x){
            // ブロックが2つ以上重なっている場合
            if(collisionMap_[y][x].size() >= 2){

                // 当たったブロックを通知する
                for(int i = 0; i < collisionMap_[y][x].size(); i++){
                    for(int j = i + 1; j < collisionMap_[y][x].size(); j++){
                        collisionMap_[y][x][i]->SolveCollision(collisionMap_[y][x][j]);
                        collisionMap_[y][x][j]->SolveCollision(collisionMap_[y][x][i]);
                    }
                }
            }
        }
    }

    // プレイヤーが他のプレイヤーに見られているか(正面の直線上にいるか)確認
    std::list<Player*> players;
    for(auto& block : blocks_){
        if(block && block->blockType_ == BlockType::Player){
            players.emplace_back(static_cast<Player*>(block.get()));
        }
    }

    // プレイヤーが他のプレイヤーに見られているか(正面の直線上にいるか)確認
    for(auto& player : players){
        for(auto& otherPlayer : players){
            if(player != otherPlayer){
                Vector2i direction = player->address_ - otherPlayer->address_;
                Vector2i forward[2] = {
                    player->GetForwardVec(),
                    otherPlayer->GetForwardVec()
                };

                // 一直線上にいる場合
                if(direction.x == 0 && direction.y != 0){

                    if(direction.y > 0){
                        direction.y = 1;
                    } else if(direction.y < 0){
                        direction.y = -1;
                    }

                    if(direction.y + forward[0].y == 0){
                        // こちらから見ていて番号が後ろの階層の場合
                        if(otherPlayer->GetCharacterNo() > player->GetCharacterNo()){
                            // 見つかったフラグを立てる
                            player->SetIsFoundDoppel(true);
                        }
                    }

                    if(-direction.y + forward[1].y == 0){
                        // 相手から見ていて番号が後ろの階層の場合
                        if(player->GetCharacterNo() > otherPlayer->GetCharacterNo()){
                            // 見つけたフラグを立てる
                            otherPlayer->SetIsFoundDoppel(true);
                        }
                    }

                } else if(direction.y == 0 && direction.x != 0){

                    if(direction.x > 0){
                        direction.x = 1;
                    } else if(direction.x < 0){
                        direction.x = -1;
                    }

                    if(direction.x + forward[0].x == 0){
                        // こちらから見ていて番号が後ろの階層の場合
                        if(otherPlayer->GetCharacterNo() > player->GetCharacterNo()){
                            // 見つけたフラグを立てる
                            player->SetIsFoundDoppel(true);
                        }
                    }
                    if(-direction.x + forward[1].x == 0){
                        // 相手から見ていて番号が後ろの階層の場合
                        if(player->GetCharacterNo() > otherPlayer->GetCharacterNo()){
                            // 見つけたフラグを立てる
                            otherPlayer->SetIsFoundDoppel(true);
                        }
                    }
                }
            }
        }
    }

    // 
    int32_t topCharacterNo = 9999;
    for(auto& player : players){
        // ドッペルゲンガーを見つけた場合
        if(player->GetIsFoundDoppel()){
            // より上の階層の場合に番号を更新
            if(player->GetCharacterNo() < topCharacterNo){
                topCharacterNo = player->GetCharacterNo();
            }
        }
    }

    // 上の階層に見られたら消す
    if(topCharacterNo != 9999){
        for(auto& player : players){
            if(player->GetCharacterNo() > topCharacterNo){
                player->SetIsAlive(false);
            } else if(player->GetCharacterNo() == topCharacterNo){
                // 操作権を戻す
                player->SetIsDoppel(false);
                player->SetIsPlaying(true);
                player->SetIsFoundDoppel(false);
            }
        }
    }


}

void Stage::MoveBlock(const Vector2i& from, const Vector2i& to){

    // collisionMapの移動元をリセットする
    for(auto it = collisionMap_[from.y][from.x].begin(); it != collisionMap_[from.y][from.x].end();){
        if(!blockMap_[from.y][from.x].has_value()){ break; }
        if(blockMap_[from.y][from.x].value().first == *it){
            it = collisionMap_[from.y][from.x].erase(it);
        } else{
            ++it;
        }
    }

    // ブロックを移動
    blockMap_[to.y][to.x] = blockMap_[from.y][from.x];
    blockMap_[from.y][from.x].reset();
    blockMap_[to.y][to.x].value().first->SetAddress(to);
    blockMap_[to.y][to.x].value().first->UpdateMatrix();
    collisionMap_[to.y][to.x].emplace_back(blockMap_[to.y][to.x].value().first);

    isCharacterMoved_ = true;
}

void Stage::CreateDoppelganger(const Vector2i address){

    // 回転を取得
    Vector3 rotate;
    rotate = blockMap_[address.y][address.x].value().first->GetRotate();

    // リセットする
    for(int32_t i = 0; i < blocks_.size(); i++){
        if(blocks_[i]){

            Vector2i address2 = blocks_[i]->address_;

            // 元居た場所をリセット
            for(auto it = collisionMap_[address2.y][address2.x].begin(); it != collisionMap_[address2.y][address2.x].end();){
                if(blockMap_[address2.y][address2.x].value().first == *it){
                    it = collisionMap_[address2.y][address2.x].erase(it);
                } else{
                    ++it;
                }
            }

            blockMap_[address2.y][address2.x].reset();

            // アドレスを初期の場所に
            blocks_[i]->ResetAddress();
            blocks_[i]->UpdateMatrix();
            // ドッペルゲンガーがいる場所をリセット
            blockMap_[blocks_[i]->address_.y][blocks_[i]->address_.x].reset();
            blockMap_[blocks_[i]->address_.y][blocks_[i]->address_.x] = std::make_pair(blocks_[i].get(), i);
            collisionMap_[blocks_[i]->address_.y][blocks_[i]->address_.x].emplace_back(blocks_[i].get());
        }
    }

    // ドッペルゲンガーを生成
    auto& doppel = blocks_.emplace_back(std::make_unique<Player>());
    doppel->SetAddress(address);
    doppel->SetFirstAddress(address);
    doppel->Initialize();
    doppel->SetRotate(rotate);
    doppel->UpdateMatrix();
    doppel->SetFirstPos(doppel->GetTranslate());

    // ドッペルゲンガーがいる場所をリセット
    blockMap_[address.y][address.x].reset();
    blockMap_[address.y][address.x] = std::make_pair(doppel.get(), blocks_.size() - 1);
    collisionMap_[address.y][address.x].emplace_back(doppel.get());
}
