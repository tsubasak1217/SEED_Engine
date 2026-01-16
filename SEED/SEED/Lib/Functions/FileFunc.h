#pragma once
#include <random>
#include <cstdint>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <filesystem>
#include <initializer_list>
#include <json.hpp>
#include <SEED/Lib/Functions/StringFunc.h>

namespace SEED{
    namespace Methods{
        namespace File{

            // 指定したパスに存在するフォルダの一覧を取得する関数
            std::vector<std::filesystem::path> GetFolderList(const std::filesystem::path& entryPath, bool isRelative, bool isRecursive);
            // 指定したパス以降で拡張子の合致するファイル一覧を取得する関数
            std::vector<std::filesystem::path> GetFileList(
                const std::filesystem::path& entryPath, std::initializer_list<std::string>extensions, bool isRelative = true
            );

            // 指定したパス以降で名前の一致するファイルがあればそのパスを返す関数
            std::filesystem::path FindFile(const std::filesystem::path& entryPath, const std::string& filename, bool isRelative = true);

            // projectのディレクトリまでのフルパスを取得する関数
            std::filesystem::path GetProjectDirectory();

            // jsonふぁあいる関連の関数
            nlohmann::json GetJson(const std::filesystem::path& filePath, bool createFile = false);
            void CreateJsonFile(const std::filesystem::path& filePath, const nlohmann::json& jsonData);

            // ファイルのリネーム
            bool RenameFile(const std::filesystem::path& oldFilePath, const std::filesystem::path& newFilePath);

            // エクスプローラーで開く
            void OpenInExplorer(const std::filesystem::path& path);

            // ファイルの削除
            bool DeleteFileObject(const std::filesystem::path& path);

            // ファイルの保存
            std::filesystem::path OpenSaveFileDialog(const std::filesystem::path& directory, const std::string& ext, const std::string& initialName = "");

            // フォルダの新規作成
            bool CreateNewFolder(const std::filesystem::path& directory, const std::string& folderName);
        };
    }
} // namespace SEED