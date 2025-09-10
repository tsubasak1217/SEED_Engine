#pragma once

//============================================================================
//	include
//============================================================================
#include <SEED/Lib/Input/InputMapper.h>
#include <Game/Manager/AudioDictionary.h>
#include <Game/Objects/Stage/Objects/Player/Input/PlayerInputActionEnum.h>

// json
#include <SEED/External/nlohmann/json.hpp>
// front
class Player;

//============================================================================
//	PlayerIState class
//============================================================================
class PlayerIState {
public:
    //========================================================================
    //	public Methods
    //========================================================================

    PlayerIState() = default;
    virtual ~PlayerIState() = default;

    // 状態遷移時
    virtual void Enter(Player& player) = 0;

    // 更新処理
    virtual void Update(Player& player) = 0;

    // 描画処理
    virtual void Draw([[maybe_unused]] Player& player) {};

    // 状態終了時
    virtual void Exit(Player& player) = 0;

    // エディター
    virtual void Edit(const Player& player) = 0;

    // json
    virtual void FromJson(const nlohmann::json& data) = 0;
    virtual void ToJson(nlohmann::json& data) = 0;

    // audio
    virtual void StopAudio() {}

    //--------- accessor -----------------------------------------------------

    void SetInputMapper(const InputMapper<PlayerInputAction>* inputMapper) { inputMapper_ = inputMapper; }
protected:
    //========================================================================
    //	protected Methods
    //========================================================================

    //--------- variables ----------------------------------------------------

    const InputMapper<PlayerInputAction>* inputMapper_;
};