#include <SEED/Lib/Functions/MyFunc/MyFunc.h>
#include <SEED/Lib/Functions/MyFunc/MyMath.h>
#include <SEED/Lib/Tensor/Quaternion.h>
#include <Windows.h>
#include <shobjidl.h>

// usiing
namespace fs = std::filesystem;

// staticメンバーの定義
std::random_device MyFunc::rd;
std::mt19937 MyFunc::gen(MyFunc::rd());

//////////////////////////////////////////////////////////////////////////////////////////////////////////
// 指定した範囲の乱数を返す関数
//////////////////////////////////////////////////////////////////////////////////////////////////////////

//------------------ int ------------------//
int32_t MyFunc::Random(int min, int max){

    // もしminがmaxより大きい場合は入れ替える
    if(min > max){ std::swap(min, max); }

    // minからmaxまでの一様分布を設定
    std::uniform_int_distribution<> distrib(min, max);

    // 乱数を生成して返す
    return distrib(gen);
}

//------------------ float ------------------//
float MyFunc::Random(float min, float max){

    // もしminがmaxより大きい場合は入れ替える
    if(min > max){ std::swap(min, max); }

    // minからmaxまでの一様分布を設定 (float用)
    std::uniform_real_distribution<float> distrib(min, max);

    // 乱数を生成して返す
    return distrib(gen);
}

//------------------ uint64_t ------------------//
uint64_t MyFunc::RandomU64(){
    int32_t hi = Random(0, INT32_MAX);
    int32_t lo = Random(0, INT32_MAX);

    // 64ビットに合成
    //  hi を上位32ビットに, lo を下位32ビットに詰める
    uint64_t high = static_cast<uint64_t>(hi);
    uint64_t low = static_cast<uint64_t>(lo);
    return (high << 32) | low;
}

//----------------- 3次元ベクトル用 -----------------//

Vector3 MyFunc::Random(const Vector3& min, const Vector3& max){
    return { Random(min.x,max.x),Random(min.y,max.y),Random(min.z,max.z) };
}

Vector3 MyFunc::Random(const Range3D& range){
    return Random(range.min, range.max);
}

//----------------- 2次元ベクトル用 -----------------//

Vector2 MyFunc::Random(const Vector2& min, const Vector2& max){
    return { Random(min.x,max.x),Random(min.y,max.y) };
}

Vector2 MyFunc::Random(const Range2D& range){
    return Random(range.min, range.max);
}

float MyFunc::Random(const Range1D& range){
    return Random(range.min, range.max);
}



//--------------- ランダムな方向を返す関数 ---------------//

Vector3 MyFunc::RandomVector(){
    // ランダムなthetaとphiの範囲
    float theta = MyFunc::Random(0.0f, 2.0f * 3.14159265358979323846f); // 0 ~ 2π
    float phi = MyFunc::Random(0.0f, 3.14159265358979323846f); // 0 ~ π

    // 球座標から直交座標への変換
    float x = std::sin(phi) * std::cos(theta);
    float y = std::sin(phi) * std::sin(theta);
    float z = std::cos(phi);

    return { x,y,z };
}

Vector2 MyFunc::RandomVector2(){
    // ランダムなthetaの範囲
    float theta = MyFunc::Random(0.0f, 2.0f * 3.14159265358979323846f); // 0 ~ 2π

    // 球座標から直交座標への変換
    float x = std::cos(theta);
    float y = std::sin(theta);

    return { x,y };
}

//----------------- ランダムな色を返す関数 -----------------//
Vector4 MyFunc::RandomColor(){
    return { Random(0.0f,1.0f),Random(0.0f,1.0f),Random(0.0f,1.0f),1.0f };
}

Vector4 MyFunc::RandomColor(std::initializer_list<uint32_t> colorList, bool isCorrectionToLiner){
    if(colorList.size() == 0){
        return { 1.0f,1.0f,1.0f,1.0f }; // デフォルトの白色
    }

    // ランダムに選択
    auto it = std::next(colorList.begin(), Random(0, static_cast<int>(colorList.size()) - 1));
    uint32_t color = *it;
    // RGBA成分を抽出して返す
    return MyMath::FloatColor(color, isCorrectionToLiner);
}

//----------------- ランダムな方向を返す関数 -----------------//
Vector3 MyFunc::RandomDirection(const Vector3& baseDirection, float angle){
    // ランダムなthetaとphiの範囲
    float theta = MyFunc::Random(-angle, angle); // -angle ~ angle
    float phi = MyFunc::Random(-angle / 2.0f, angle / 2.0f); // -angle/2 ~ angle/2
    // 球座標から直交座標への変換
    float x = std::sin(phi) * std::cos(theta);
    float y = std::sin(phi) * std::sin(theta);
    float z = std::cos(phi);
    return baseDirection * Quaternion::DirectionToDirection({ 1.0f,0.0f,0.0f }, { x,y,z });
}

//----------------- thetaとphiからベクトルを生成する関数 -----------------//
Vector3 MyFunc::CreateVector(float theta, float phi){
    // 球座標から直交座標への変換（左手座標系用）
    float x = std::sin(phi) * std::cos(theta);
    float y = std::cos(phi);
    float z = std::sin(phi) * std::sin(theta);

    return { x,y,z };
}



//////////////////////////////////////////////////////////////////////////////////////////////////////////
// 指定範囲を繰り返す関数 (最大値を超えたら最小値へ戻る)
//////////////////////////////////////////////////////////////////////////////////////////////////////////

int32_t MyFunc::Spiral(int32_t input, int32_t min, int32_t max){

    if(max < min){
        assert(false);
    }

    if(min == max){
        return min; // minとmaxが同じ場合はその値を返す
    }

    int32_t range = max - min + 1; // 🔧 範囲のサイズ
    int32_t offset = input - min;

    // 正の mod を保つようにする（負数対応）
    int32_t wrapped = ((offset % range) + range) % range;

    return min + wrapped;

}


float MyFunc::Spiral(float input, float min, float max){

    if(max < min){
        return Spiral(input, max, min);
    }



    float range = max - min;

    if(input > max){

        float sub = input - max;
        return min + std::fmod(sub, range);

    } else if(input < min){

        float sub = input - min;
        return max + std::fmod(sub, range);
    }

    return input;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////
// 範囲に含まれているかを判定する関数
//////////////////////////////////////////////////////////////////////////////////////////////////////////
bool MyFunc::IsContain(const Range1D& range, float value){
    return range.min <= value && value <= range.max;
}

bool MyFunc::IsContain(const Range2D& range, const Vector2& value){
    Range1D xRange = { range.min.x,range.max.x };
    Range1D yRange = { range.min.y,range.max.y };
    return IsContain(xRange, value.x) && IsContain(yRange, value.y);
}

bool MyFunc::IsContain(const Range3D& range, const Vector3& value){
    Range1D xRange = { range.min.x,range.max.x };
    Range1D yRange = { range.min.y,range.max.y };
    Range1D zRange = { range.min.z,range.max.z };
    return IsContain(xRange, value.x) && IsContain(yRange, value.y) && IsContain(zRange, value.z);
}



//////////////////////////////////////////////////////////////////////////////////////////////////////////
// ベクトルから三次元の回転角を算出する関数
//////////////////////////////////////////////////////////////////////////////////////////////////////////
Vector3 MyFunc::CalcRotateVec(const Vector3& vec){
    Vector3 rotate = { 0.0f,0.0f,0.0f };

    // ベクトルの長さを計算
    float length = MyMath::Length(vec);
    if(length != 0.0f){
        // 正規化されたベクトル
        Vector3 normalizedVec = vec / length;

        // X軸回りの回転角
        rotate.x = -std::asin(normalizedVec.y); // Y成分で縦方向の角度を決定

        // Y軸回りの回転角
        rotate.y = std::atan2(normalizedVec.x, normalizedVec.z); // XとZの比率で横方向の角度を決定
    }

    return rotate;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////
// トランスフォームの補間を行う関数
//////////////////////////////////////////////////////////////////////////////////////////////////////////

// 3D
Transform MyFunc::Interpolate(const Transform& a, const Transform& b, float t){
    Transform result;
    // 位置の補間
    result.translate = MyMath::Lerp(a.translate, b.translate, t);
    // 回転の補間（球面線形補間を使用）
    result.rotate = Quaternion::Slerp(a.rotate, b.rotate, t);
    // スケールの補間
    result.scale = MyMath::Lerp(a.scale, b.scale, t);
    return result;
}

// 2D
Transform2D MyFunc::Interpolate(const Transform2D& a, const Transform2D& b, float t){
    Transform2D result;
    // 位置の補間
    result.translate = MyMath::Lerp(a.translate, b.translate, t);
    // 回転の補間
    result.rotate = MyMath::Lerp(a.rotate, b.rotate, t);
    // スケールの補間
    result.scale = MyMath::Lerp(a.scale, b.scale, t);
    return result;
}

// 2D 複数のトランスフォームから補間
Transform2D MyFunc::Interpolate(const std::vector<Transform2D>& transforms, float t){
    int32_t size = static_cast<int32_t>(transforms.size());
    int32_t currentIdx = MyMath::CalcElement(t, size);
    float sectionT = MyMath::CalcSectionT(t, size);
    return Interpolate(transforms[currentIdx], transforms[std::clamp(currentIdx + 1, 0, size - 1)], sectionT);
}

// 3D 複数のトランスフォームから補間
Transform MyFunc::Interpolate(const std::vector<Transform>& transforms, float t){
    int32_t size = static_cast<int32_t>(transforms.size());
    int32_t currentIdx = MyMath::CalcElement(t, size);
    float sectionT = MyMath::CalcSectionT(t, size);
    return Interpolate(transforms[currentIdx], transforms[std::clamp(currentIdx + 1, 0, size - 1)], sectionT);
}

// Quaternionを複数配列から補間
Quaternion MyFunc::Interpolate(const std::vector<Quaternion>& transforms, float t){
    int32_t size = static_cast<int32_t>(transforms.size());
    int32_t currentIdx = MyMath::CalcElement(t, size);
    float sectionT = MyMath::CalcSectionT(t, size);
    return Quaternion::Slerp(transforms[currentIdx], transforms[std::clamp(currentIdx + 1, 0, size - 1)], sectionT);
}

// 2D Catmull-Rom補間
Transform2D MyFunc::CatmullRomInterpolate(const std::vector<Transform2D>& transforms, float t){
    Transform2D result;

    // 各成分をそれぞれ配列に分解
    std::vector<Vector2> scales;
    std::vector<float> rotations;
    std::vector<Vector2> positions;
    for(const auto& tr : transforms){
        scales.push_back(tr.scale);
        rotations.push_back(tr.rotate);
        positions.push_back(tr.translate);
    }

    // 結果の各成分を求める
    result.scale = MyMath::CatmullRomPosition(scales, t);
    result.rotate = MyMath::CatmullRomPosition(rotations, t);
    result.translate = MyMath::CatmullRomPosition(positions, t);
    return result;
}

// 3D Catmull-Rom補間
Transform MyFunc::CatmullRomInterpolate(const std::vector<Transform>& transforms, float t){
    Transform result;

    // 各成分をそれぞれ配列に分解
    std::vector<Vector3> scales;
    std::vector<Quaternion> rotations;
    std::vector<Vector3> positions;
    for(const auto& tr : transforms){
        scales.push_back(tr.scale);
        rotations.push_back(tr.rotate);
        positions.push_back(tr.translate);
    }

    // 結果の各成分を求める
    result.scale = MyMath::CatmullRomPosition(scales, t);
    result.rotate = Interpolate(rotations, t);
    result.translate = MyMath::CatmullRomPosition(positions, t);
    return result;
}


//////////////////////////////////////////////////////////////////////////////////////////////////////
// 放物線の計算
//////////////////////////////////////////////////////////////////////////////////////////////////////
Vector2 MyFunc::CalculateParabolic(const Vector2& _direction, float _speed, float _time, float _gravity){
    float x = _direction.x * _speed * _time;
    float y = _direction.y * _speed * _time - 0.5f * _gravity * _time * _time;
    return Vector2(x, y);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////
// 文字列を変換する関数
//////////////////////////////////////////////////////////////////////////////////////////////////////////
std::wstring MyFunc::ConvertString(const std::string& str){
    if(str.empty()){
        return std::wstring();
    }

    auto sizeNeeded = MultiByteToWideChar(CP_UTF8, 0, reinterpret_cast<const char*>(&str[0]), static_cast<int>(str.size()), NULL, 0);
    if(sizeNeeded == 0){
        return std::wstring();
    }
    std::wstring result(sizeNeeded, 0);
    MultiByteToWideChar(CP_UTF8, 0, reinterpret_cast<const char*>(&str[0]), static_cast<int>(str.size()), &result[0], sizeNeeded);
    return result;
}

std::string MyFunc::ConvertString(const std::wstring& str){
    if(str.empty()){
        return std::string();
    }

    auto sizeNeeded = WideCharToMultiByte(CP_UTF8, 0, str.data(), static_cast<int>(str.size()), NULL, 0, NULL, NULL);
    if(sizeNeeded == 0){
        return std::string();
    }
    std::string result(sizeNeeded, 0);
    WideCharToMultiByte(CP_UTF8, 0, str.data(), static_cast<int>(str.size()), result.data(), sizeNeeded, NULL, NULL);
    return result;
}

std::string MyFunc::ToString(const std::u8string& u8str){
    return std::string(reinterpret_cast<const char*>(u8str.data()), u8str.size());
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////
// 文字コードを変換する関数
//////////////////////////////////////////////////////////////////////////////////////////////////////////
std::string MyFunc::UTF16ToUTF8(const std::wstring& utf16Str){
    if(utf16Str.empty()) return "";
    int sizeNeeded = WideCharToMultiByte(CP_UTF8, 0, utf16Str.c_str(), -1, nullptr, 0, nullptr, nullptr);
    std::string result(sizeNeeded, 0);
    WideCharToMultiByte(CP_UTF8, 0, utf16Str.c_str(), -1, &result[0], sizeNeeded, nullptr, nullptr);
    result.pop_back(); // 終端の null を削除
    return result;
}

std::string MyFunc::UTF16ToUTF8(const std::string& utf16Str){
    return UTF16ToUTF8(ConvertString(utf16Str));
}

std::wstring MyFunc::UTF8ToUTF16(const std::string& utf8Str){
    if(utf8Str.empty()) return L"";
    int sizeNeeded = MultiByteToWideChar(CP_UTF8, 0, utf8Str.c_str(), -1, nullptr, 0);
    std::wstring result(sizeNeeded, 0);
    MultiByteToWideChar(CP_UTF8, 0, utf8Str.c_str(), -1, &result[0], sizeNeeded);
    result.pop_back(); // 終端の null を削除
    return result;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////
// ポインタを文字列に変換する関数
//////////////////////////////////////////////////////////////////////////////////////////////////////////
std::string MyFunc::PtrToStr(const void* ptr){
    std::ostringstream oss;
    oss << ptr;
    return oss.str();
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////
// 文字列のハッシュ値を計算する関数
//////////////////////////////////////////////////////////////////////////////////////////////////////////
uint64_t MyFunc::Hash64(const std::string& str){
    uint64_t hash = 14695981039346656037ull; // FNV offset basis (64bit)
    for(char c : str){
        hash ^= static_cast<uint8_t>(c);
        hash *= 1099511628211ull; // FNV prime (64bit)
    }
    return hash;
}

uint32_t MyFunc::Hash32(const std::string& str){
    uint32_t hash = 2166136261u; // FNV offset basis (32bit)
    for(char c : str){
        hash ^= static_cast<uint8_t>(c);
        hash *= 16777619u; // FNV prime (32bit)
    }
    return hash;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////
// 文字のカテゴリを判定する関数
//////////////////////////////////////////////////////////////////////////////////////////////////////////
int MyFunc::CharCategory(wchar_t ch){
    if((ch >= L'a' && ch <= L'z') || (ch >= L'A' && ch <= L'Z')) return 0; // 英字
    if((ch >= L'ぁ' && ch <= L'ん') || (ch >= L'ァ' && ch <= L'ン')) return 1; // ひらがな・カタカナ
    if(ch >= L'0' && ch <= L'9') return 2; // 数字
    return 3; // その他
}


//////////////////////////////////////////////////////////////////////////////////////////////////////////
// 文字列を比較する関数(str1の方が若いとtrue)
//////////////////////////////////////////////////////////////////////////////////////////////////////////
bool MyFunc::CompareStr(const std::string& str1, const std::string& str2){
    // wstring 変換
    std::wstring wS1 = ConvertString(str1);
    std::wstring wS2 = ConvertString(str2);

    size_t len = (std::min)(wS1.size(), wS2.size());
    for(size_t i = 0; i < len; ++i){
        // 文字のカテゴリを取得
        int ca = CharCategory(wS1[i]);
        int cb = CharCategory(wS2[i]);
        // カテゴリが違えばカテゴリ順
        if(ca != cb) return ca < cb;
        // 同カテゴリ内では文字コード順
        if(wS1[i] != wS2[i]) return wS1[i] < wS2[i];
    }

    // 長さ順
    return wS1.size() < wS2.size();
}


//////////////////////////////////////////////////////////////////////////////////////////////////////////
// entry階層以下のフォルダリストを取得する関数
//////////////////////////////////////////////////////////////////////////////////////////////////////////
std::vector<std::string> MyFunc::GetFolderList(const std::string& entryPath, bool isRelative){
    std::vector<std::string> folderList;
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
    return folderList;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////
// entry階層以下の拡張子の合致するファイルリストを取得する関数
//////////////////////////////////////////////////////////////////////////////////////////////////////////
std::vector<std::string> MyFunc::GetFileList(
    const std::string& entryPath, std::initializer_list<std::string>extensions, bool isRelative
){

    std::vector<std::string> fileList;

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
// entry階層以下のディレクトリオブジェクトを取得する関数
//////////////////////////////////////////////////////////////////////////////////////////////////////////
std::vector<std::string> MyFunc::GetDirectoryNameList(const std::string& entryPath, bool isRelative, bool isRecursive){

    std::vector<std::string> dirList;
    if(isRecursive){
        for(const auto& cur : fs::recursive_directory_iterator(entryPath)){
            // ディレクトリのみを探す
            if(cur.is_directory()){
                if(isRelative){
                    dirList.push_back(fs::relative(cur.path(), entryPath).generic_string()); // 相対パスを格納
                } else{
                    dirList.push_back(cur.path().generic_string()); // 絶対パスを格納
                }
            }
        }
    } else{
        for(const auto& cur : fs::directory_iterator(entryPath)){
            // ディレクトリのみを探す
            if(cur.is_directory()){
                if(isRelative){
                    dirList.push_back(fs::relative(cur.path(), entryPath).generic_string()); // 相対パスを格納
                } else{
                    dirList.push_back(cur.path().generic_string()); // 絶対パスを格納
                }
            }
        }
    }
    return dirList;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////
// entry階層以下のディレクトリエントリを取得する関数
//////////////////////////////////////////////////////////////////////////////////////////////////////////
std::vector<std::filesystem::directory_entry> MyFunc::GetDirectoryEntryList(const std::string& entryPath, bool isRecursive){

    std::vector<std::filesystem::directory_entry> entryList;
    if(isRecursive){
        for(const auto& cur : fs::recursive_directory_iterator(entryPath)){
            // ディレクトリエントリを格納
            entryList.push_back(cur);
        }
    } else{
        for(const auto& cur : fs::directory_iterator(entryPath)){
            // ディレクトリエントリを格納
            entryList.push_back(cur);
        }
    }
    return entryList;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////
// entry階層以下に指定したファイル名が存在するかを確認し、あればそのパスを返す関数
//////////////////////////////////////////////////////////////////////////////////////////////////////////
std::string MyFunc::FindFile(const std::string& entryPath, const std::string& filePath, bool isRelative){
    for(const auto& cur : fs::recursive_directory_iterator(entryPath)){
        // 名前の合致するファイルを探す
        if(cur.is_regular_file() && cur.path().filename() == filePath){
            // 存在する場合はパスを返す
            if(isRelative){
                return fs::relative(cur.path(), entryPath).generic_string(); // 相対パスを返す
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
std::filesystem::path MyFunc::GetProjectDirectory(){
    // exeファイルの場所までのパスを取得
    char path[MAX_PATH];
    GetModuleFileNameA(NULL, path, MAX_PATH);
    auto exeDir = std::filesystem::path(path).parent_path();

    // Debug/Release → EXE → Build ->SEED_Engine -> SEED
    return exeDir.parent_path().parent_path().parent_path() / "SEED";
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////
// ファイル名のみ抜き出す関数
//////////////////////////////////////////////////////////////////////////////////////////////////////////
std::string MyFunc::ExtractFileName(const std::string& filePath, bool isContainExt){
    // ファイルパスからファイル名を取得
    std::filesystem::path path(filePath);
    std::string fileName = path.filename().generic_string(); // ファイル名を取得
    if(isContainExt){
        return fileName; // 拡張子を含む場合はそのまま返す
    } else{
        // 拡張子を除去して返す
        return path.stem().generic_string(); // 拡張子を除去したファイル名を返す
    }
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////
// ProjectDirからの相対パスをユーザーのフルパスに変換する関数
//////////////////////////////////////////////////////////////////////////////////////////////////////////
std::string MyFunc::ToFullPath(const std::string& relativePath){
    std::string fullPath = GetProjectDirectory().generic_string() + "/" + relativePath;
    // フルパスを正規化して返す
    return std::filesystem::canonical(fullPath).generic_string();
}

std::wstring MyFunc::ToFullPath(const std::wstring& relativePath){
    std::wstring fullPath = GetProjectDirectory().generic_wstring() + L"/" + relativePath;
    // フルパスを正規化して返す
    return std::filesystem::canonical(fullPath).generic_wstring();
}


//////////////////////////////////////////////////////////////////////////////////////////////////////////
// Jsonファイル関連
//////////////////////////////////////////////////////////////////////////////////////////////////////////
nlohmann::json MyFunc::GetJson(const std::string& filePath, bool createFile){

    // フルパスに変換
    std::string fullPath = ToFullPath(filePath);
    // '/'を'\\'に変換
    std::replace(fullPath.begin(), fullPath.end(), '/', '\\');

    // ファイルを開く
    std::ifstream file(filePath);
    if(!file.is_open()){
        if(createFile){
            // ファイルが存在しない場合は新規作成
            std::ofstream newFile(filePath);
            if(!newFile.is_open()){
                assert(false && "Jsonファイルの作成に失敗");
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
void MyFunc::CreateJsonFile(const std::string& filePath, const nlohmann::json& jsonData){

    // 親ディレクトリを作成（なければ）
    fs::path path(filePath);
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
bool MyFunc::RenameFile(const std::string& oldFilePath, const std::string& newFilePath){

    // wstringに変換
    std::string fixedOldPath = oldFilePath.c_str();
    std::string fixedNewPath = newFilePath.c_str();
    std::replace(fixedOldPath.begin(), fixedOldPath.end(), '/', '\\');
    std::replace(fixedNewPath.begin(), fixedNewPath.end(), '/', '\\');
    std::wstring wOldPath = ConvertString(fixedOldPath);
    std::wstring wNewPath = ConvertString(fixedNewPath);

    // 同じなら何もしない
    if(wOldPath == wNewPath){
        return true;
    }

    // パスにする
    fs::path src(wOldPath);
    fs::path dst(wNewPath);

    // ディレクトリと拡張子・基本名を分解
    fs::path dir = dst.parent_path();
    std::wstring stem = dst.stem().wstring();     // 拡張子を除いたファイル名
    std::wstring ext = dst.extension().wstring(); // ".txt" など

    fs::path finalPath = dst;
    int counter = 1;

    // 既に存在する場合は "(n)" を付けて回避
    while(fs::exists(finalPath)){
        finalPath = dir / fs::path(stem + L"(" + std::to_wstring(counter++) + L")" + ext);
    }

    fs::rename(src, finalPath);
    return true;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////
// エクスプローラーで開く
//////////////////////////////////////////////////////////////////////////////////////////////////////////
void MyFunc::OpenInExplorer(const std::string& path){
    // '/'を'\\'に変換
    std::string fixedPath = path.c_str();
    std::replace(fixedPath.begin(), fixedPath.end(), '/', '\\');
    std::wstring wpath = ConvertString(fixedPath);

    // パスからファイル名を除去
    fs::path fsPath(wpath);

    if(fs::is_directory(fsPath)){
        // フォルダの場合はそのフォルダを開く
        ShellExecuteA(NULL, "open", fsPath.string().c_str(), NULL, NULL, SW_SHOW);
    } else if(fs::is_regular_file(fsPath)){
        // ファイルの場合はそのファイルの場所を開く(parentPath)
        ShellExecuteA(NULL, "open", fsPath.parent_path().string().c_str(), NULL, NULL, SW_SHOW);

    } else{
        assert(false && "指定したパスが存在しません");
    }
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////
// ファイルを削除する
//////////////////////////////////////////////////////////////////////////////////////////////////////////
bool MyFunc::DeleteFileObject(const std::string& path){

    // '/'を'\\'に変換
    std::string fixedPath = path.c_str();
    std::replace(fixedPath.begin(), fixedPath.end(), '/', '\\');
    std::wstring wpath = ConvertString(fixedPath);
    
    // パスが存在するか確認
    if(!std::filesystem::exists(wpath)){
        return false;
    }

    // 削除確認メッセージを出す
    std::wstring message = L"次のファイル（またはフォルダ）を削除しますか？\n\n" + wpath;
    int res = MessageBoxW(nullptr, message.c_str(), L"削除の確認",
        MB_YESNO | MB_ICONWARNING | MB_DEFBUTTON2);
    if(res != IDYES){
        return false; // キャンセルされた
    }

    // 実際の削除
    std::error_code ec; // エラーコードを受け取るための変数
    fs::remove_all(wpath, ec);
    if(ec){
        // エラーが発生した場合
        std::wstring errorMessage = L"ファイル（またはフォルダ）の削除に失敗しました。\n\nエラー内容: " + ConvertString(ec.message());
        MessageBoxW(nullptr, errorMessage.c_str(), L"削除エラー", MB_OK | MB_ICONERROR);
        return false;
    }

    return true;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////
// ファイルを保存するダイアログを表示する関数
//////////////////////////////////////////////////////////////////////////////////////////////////////////
std::string MyFunc::OpenSaveFileDialog(const std::string& directory, const std::string& ext, const std::string& initialName){
    // 保存ダイアログを表示するラムダ関数
    auto ShowSaveFileDialog = [](const std::wstring& title = L"名前を付けて保存",
        const std::wstring& filter = L"すべてのファイル (*.*)\0*.*\0",
        const std::wstring& defaultExt = L"",
        const std::wstring& initialDir = L"",
        const std::wstring& initName = L""
        ){
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
            return std::wstring(filePath); // ユーザーが選択したパス
        }
        return std::wstring(); // キャンセル時
    };

    // 絶対パスに変換
    std::string absDirectory = ToFullPath(directory);
    // '/'を'\\'に変換
    std::replace(absDirectory.begin(), absDirectory.end(), '/', '\\');

    // directoryが存在しなければreturn
    if(!fs::exists(absDirectory)){
        return "";
    }

    // extの拡張子で保存されるようにする
    std::wstring wExt = ConvertString(ext);
    std::wstring filter = L"指定の拡張子 (*" + wExt + L")\0*" + wExt + L"\0すべてのファイル (*.*)\0*.*\0";
    std::wstring initialDir = ConvertString(absDirectory);
    std::wstring initName = ConvertString(initialName);
    return ConvertString(ShowSaveFileDialog(L"名前を付けて保存", filter, wExt.substr(1), initialDir, initName));
}


//////////////////////////////////////////////////////////////////////////////////////////////////////////
// 新しいフォルダを作成する関数
//////////////////////////////////////////////////////////////////////////////////////////////////////////
bool MyFunc::CreateNewFolder(const std::string& directory, const std::string& folderName){
    // 絶対パスに変換
    std::string absDirectory = ToFullPath(directory);

    // '/'を'\\'に変換
    std::replace(absDirectory.begin(), absDirectory.end(), '/', '\\');

    // directoryが存在しなければreturn
    if(!fs::exists(absDirectory)){
        return false;
    }

    // wstring 変換
    std::wstring wAbsDir = ConvertString(absDirectory);
    std::wstring wFolder = ConvertString(folderName);
    fs::path newPath = fs::path(wAbsDir) / fs::path(wFolder);
    int counter = 1;

    // 既に存在する場合は "(n)" を付けて回避
    while(fs::exists(newPath)){
        std::wstring numbered = wFolder + L"(" + std::to_wstring(counter++) + L")";
        newPath = fs::path(wAbsDir) / fs::path(numbered);
    }

    // フォルダを作成
    return fs::create_directory(newPath);
}