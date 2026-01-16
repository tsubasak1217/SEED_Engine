#include <SEED/Lib/Functions/StringFunc.h>
#include <Windows.h>

// usiing
namespace fs = std::filesystem;

namespace SEED{
    namespace Methods{
        namespace String{
            //////////////////////////////////////////////////////////////////////////////////////////////////////////
            // 文字列を変換する関数
            //////////////////////////////////////////////////////////////////////////////////////////////////////////
            std::wstring ConvertString(const std::string& str){
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

            std::string ConvertString(const std::wstring& str){
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

            std::string ToString(const std::u8string& u8str){
                return std::string(reinterpret_cast<const char*>(u8str.data()), u8str.size());
            }

            //////////////////////////////////////////////////////////////////////////////////////////////////////////
            // 文字コードを変換する関数
            //////////////////////////////////////////////////////////////////////////////////////////////////////////
            std::string UTF16ToUTF8(const std::wstring& utf16Str){
                if(utf16Str.empty()) return "";
                int sizeNeeded = WideCharToMultiByte(CP_UTF8, 0, utf16Str.c_str(), -1, nullptr, 0, nullptr, nullptr);
                std::string result(sizeNeeded, 0);
                WideCharToMultiByte(CP_UTF8, 0, utf16Str.c_str(), -1, &result[0], sizeNeeded, nullptr, nullptr);
                result.pop_back(); // 終端の null を削除
                return result;
            }

            std::string UTF16ToUTF8(const std::string& utf16Str){
                return UTF16ToUTF8(ConvertString(utf16Str));
            }

            std::wstring UTF8ToUTF16(const std::string& utf8Str){
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
            std::string PtrToStr(const void* ptr){
                std::ostringstream oss;
                oss << ptr;
                return oss.str();
            }

            //////////////////////////////////////////////////////////////////////////////////////////////////////////
            // 文字列のハッシュ値を計算する関数
            //////////////////////////////////////////////////////////////////////////////////////////////////////////
            uint64_t Methods::String::Hash64(const std::string& str){
                uint64_t hash = 14695981039346656037ull; // FNV offset basis (64bit)
                for(char c : str){
                    hash ^= static_cast<uint8_t>(c);
                    hash *= 1099511628211ull; // FNV prime (64bit)
                }
                return hash;
            }

            uint32_t Hash32(const std::string& str){
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
            int CharCategory(wchar_t ch){
                if((ch >= L'a' && ch <= L'z') || (ch >= L'A' && ch <= L'Z')) return 0; // 英字
                if((ch >= L'ぁ' && ch <= L'ん') || (ch >= L'ァ' && ch <= L'ン')) return 1; // ひらがな・カタカナ
                if(ch >= L'0' && ch <= L'9') return 2; // 数字
                return 3; // その他
            }


            //////////////////////////////////////////////////////////////////////////////////////////////////////////
            // 文字列を比較する関数(str1の方が若いとtrue)
            //////////////////////////////////////////////////////////////////////////////////////////////////////////
            bool CompareStr(const std::string& str1, const std::string& str2){
                // wstring 変換
                std::wstring wS1 = Methods::String::ConvertString(str1);
                std::wstring wS2 = Methods::String::ConvertString(str2);

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

        } // namespace SEED
    }
}