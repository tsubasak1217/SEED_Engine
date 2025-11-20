#pragma once
#include <SEED/Source/Basic/Components/IComponent.h>
#include <string>
#include <vector>
#include <SEED/Source/Manager/AudioManager/AudioManager.h>

/// <summary>
/// 色を制御するコンポーネント
/// </summary>
class AudioComponent : public IComponent{
private:
    struct AudioItem{
        // 書き出す項目
        std::string filePath;
        std::string name;
        float volume = 0.5f;
        bool isLoop = false;
        float startTime = 0.0f;
        float playOffsetTime;
        Timer playDelayTimer;

        // 書き出さない項目
        AudioHandle handle = -1;
        Timer playTimer;
        bool isPlayed = false;
    };

public:// 基礎関数
    AudioComponent(std::variant<GameObject*, GameObject2D*> pOwner, const std::string& tagName = "");
    ~AudioComponent() = default;
    void Initialize()override;
    void BeginFrame() override;
    void Update() override;
    void Draw() override;
    void EndFrame() override;
    void Finalize() override;

public:// 入出力
    // GUI編集
    void EditGUI() override;
    // json
    nlohmann::json GetJsonData() const override;
    void LoadFromJson(const nlohmann::json& jsonData) override;

private:
    std::vector<AudioItem> audioList_;
};