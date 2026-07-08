#pragma once
#include <string>
#include <functional>
#include <future>
#include <mutex>
#include <vector>
#include <utility>

namespace SEED {

class HttpManager {
private:
    HttpManager() = default;
    HttpManager(const HttpManager&) = delete;
    void operator=(const HttpManager&) = delete;
    inline static HttpManager* instance_ = nullptr;

public:
    ~HttpManager() = default;
    static HttpManager* GetInstance();
    static void Initialize();
    static void Release();

    static void Update();

public:
    using Callback = std::function<void(bool success, const std::string& body)>;

    static void RequestAsync(
        const std::string& method, const std::string& url,
        const std::string& body, Callback callback,
        const std::string& token = ""
    );

    static std::string RequestSync(
        const std::string& method, const std::string& url,
        const std::string& body, bool* outSuccess = nullptr,
        const std::string& token = ""
    );

    static void GetAsync(const std::string& url, Callback callback, const std::string& token = "");
    static std::string GetSync(const std::string& url, bool* outSuccess = nullptr, const std::string& token = "");

private:
    struct PendingRequest {
        std::future<std::pair<bool, std::string>> future;
        Callback callback;
    };

    std::mutex mutex_;
    std::vector<PendingRequest> pendingRequests_;
};

} // namespace SEED
