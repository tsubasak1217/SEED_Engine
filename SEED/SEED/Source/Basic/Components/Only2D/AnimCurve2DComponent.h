#pragma once
#include <SEED/Source/Basic/Object/GameObject2D.h>
#include <SEED/Source/Basic/Components/IComponent.h>
#include <SEED/Lib/Structs/Timer.h>
#include <SEED/Lib/Structs/Sprite.h>
// stl
#include <memory>
#include <vector>

enum class InterpolationType{
    LINEAR = 0,
    CATMULLROM
};

/*----------- 衝突判定をするやつ ----------*/
class AnimCurve2DComponent : public IComponent{
public:
    AnimCurve2DComponent(GameObject2D* pOwner, const std::string& tagName = "");
    ~AnimCurve2DComponent() = default;
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
    static inline bool isTextureLoaded_ = false;
    static inline unordered_map<std::string, ImTextureID> textureIDs_;
    bool defaultPaused_ = false;
    bool isLoop_ = false;
    bool isPlaying_ = false;
    Timer timer_;
    InterpolationType interpolationType_ = InterpolationType::CATMULLROM;
    std::vector<Transform2D> controlPoints_;

#ifdef _DEBUG
    bool isDebugItemVisible_ = true;
    Sprite debugPointSprite_;
#endif // _DEBUG
};