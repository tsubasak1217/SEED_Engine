#include "ErrorLog.h"
#include <Windows.h>
#include <fstream>
#include <filesystem>
#include <chrono>
#include <ctime>
#include <mutex>

namespace SEED{
    namespace Methods{

        void LogCriticalError(const std::string& message){

            OutputDebugStringA(("[SEED ERROR] " + message + "\n").c_str());

            static std::mutex logMutex;
            std::lock_guard<std::mutex> lock(logMutex);

            // 実行ファイルと同じフォルダに記録する(ANSIコードページに依存しないようワイド文字APIを使用)
            wchar_t exePath[MAX_PATH]{};
            GetModuleFileNameW(nullptr, exePath, MAX_PATH);
            std::filesystem::path logPath = std::filesystem::path(exePath).parent_path() / L"crash_log.txt";

            std::ofstream ofs(logPath, std::ios::app);
            if(!ofs.is_open()){
                return;
            }

            std::time_t t = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
            char timeBuf[32]{};
            ctime_s(timeBuf, sizeof(timeBuf), &t);
            std::string timeStr(timeBuf);
            if(!timeStr.empty() && timeStr.back() == '\n'){
                timeStr.pop_back();
            }

            ofs << "[" << timeStr << "] " << message << "\n";
        }
    }
} // namespace SEED
