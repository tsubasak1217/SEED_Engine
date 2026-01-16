#include <SEED/Lib/Functions/FileFunc.h>
#include <Windows.h>
#include <shobjidl.h>

// usiing
namespace fs = std::filesystem;

namespace SEED {
    namespace Methods{
        namespace File{

            //////////////////////////////////////////////////////////////////////////////////////////////////////////
            // entry階層以下のフォルダリストを取得する関数
            //////////////////////////////////////////////////////////////////////////////////////////////////////////
            std::vector<std::filesystem::path> GetFolderList(const std::filesystem::path& entryPath, bool isRelative, bool isRecursive){

                std::vector<std::filesystem::path> folderList;

                if(isRecursive){
                    for(const auto& cur : fs::recursive_directory_iterator(entryPath)){
                        // ディレクトリのみを探す
                        if(cur.is_directory()){
                            if(isRelative){
                                folderList.push_back(fs::relative(cur.path(), entryPath).generic_string()); // 相対パスを格納
                            } else{
                                folderList.push_back(cur.path().generic_string()); // 絶対パスを格納
                            }
                        }
                    }
                } else{
                    for(const auto& cur : fs::directory_iterator(entryPath)){
                        // ディレクトリのみを探す
                        if(cur.is_directory()){
                            if(isRelative){
                                folderList.push_back(fs::relative(cur.path(), entryPath).generic_string()); // 相対パスを格納
                            } else{
                                folderList.push_back(cur.path().generic_string()); // 絶対パスを格納
                            }
                        }
                    }
                }

                return folderList;
            }

            //////////////////////////////////////////////////////////////////////////////////////////////////////////
            // entry階層以下の拡張子の合致するファイルリストを取得する関数
            //////////////////////////////////////////////////////////////////////////////////////////////////////////
            std::vector<std::filesystem::path> GetFileList(
                const std::filesystem::path& entryPath, std::initializer_list<std::string>extensions, bool isRelative
            ){
                std::vector<std::filesystem::path> fileList;

                for(const auto& cur : fs::recursive_directory_iterator(entryPath)){
                    // 拡張子が合致するファイルを探す
                    for(const auto& ext : extensions){
                        if(cur.is_regular_file() && cur.path().extension() == ext){
                            // ファイル名を格納
                            if(isRelative){
                                fileList.push_back(fs::relative(cur.path(), entryPath).generic_string()); // 相対パスを格納
                            } else{
                                fileList.push_back(cur.path().generic_string()); // 絶対パスを格納
                            }
                        }
                    }
                }

                return fileList;
            }



            //////////////////////////////////////////////////////////////////////////////////////////////////////////
            // entry階層以下に指定したファイル名が存在するかを確認し、あればそのパスを返す関数
            //////////////////////////////////////////////////////////////////////////////////////////////////////////
            std::filesystem::path FindFile(const std::filesystem::path& entryPath, const std::string& filename, bool isRelative){

                std::filesystem::path path = entryPath;
                for(const auto& cur : fs::recursive_directory_iterator(path)){
                    // 名前の合致するファイルを探す
                    if(cur.is_regular_file() && cur.path().filename() == filename){
                        // 存在する場合はパスを返す
                        if(isRelative){
                            return fs::relative(cur.path(), path).generic_string(); // 相対パスを返す
                        } else{
                            return cur.path().generic_string(); // 絶対パスを返す
                        }
                    }
                }
                return ""; // 存在しない場合は空文字列を返す
            }

            //////////////////////////////////////////////////////////////////////////////////////////////////////////
            // プロジェクトのルートディレクトリまでのフルパスを取得する関数
            //////////////////////////////////////////////////////////////////////////////////////////////////////////
            std::filesystem::path GetProjectDirectory(){
                // exeファイルの場所までのパスを取得
                char path[MAX_PATH];
                GetModuleFileNameA(NULL, path, MAX_PATH);
                auto exeDir = std::filesystem::path(path).parent_path();

                // 開発環境では exeDir の3階層上がプロジェクトルート
                auto candidate = exeDir.parent_path().parent_path().parent_path() / "SEED";
                if(std::filesystem::exists(candidate)){
                    return candidate;
                }

                // パッケージ化された実行環境では exeDir がプロジェクトルート
                return exeDir;
            }


            //////////////////////////////////////////////////////////////////////////////////////////////////////////
            // Jsonファイル関連
            //////////////////////////////////////////////////////////////////////////////////////////////////////////
            nlohmann::json GetJson(const std::filesystem::path& filePath, bool createFile){

                // ファイルを開く
                std::ifstream file(filePath);
                if(!file.is_open()){
                    if(createFile){
                        // ディレクトリを作成
                        std::filesystem::path path(filePath);
                        if(path.has_parent_path()){
                            std::filesystem::create_directories(path.parent_path());
                        }

                        // ファイル作成
                        std::ofstream newFile(filePath);
                        if(!newFile.is_open()){
                            assert(false && "Jsonファイルの作成に失敗しました");
                            return nlohmann::json();
                        }
                        newFile << "{}"; // 空のJSONオブジェクトを書き込む
                        newFile.close();
                    }


                    return nlohmann::json();
                }

                // JSONデータをパース
                nlohmann::json jsonData;
                file >> jsonData;
                return jsonData;
            }

            // Jsonファイルを作成する関数
            void CreateJsonFile(const std::filesystem::path& filePath, const nlohmann::json& jsonData){

                // 親ディレクトリを作成（なければ）
                std::filesystem::path path(filePath);
                if(path.has_parent_path()){
                    fs::create_directories(path.parent_path());
                }

                // ファイルを開く
                std::ofstream file(filePath);
                if(!file.is_open()){
                    assert(false && "Jsonファイルの作成に失敗");
                    return;
                }

                // JSONデータを書き込む
                file << jsonData.dump(4); // インデント幅を4に設定して書き込む
            }


            //////////////////////////////////////////////////////////////////////////////////////////////////////////
            // ファイル名の変更
            //////////////////////////////////////////////////////////////////////////////////////////////////////////
            bool RenameFile(const std::filesystem::path& oldFilePath, const std::filesystem::path& newFilePath){

                // 同じなら何もしない
                if(oldFilePath.generic_wstring() == newFilePath.generic_wstring()){
                    return true;
                }

                // ディレクトリと拡張子・基本名を分解
                std::filesystem::path dir = newFilePath.parent_path();
                std::wstring stem = newFilePath.stem().wstring();     // 拡張子を除いたファイル名
                std::wstring ext = newFilePath.extension().wstring(); // ".txt" など

                std::filesystem::path finalPath = newFilePath;
                int counter = 1;

                // 既に存在する場合は "(n)" を付けて回避
                while(fs::exists(finalPath)){
                    finalPath = dir / std::filesystem::path(stem + L"(" + std::to_wstring(counter++) + L")" + ext);
                }

                fs::rename(oldFilePath, finalPath);
                return true;
            }

            //////////////////////////////////////////////////////////////////////////////////////////////////////////
            // エクスプローラーで開く
            //////////////////////////////////////////////////////////////////////////////////////////////////////////
            void OpenInExplorer(const std::filesystem::path& path){

                // '/'を'\\'に変換
                std::string pathStr = path.string();
                std::replace(pathStr.begin(), pathStr.end(), '/', '\\');
                std::filesystem::path pathFixed(pathStr);

                // フォルダかファイルかで場合分け
                if(fs::is_directory(pathFixed)){
                    // フォルダの場合はそのフォルダを開く
                    ShellExecuteA(NULL, "open", pathFixed.string().c_str(), NULL, NULL, SW_SHOW);

                } else if(fs::is_regular_file(pathFixed)){
                    // ファイルの場合はそのファイルの場所を開く(parentPath)
                    auto parentPath = pathFixed.parent_path();
                    ShellExecuteA(NULL, "open", parentPath.string().c_str(), NULL, NULL, SW_SHOW);

                } else{
                    assert(false && "指定したパスが存在しません");
                }
            }

            //////////////////////////////////////////////////////////////////////////////////////////////////////////
            // ファイルを削除する
            //////////////////////////////////////////////////////////////////////////////////////////////////////////
            bool DeleteFileObject(const std::filesystem::path& path){

                // パスが存在するか確認
                if(!std::filesystem::exists(path)){
                    return false;
                }

                // 削除確認メッセージを出す
                std::wstring message = L"次のファイル（またはフォルダ）を削除しますか？\n\n" + path.generic_wstring();
                int res = MessageBoxW(nullptr, message.c_str(), L"削除の確認",
                    MB_YESNO | MB_ICONWARNING | MB_DEFBUTTON2);
                if(res != IDYES){
                    return false; // キャンセルされた
                }

                // 実際の削除
                std::error_code ec; // エラーコードを受け取るための変数
                fs::remove_all(path, ec);
                if(ec){
                    // エラーが発生した場合
                    std::wstring errorMessage = L"ファイル（またはフォルダ）の削除に失敗しました。\n\nエラー内容: " + String::ConvertString(ec.message());
                    MessageBoxW(nullptr, errorMessage.c_str(), L"削除エラー", MB_OK | MB_ICONERROR);
                    return false;
                }

                return true;
            }

            //////////////////////////////////////////////////////////////////////////////////////////////////////////
            // ファイルを保存するダイアログを表示する関数
            //////////////////////////////////////////////////////////////////////////////////////////////////////////
            std::filesystem::path OpenSaveFileDialog(const std::filesystem::path& directory, const std::string& ext, const std::string& initialName){
                // 保存ダイアログを表示するラムダ関数
                auto ShowSaveFileDialog = [](const std::wstring& title = L"名前を付けて保存",
                    const std::wstring& filter = L"すべてのファイル (*.*)\0*.*\0",
                    const std::wstring& defaultExt = L"",
                    const std::wstring& initialDir = L"",
                    const std::wstring& initName = L""
                    ){
                    // 現在のディレクトリを保存
                    char prevDir[MAX_PATH];
                    GetCurrentDirectoryA(MAX_PATH, prevDir);

                    // ファイル名を格納するバッファ
                    wchar_t filePath[MAX_PATH];
                    // 初期ファイル名をセット
                    if(!initName.empty()){
                        wcsncpy_s(filePath, initName.c_str(), MAX_PATH);
                    } else{
                        filePath[0] = L'\0'; // 空文字列で初期化
                    }

                    OPENFILENAMEW ofn{};
                    ofn.lStructSize = sizeof(ofn);
                    ofn.hwndOwner = nullptr; // 親ウィンドウハンドル（ImGui等から渡すなら指定してOK）
                    ofn.lpstrFilter = filter.c_str();
                    ofn.lpstrFile = filePath;
                    ofn.nMaxFile = MAX_PATH;
                    ofn.lpstrTitle = title.c_str();
                    ofn.lpstrDefExt = defaultExt.empty() ? nullptr : defaultExt.c_str();
                    ofn.lpstrInitialDir = initialDir.empty() ? nullptr : initialDir.c_str();
                    ofn.Flags = OFN_OVERWRITEPROMPT | OFN_PATHMUSTEXIST;

                    if(GetSaveFileNameW(&ofn)){
                        // カレントディレクトリを復元
                        SetCurrentDirectoryA(prevDir);
                        // ユーザーが保存したパス+ファイル名を返す
                        return std::wstring(filePath);
                    }
                    return std::wstring(); // キャンセル時
                };

                // directoryが存在しなければreturn
                if(!fs::exists(directory)){
                    return "";
                }

                // extの拡張子で保存されるようにする
                std::wstring wext = std::wstring(ext.begin(), ext.end());
                if(wext[0] == L'.') wext.erase(0, 1); // ".prefab" → "prefab"

                // filterを1文字ずつ安全に構築
                std::wstring filter;
                filter.append(L"Prefab ファイル (*.");
                filter.append(wext);
                filter.append(L")");
                filter.push_back(L'\0');
                filter.append(L"*.");
                filter.append(wext);
                filter.push_back(L'\0');
                filter.append(L"すべてのファイル (*.*)");
                filter.push_back(L'\0');
                filter.append(L"*.*");
                filter.push_back(L'\0');
                filter.push_back(L'\0'); // ← 末尾のダブルヌル終端

                std::wstring initName = String::ConvertString(initialName);
                return ShowSaveFileDialog(L"名前を付けて保存", filter, wext.substr(1), directory.generic_wstring(), initName);
            }


            //////////////////////////////////////////////////////////////////////////////////////////////////////////
            // 新しいフォルダを作成する関数
            //////////////////////////////////////////////////////////////////////////////////////////////////////////
            bool CreateNewFolder(const std::filesystem::path& directory, const std::string& folderName){

                // directoryが存在しなければreturn
                if(!fs::exists(directory)){
                    return false;
                }

                // wstring 変換
                std::filesystem::path newPath = directory / folderName;
                int counter = 1;

                // 既に存在する場合は "(n)" を付けて回避
                while(fs::exists(newPath)){
                    std::wstring numbered = String::ConvertString(folderName) + L"(" + std::to_wstring(counter++) + L")";
                    newPath = directory / std::filesystem::path(numbered);
                }

                // フォルダを作成
                return fs::create_directory(newPath);
            }
        }
    }
} // namespace SEED