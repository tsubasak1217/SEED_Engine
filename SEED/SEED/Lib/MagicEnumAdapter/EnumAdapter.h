#pragma once

//============================================================================
//	include
//============================================================================

// c++
#include <typeinfo>
#include <array>
#include <cstdint>
#include <optional>
#include <type_traits>
// imgui
#include <SEED/External/imgui/imgui.h>
// magic_enum
#include <SEED/External/magic_enum/magic_enum.hpp>

// enumにのみ制限
template<typename T>
concept MagicEnumConcept = std::is_enum_v<T>;

//============================================================================
//	EnumAdapter class
//============================================================================
template<MagicEnumConcept Enum>
class EnumAdapter {
public:
	//========================================================================
	//	public Methods
	//========================================================================

    // enumの列挙数
    static constexpr size_t GetEnumCount() noexcept {

        return magic_enum::enum_count<Enum>();
    }

    // index番目の名前を取得
    static constexpr const char* GetEnumName(uint32_t index) noexcept {

        constexpr auto names = magic_enum::enum_names<Enum>();
        return index < names.size() ? names[index].data() : "";
    }

    // 全ての列挙名を取得
    static constexpr std::array<const char*, magic_enum::enum_count<Enum>()> GetEnumArray() noexcept {

        constexpr auto names = magic_enum::enum_names<Enum>();
        constexpr auto arr = [] {
            std::array<const char*, magic_enum::enum_count<Enum>()> tmp{};
            for (std::size_t i = 0; i < tmp.size(); ++i)
                tmp[i] = names[i].data();
            return tmp;
            }();
        return arr;
    }

    static constexpr Enum GetValue(std::uint32_t index) noexcept {

        constexpr auto values = magic_enum::enum_values<Enum>();
        return index < values.size() ? values[index] : static_cast<Enum>(0);
    }

    static constexpr uint32_t GetIndex(Enum value) noexcept {

        auto idx = magic_enum::enum_index<Enum>(value);
        return idx ? static_cast<uint32_t>(*idx) : 0;
    }

    static constexpr const char* ToString(Enum value) noexcept {

        return magic_enum::enum_name(value).data();
    }

    static constexpr const std::string_view& ToStringView(Enum value) noexcept {

        return magic_enum::enum_name(value);
    }

    static constexpr std::optional<Enum> FromString(std::string_view name) noexcept {

        return magic_enum::enum_cast<Enum>(name);
    }

    static bool Combo(const char* label, Enum* current) noexcept {

        int idx = static_cast<int>(GetIndex(*current));
        bool changed = ImGui::Combo(label,
            &idx, GetEnumArray().data(),
            static_cast<int>(GetEnumCount()));
        if (changed) {

            *current = GetValue(static_cast<std::uint32_t>(idx));
        }
        return changed;
    }
};