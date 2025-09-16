#pragma once
#include <string>
#include <unordered_map>

class AudioDictionary{
public:
    static void Initialize();
    static const std::string& Get(const std::string& key) {

        auto it = dict.find(key);
        if(it != dict.end()){
            return it->second;
        }
        static const std::string emptyString = "";
        return emptyString;
    }
private:
    static inline std::unordered_map<std::string, std::string> dict{};
};