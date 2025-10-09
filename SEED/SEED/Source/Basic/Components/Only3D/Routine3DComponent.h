#pragma once
#include <SEED/Source/Basic/Object/GameObject.h>
#include <SEED/Source/Basic/Components/IComponent.h>
#include <SEED/Lib/Structs/Timer.h>
#include <SEED/Lib/Structs/Sprite.h>
#include <SEED/Lib/enums/InterpolateType.h>
// stl
#include <memory>
#include <vector>

class Routine3DComponent : public IComponent{
public:
    Routine3DComponent(GameObject* pOwner, const std::string& tagName = "");
    ~Routine3DComponent() = default;
    void BeginFrame()override;
    void Update()override;
    void Draw()override;
    void EndFrame()override;
    void Finalize()override;
    void EditGUI()override;

public:// accessor
    void Play(){ isPlaying_ = true; }
    void Pause(){ isPlaying_ = false; }
    const Transform* GetControlPoint(int32_t index) const;

public:// json
    void LoadFromJson(const nlohmann::json& jsonData) override;
    nlohmann::json GetJsonData() const override;

private:
    static inline bool isTextureLoaded_ = false;
    static inline unordered_map<std::string, ImTextureID> textureIDs_;
    bool defaultPaused_ = false;
    bool isLoop_ = false;
    bool isPlaying_ = false;
    Timer timer_;
    InterpolationType interpolationType_ = InterpolationType::CATMULLROM;
    std::vector<Transform> controlPoints_;
    Easing::Type easingType_ = Easing::Type::None;

#ifdef _DEBUG
    bool isDebugItemVisible_ = true;
    bool isEditting_ = false;
    Model debugPointModel_;
#endif // _DEBUG
};