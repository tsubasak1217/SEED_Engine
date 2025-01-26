#pragma once


#include "Vector2.h"
#include "Vector3.h"
#include <filesystem>
#include <fstream>
#include <functional>
#include <json.hpp>
#include <optional>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <variant>

using json = nlohmann::ordered_json;
using AdjustableValue = std::variant<int,float,Vector2,Vector3,bool,std::string>;

class JsonCoordinator{
public:
    //===================================================================*/
    //                   public function
    //===================================================================*/

    // 項目を登録してバインド
    template <typename T>
    static bool RegisterItem(const std::string& group,const std::string& key,T& target);

    // 値を設定
    static bool SetValue(const std::string& group,const std::string& key,AdjustableValue value);

    // 値を取得
    static std::optional<AdjustableValue> GetValue(const std::string& group,const std::string& key);


    // グループのみ保存 (ファイル分割方式)
    static bool SaveGroup(const std::string& group,std::optional<std::string> directoryPath = std::nullopt);

    // グループのみロード (ファイル分割方式)
    static bool LoadGroup(const std::string& group,std::optional<std::string> directoryPath = std::nullopt);

    // グループ内のすべての項目をレンダリング (ImGui)
    static void RenderGroupUI(const std::string& group);

    // グループのClear
    static void ClearGroup(const std::string& group);

private:
    //===================================================================*/
    //                   private function
    //===================================================================*/
    static void RenderAdjustableItem(const std::string& group,const std::string& key);

    // ディレクトリ作成
    static void EnsureDirectoryExists(const std::string& path);


    // グループ -> JSON ファイル名を作る（適宜カスタマイズ）
    static std::string MakeFilePath(const std::string& group,std::optional<std::string> directoryPath = std::nullopt);

private:
    //===================================================================*/
    //                   private variable
    //===================================================================*/
    // グループごとに独立した JSON データを保管
    static inline std::unordered_map<std::string,json> s_groupData_;

    // バインディング: グループ -> (キー -> コールバック)
    static inline std::unordered_map<std::string,
        std::unordered_map<std::string,std::function<void(const AdjustableValue&)>>>
        s_bindings_;

    // ベースパスなど
    static std::string s_baseDirectory_;
};

//===================================================================*/
//                  inline Function
//===================================================================*/

// AdjustableValue を JSON 形式に変換
//-------------------------------------------------------------------
inline void to_json(json& j,const AdjustableValue& value){
    std::visit([&](auto&& arg){ j = arg; },value);
}

//-------------------------------------------------------------------
// JSON から AdjustableValue に変換
//-------------------------------------------------------------------
inline void from_json(const json& j,AdjustableValue& value){
    if(j.is_number_integer()){
        value = j.get<int>();
    } else if(j.is_number_float()){
        value = j.get<float>();
    } else if(j.is_object()){
        size_t objectSize = j.object().size();
        if(objectSize == 2){
            value = j.get<Vector2>();
        } else if(objectSize == 3){
            value = j.get<Vector3>();
        }
    } else if(j.is_boolean()){
        value = j.get<bool>();
    } else if(j.is_string()){
        value = j.get<std::string>();
    }
}

//-------------------------------------------------------------------
// RegisterItem (テンプレート)
//-------------------------------------------------------------------
template <typename T>
bool JsonCoordinator::RegisterItem(const std::string& group,const std::string& key,T& target){
    // まだグループが存在しなければ空の json を確保
    if(!s_groupData_.count(group)){
        s_groupData_[group] = json::object();
    }

    // すでにバインディングがあれば、JSON → target の同期だけ
    if(s_bindings_[group].count(key) > 0){
        auto existingVal = s_groupData_[group][key].get<AdjustableValue>();
        if(auto valPtr = std::get_if<T>(&existingVal)){
            target = *valPtr;
        }
        return true;
    }

    // まだバインディングが無い場合は、「JSON にキーがあるか」をチェック
    if(s_groupData_[group].contains(key)){
        // JSON 側にキーがあれば、JSON → target
        auto existingVal = s_groupData_[group][key].get<AdjustableValue>();
        if(auto valPtr = std::get_if<T>(&existingVal)){
            target = *valPtr;
        }
        // もし型が違う場合は、何らかのログを出すか変換ロジックを入れるなど検討
    } else{
        // JSON 側にキーが無ければ、target → JSON
        s_groupData_[group][key] = target;
    }

    // バインディング登録（target のポインタをコピーキャプチャ）
    s_bindings_[group][key] = [targetPtr = &target](const AdjustableValue& value){
        if(auto valPtr = std::get_if<T>(&value)){
            *targetPtr = *valPtr;
        }
        };

    return true;
}

