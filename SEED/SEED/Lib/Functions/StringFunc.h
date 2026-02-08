#pragma once
#include <cstdint>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <filesystem>
#include <initializer_list>
#include <json.hpp>

namespace SEED{
    namespace Methods{
        namespace String{

            // 文字列を変換する関数
            std::wstring ConvertString(const std::string& str);
            std::string ConvertString(const std::wstring& str);
            std::string ToString(const std::u8string& u8str);

            // 文字コードを変換する関数
            std::string UTF16ToUTF8(const std::wstring& utf16Str);
            std::string UTF16ToUTF8(const std::string& utf16Str);
            std::wstring UTF8ToUTF16(const std::string& utf8Str);

            // ポインタを文字列に変換する関数
            std::string PtrToStr(const void* ptr);

            // 文字列を変換するhash関数
            uint64_t Hash64(const std::string& str);
            uint32_t Hash32(const std::string& str);

            // カテゴリ分け関数（0:英字, 1:かな, 2:数字, 3:その他）
            int CharCategory(wchar_t ch);

            // 文字列の比較関数
            bool CompareStr(const std::string& str1, const std::string& str2);
        };
    }
} // namespace SEED