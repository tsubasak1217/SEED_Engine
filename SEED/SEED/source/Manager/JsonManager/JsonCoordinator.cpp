#include "JsonCoordinator.h"
#include <filesystem>
#include <iostream>
#include <algorithm>
#include "../SEED/external/imgui/imgui.h"

//-------------------------------------------------------------------
// ヘルパー関数: フルパスを構築
//-------------------------------------------------------------------
std::string JsonCoordinator::ConstructFullPath(const std::string& fileName, const std::optional<std::string>& parentPath){
    std::string fullPath = baseDirectory_;
    if (parentPath){
        fullPath += *parentPath + "/";
    }
    fullPath += fileName + ".json";
    return fullPath;
}

//-------------------------------------------------------------------
// ヘルパー関数: ディレクトリ作成
//-------------------------------------------------------------------
void JsonCoordinator::EnsureDirectoryExists(const std::string& path){
    std::filesystem::create_directories(std::filesystem::path(path).parent_path());
}

//-------------------------------------------------------------------
// 値を設定する
//-------------------------------------------------------------------
bool JsonCoordinator::SetValue(const std::string& group, const std::string& key, AdjustableValue value){
    auto& groupData = data_[group];
    auto it = std::find_if(groupData.begin(), groupData.end(), [&] (const auto& pair){ return pair.first == key; });
    if (it == groupData.end()){
        return false; // 未登録のキー
    }

    it->second = value;

    if (bindings_.count(group) && bindings_[group].count(key)){
        bindings_[group][key](value);
    }
    return true;
}

//-------------------------------------------------------------------
// 値を取得する
//-------------------------------------------------------------------
std::optional<AdjustableValue> JsonCoordinator::GetValue(const std::string& group, const std::string& key){
    auto& groupData = data_[group];
    auto it = std::find_if(groupData.begin(), groupData.end(), [&] (const auto& pair){ return pair.first == key; });
    if (it != groupData.end()){
        return it->second;
    }
    return std::nullopt;
}

//-------------------------------------------------------------------
// JSONとして保存
//-------------------------------------------------------------------
bool JsonCoordinator::Save(const std::string& fileName, std::optional<std::string> parentPath){
    std::string fullPath = ConstructFullPath(fileName, parentPath);
    EnsureDirectoryExists(fullPath);

    json jsonData = json::object();

    // 登録順序を保持して JSON に書き込む
    for (const auto& [group, items] : data_){
        json groupData = json::object();
        for (const auto& [key, value] : items){
            groupData[key] = value;
        }
        jsonData[group] = groupData;  // グループごとに順序を保持
    }

    // ファイルに書き込み
    std::ofstream file(fullPath);
    if (!file.is_open()){
        return false;
    }
    file << jsonData.dump(4);  // インデントを 4 に設定
    file.close();

    return true;
}



//-------------------------------------------------------------------
// JSONからロード
//-------------------------------------------------------------------
bool JsonCoordinator::Load(const std::string& fileName, std::optional<std::string> parentPath){
    std::string fullPath = ConstructFullPath(fileName, parentPath);

    // ファイルが存在しない場合、新規作成
    if (!std::filesystem::exists(fullPath)){
        Save(fileName, parentPath);
        return false;  // 新規作成
    }

    // ファイルを開く
    std::ifstream file(fullPath);
    if (!file.is_open()){
        return false;
    }

    // JSON を読み込み
    json jsonData;
    file >> jsonData;

    // JSON データを登録された順序に従って `data_` にロード
    try{
        for (const auto& [group, items] : jsonData.items()){
            data_[group].clear();  // グループのデータをクリア
            for (const auto& [key, value] : items.items()){
                data_[group].emplace_back(key, value.get<AdjustableValue>());
            }
        }
    } catch (...){
        return false;  // パース失敗
    }

    return true;  // 正常終了
}


//-------------------------------------------------------------------
// グループ内のすべての項目をレンダリング
//-------------------------------------------------------------------
void JsonCoordinator::ShowGroupUI(const std::string& group){
    if (!data_.count(group)){
        std::cout << "No data for group: " << group << std::endl;
        return;
    }

    for (const auto& [key, value] : data_[group]){
        ShowAdjustableItem(group, key);
    }
}

//-------------------------------------------------------------------
// 個別アイテムをレンダリング
//-------------------------------------------------------------------
void JsonCoordinator::ShowAdjustableItem(const std::string& group, const std::string& key){
    if (auto value = GetValue(group, key)){
        if (std::holds_alternative<int>(*value)){
            int val = std::get<int>(*value);
            if (ImGui::InputInt(key.c_str(), &val)){
                SetValue(group, key, val);
            }
        } else if (std::holds_alternative<float>(*value)){
            float val = std::get<float>(*value);
            if (ImGui::InputFloat(key.c_str(), &val)){
                SetValue(group, key, val);
            }
        } else if (std::holds_alternative<Vector3>(*value)){
            Vector3 v = std::get<Vector3>(*value);
            if (ImGui::DragFloat3(key.c_str(), &v.x, 0.01f)){
                SetValue(group, key, v);
            }
        }
    }
}