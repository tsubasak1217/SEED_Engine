#pragma once
#include <SEED/Source/Basic/Object/GameObject2D.h>
#include <SEED/Source/Basic/Components/IComponent.h>
#include <SEED/Lib/Structs/Timer.h>
#include <SEED/Lib/Structs/Sprite.h>
#include <SEED/Lib/enums/InterpolateType.h>
// stl
#include <memory>
#include <vector>

class Routine2DComponent : public IComponent{
public:
    Routine2DComponent(GameObject2D* pOwner, const std::string& tagName = "");
    ~Routine2DComponent() = default;
    void BeginFrame()override;
    void Update()override;
    void Draw()override;
    void EndFrame()override;
    void Finalize()override;
    void EditGUI()override;

public:// accessor
    void Play(){ isPlaying_ = true; }
    void Pause(){ isPlaying_ = false; }

public:// json
    void LoadFromJson(const nlohmann::json& jsonData) override;
    nlohmann::json GetJsonData() const override;

private:
    bool defaultPaused_ = false;
    bool isLoop_ = false;
    bool isPlaying_ = false;
    Timer timer_;
    InterpolationType interpolationType_ = InterpolationType::CATMULLROM;
    std::vector<Transform2D> controlPoints_;
    Easing::Type easingType_ = Easing::Type::None;

#ifdef _DEBUG
    bool isDebugItemVisible_ = true;
    bool isEditting_ = false;
    Sprite debugPointSprite_;
#endif // _DEBUG
};