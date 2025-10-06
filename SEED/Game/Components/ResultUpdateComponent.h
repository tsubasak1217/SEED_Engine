#pragma once
#include <Game/GameSystem.h>
#include <SEED/Source/Basic/Object/GameObject2D.h>
#include <SEED/Source/Basic/Components/IComponent.h>
#include <SEED/Lib/Structs/Timer.h>
#include <SEED/Lib/Structs/Sprite.h>
#include <Game/Objects/Result/PlayResult.h>
// stl
#include <memory>
#include <vector>
#include <unordered_map>
#include <string>

class ResultUpdate2DComponent : public IComponent{
public:
    ResultUpdate2DComponent(GameObject2D* pOwner, const std::string& tagName = "");
    ~ResultUpdate2DComponent() = default;
    void BeginFrame()override;
    void Update()override;
    void Draw()override;
    void EndFrame()override;
    void Finalize()override;
    void EditGUI()override;

public:// accessor
    void SetPlayResult(const PlayResult& result) { playResult_ = result; }

private:
    TimerArray timer_;
    std::unordered_map<std::string, GameObject2D*> uiObjects_;
    PlayResult playResult_;
    PlayResult displayResult_;
    bool isFirstUpdate_ = true;
    bool isInitialized_ = false;
};