#pragma once
#include <string>
#include <unordered_map>

class AudioDictionary{
public:
    static const std::string& Get(const std::string& key) {

        static bool isInitialized = false;

        if(!isInitialized){
            Initialize();
        }

        auto it = dict.find(key);
        if(it != dict.end()){
            return it->second;
        }
        static const std::string emptyString = "";
        return emptyString;
    }

private:
    static void Initialize();

public:
    static inline std::unordered_map<std::string, std::string> dict{};
};