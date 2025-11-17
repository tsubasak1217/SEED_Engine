#pragma once
#include <SEED/Source/Basic/Scene/SceneState_Base.h>
#include <Game/Manager/RythmGameManager.h>
#include <SEED/Source/Basic/Object/GameObject.h>
#include <memory>
#include <list>
#include <SEED/Lib/Structs/TextBox.h>
#include <SEED/Lib/Structs/Range1D.h>


/// <summary>
/// プレイシーンでのポーズ状態
/// </summary>
class GameState_SelectMenu : public SceneState_Base{
public:
    GameState_SelectMenu();
    GameState_SelectMenu(Scene_Base* pScene);
    ~GameState_SelectMenu() override;

public:
    void Initialize() override;
    void Finalize() override;
    void Update() override;
    void Draw() override;
    void BeginFrame() override;
    void EndFrame() override;
    void HandOverColliders() override;
    void ManageState() override;

private:
    void SelectMenuItem();
    void UpdateHelpTexts();

private:// option関連
    void LoadOptionItems();
    void UpdateOptionItems();
    void ReleaseOptionItems();
    void CheckPress();
    void UpdateHeaders();
    void UpdateSlider();
    void ApplyAdditionalTransform();

private:

    enum SelectMenuItem{
        Option,
        BackToTitle,
        SelectMenuItemCount
    };

    enum OptionPageItem{
        NotesSpeed,
        JudgeOffset,
        DisplayOffset,
        AnswerOffset,
        MouseSensitivity,
        OkButton,
        kOptionPageItemCount
    };

    enum SliderSprites{
        LeftPoint,
        MidPoint,
        RightPoint,
        DragPoint,
        Body,
        kSliderSpriteCount
    };

    enum SliderTexts{
        ValueText,
        DetailText,
        kSliderTextCount
    };

private:
    // 読み込んだものを格納するポインタ-----------
    // メニュー画面関連
    GameObject2D* menuItemsParent_;
    std::array<GameObject2D*, SelectMenuItemCount> menus_;
    GameObject2D* backSpriteObj_ = nullptr;
    // オプション画面関連
    GameObject2D* optionPageParent_;
    GameObject2D* sliderObj_;
    std::array<GameObject2D*, kOptionPageItemCount> optionItems_;
    std::array<Sprite*, kSliderSpriteCount> sliderSprites_;
    std::array<TextBox2D*, kSliderTextCount> optionTexts_;

    // コライダーオブジェクト
    GameObject2D* mouseColliderObj_;
    GameObject2D* sliderColliderObj_;

    // 座標
    Vector2 sliderLeftPos_;
    Vector2 sliderRightPos_;

    //選択関連
    int32_t selectedIndex_ = 0;
    OptionPageItem currentOptionItem_ = NotesSpeed;
    bool isDraggingSlider_ = false;
    bool isLoadOptionItems_ = false;
    std::array<float, kOptionPageItemCount - 1> slidersT_;
    std::array<float, kOptionPageItemCount - 1> sliderValues_;
    std::array<Range1D, kOptionPageItemCount - 1> sliderValueRanges_;
    std::array<std::string, kOptionPageItemCount - 1> sliderDetailTexts_;

    // タイマー関連
    Timer menuTransitionTimer_;
    Timer optionPageTimer_;
    Timer sliderScalingTimer_;
    std::array<Timer, SelectMenuItemCount> menuItemScalingTimers_;
    std::array<Timer, kOptionPageItemCount> optionItemTimers_;
    std::array<Timer, kOptionPageItemCount> optionItemHoverTimers_;

    // state遷移用
    bool isExit_ = false;
    bool isInOptionPage_ = false;
};