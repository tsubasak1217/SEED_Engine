#pragma once
#include <random>
#include <cstdint>
#include <map>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <filesystem>
#include <initializer_list>
#include <json.hpp>
#include <SEED/Lib/Tensor/Vector3.h>
#include <SEED/Lib/Tensor/Vector2.h>
#include <SEED/Lib/Structs/Range1D.h>
#include <SEED/Lib/Structs/Range3D.h>
#include <SEED/Lib/Structs/Range2D.h>
#include <SEED/Lib/Structs/Transform.h>


class MyFunc{
private:
    // 乱数生成器を作成
    static std::random_device rd; // staticに変更
    static std::mt19937 gen; // staticに変更

public:
    // 指定した範囲の乱数を返す関数
    static int32_t Random(int min,int max);
    static float Random(float min,float max);
    static Vector3 Random(const Vector3& min,const Vector3& max);
    static Vector3 Random(const Range3D& range);
    static Vector2 Random(const Vector2& min,const Vector2& max);
    static Vector2 Random(const Range2D& range);
    static float Random(const Range1D& range);

    // 64ビットの乱数を返す関数
    static uint64_t RandomU64();

    // ランダムな方向を返す関数
    static Vector3 RandomVector();
    static Vector2 RandomVector2();

    // ランダムな色
    static Vector4 RandomColor();
    static Vector4 RandomColor(std::initializer_list<uint32_t>colorList,bool isCorrectionToLiner = true);

    // ランダムな方向(基準方向と許容角度から)
    static Vector3 RandomDirection(const Vector3& baseDirection, float angle);

    // thetaとphiからベクトルを生成する関数
    static Vector3 CreateVector(float theta,float phi);

    // 指定範囲を繰り返す関数 (最大値を超えたら最小値へ戻る)
    static int32_t Spiral(int32_t input,int32_t min,int32_t max);
    static float Spiral(float input,float min,float max);

    // 範囲に含まれているかを判定する関数
    static bool IsContain(const Range1D& range, float value);
    static bool IsContain(const Range2D& range, const Vector2& value);
    static bool IsContain(const Range3D& range, const Vector3& value);

    // ベクトルから三次元の回転角を算出する関数
    static Vector3 CalcRotateVec(const Vector3& vec);

    // マップのキーを検索して、特定のトークンを持つ要素を後ろに回す関数(何番目かも返す)
    template <typename T>
    static int32_t ToBack(std::map<std::string,T>& myMap,const std::string& token);

    // トランスフォームのL補間関数
    static Transform Interpolate(const Transform& a, const Transform& b, float t);
    static Transform2D Interpolate(const Transform2D& a, const Transform2D& b, float t);
    static Transform2D Interpolate(const std::vector<Transform2D>& transforms, float t);
    static Transform Interpolate(const std::vector<Transform>& transforms, float t);
    static Quaternion Interpolate(const std::vector<Quaternion>& transforms, float t);
    static Transform2D CatmullRomInterpolate(const std::vector<Transform2D>& transforms, float t, bool connectEdge = false);
    static Transform CatmullRomInterpolate(const std::vector<Transform>& transforms, float t,bool connectEdge = false);

   /// <summary>
   /// ある時間 t(0~1) での放物線の位置を計算
   /// </summary>
   /// <param name="_direction">飛ぶ方向</param>
   /// <param name="speed">初速</param>
   /// <param name="_time">時間 (0~1)</param>
   /// <param name="_gravity">重力加速度</param>
   /// <returns></returns>
    static  Vector2 CalculateParabolic(const Vector2& _direction,float speed,float _gravity,float _time);


public:// ファイル・文字列関連 ==============================================================

    // 文字列を変換する関数
    static std::wstring ConvertString(const std::string& str);
    static std::string ConvertString(const std::wstring& str);
    static std::string ToString(const std::u8string& u8str);

    // 文字コードを変換する関数
    static std::string UTF16ToUTF8(const std::wstring& utf16Str);
    static std::string UTF16ToUTF8(const std::string& utf16Str);
    static std::wstring UTF8ToUTF16(const std::string& utf8Str);

    // ポインタを文字列に変換する関数
    static std::string PtrToStr(const void* ptr);

    // 文字列を変換するhash関数
    static uint64_t Hash64(const std::string& str);
    static uint32_t Hash32(const std::string& str);

    // カテゴリ分け関数（0:英字, 1:かな, 2:数字, 3:その他）
    static int CharCategory(wchar_t ch);

    // 文字列の比較関数
    static bool CompareStr(const std::string& str1, const std::string& str2);

    // 指定したパスに存在するフォルダの一覧を取得する関数
    static std::vector<std::filesystem::path> GetFolderList(const std::filesystem::path& entryPath, bool isRelative, bool isRecursive);
    // 指定したパス以降で拡張子の合致するファイル一覧を取得する関数
    static std::vector<std::filesystem::path> GetFileList(
        const std::filesystem::path& entryPath, std::initializer_list<std::string>extensions,bool isRelative = true
    );

    // 指定したパス以降で名前の一致するファイルがあればそのパスを返す関数
    static std::filesystem::path FindFile(const std::filesystem::path& entryPath, const std::string& filename,bool isRelative = true);
    
    // projectのディレクトリまでのフルパスを取得する関数
    static std::filesystem::path GetProjectDirectory();

    // jsonふぁあいる関連の関数
    static nlohmann::json GetJson(const std::filesystem::path& filePath,bool createFile = false);
    static void CreateJsonFile(const std::filesystem::path& filePath, const nlohmann::json& jsonData);

    // ファイルのリネーム
    static bool RenameFile(const std::filesystem::path& oldFilePath, const std::filesystem::path& newFilePath);

    // エクスプローラーで開く
    static void OpenInExplorer(const std::filesystem::path& path);

    // ファイルの削除
    static bool DeleteFileObject(const std::filesystem::path& path);

    // ファイルの保存
    static std::filesystem::path OpenSaveFileDialog(const std::filesystem::path& directory, const std::string& ext, const std::string& initialName = "");

    // フォルダの新規作成
    static bool CreateNewFolder(const std::filesystem::path& directory, const std::string& folderName);
};





// マップのキーを検索して、特定のトークンを持つ要素を後ろに回す関数(何番目かも返す)
template<typename T>
inline int32_t MyFunc::ToBack(std::map<std::string,T>& myMap,const std::string& token){
    // 前後に分けるための一時コンテナ
    std::vector<std::pair<std::string,T>> withToken;
    std::vector<std::pair<std::string,T>> withoutToken;

    // 分ける処理
    for(const auto& [key,value] : myMap){
        if(key.find(token) != std::string::npos){
            withToken.push_back({key,value}); // トークンを含む
        } else{
            withoutToken.push_back({key,value}); // トークンを含まない
        }
    }

    // 元のマップをクリアして再挿入
    myMap.clear();
    for(const auto& pair : withoutToken){
        myMap.insert(pair);
    }
    for(const auto& pair : withToken){
        myMap.insert(pair);
    }

    // 何番目かを返す
    return withToken.size();
}