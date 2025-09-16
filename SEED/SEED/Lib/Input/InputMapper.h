#pragma once

//============================================================================
//	include
//============================================================================
#include <SEED/Lib/Input/IInputDevice.h>

// c++
#include <memory>
#include <vector>
#include <algorithm>

//============================================================================
//	InputMapper class
//============================================================================
template<InputEnum Enum>
class InputMapper {
public:
    //========================================================================
    //	public Methods
    //========================================================================

    InputMapper() = default;
    ~InputMapper() = default;

    // 使用する入力デバイスを追加
    void AddDevice(std::unique_ptr<IInputDevice<Enum>> device);

    //--------- accessor -----------------------------------------------------

    float GetVector(Enum action) const;

    bool IsPressed(Enum button) const;
    bool IsTriggered(Enum button) const;
private:
    //========================================================================
    //	private Methods
    //========================================================================

    //--------- variables ----------------------------------------------------

    // 使用する入力デバイス
    std::vector<std::unique_ptr<IInputDevice<Enum>>> devices_;
};

//============================================================================
//	InputMapper templateMethods
//============================================================================

template<InputEnum Enum>
inline void InputMapper<Enum>::AddDevice(std::unique_ptr<IInputDevice<Enum>> device) {

    devices_.emplace_back(std::move(device));
}

template<InputEnum Enum>
inline float InputMapper<Enum>::GetVector(Enum action) const {

    float vector = 0.0f;
    // 各入力から値を入れる
    for (const auto& device : devices_) {

        vector += device->GetVector(action);
    }
    vector = std::clamp(vector, -1.0f, 1.0f);
    return vector;
}

template<InputEnum Enum>
inline bool InputMapper<Enum>::IsPressed(Enum button) const {

    // 入力結果を取得
    for (const auto& device : devices_) {
        if (device->IsPressed(button)) {

            return true;
        }
    }
    return false;
}

template<InputEnum Enum>
inline bool InputMapper<Enum>::IsTriggered(Enum button) const {

    // 入力結果を取得
    for (const auto& device : devices_) {
        if (device->IsTriggered(button)) {

            return true;
        }
    }
    return false;
}