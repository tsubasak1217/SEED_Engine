#pragma warning(push)
#pragma warning(disable: 4201) // curl ヘッダー内の無名 struct/union
#include <curl/curl.h>
#pragma warning(pop)

#include <SEED/Source/Manager/HttpManager/HttpManager.h>
#include <chrono>

namespace SEED {

static size_t CurlWriteCallback(char* ptr, size_t size, size_t nmemb, void* userdata) {       
    auto* output = static_cast<std::string*>(userdata);
    output->append(ptr, size * nmemb);
    return size * nmemb;
}

HttpManager* HttpManager::GetInstance() {
    if(!instance_) {
        instance_ = new HttpManager();
    }
    return instance_;
}

void HttpManager::Initialize() {
    curl_global_init(CURL_GLOBAL_DEFAULT);
    GetInstance();
}

void HttpManager::Release() {
    delete instance_;
    instance_ = nullptr;
    curl_global_cleanup();
}

void HttpManager::Update() {
    auto* inst = GetInstance();
    std::vector<std::pair<Callback, std::pair<bool, std::string>>> completed;
    {
        std::lock_guard<std::mutex> lock(inst->mutex_);
        for(auto it = inst->pendingRequests_.begin(); it != inst->pendingRequests_.end();) {  
            if(it->future.wait_for(std::chrono::seconds(0)) == std::future_status::ready) {   
                completed.emplace_back(std::move(it->callback), it->future.get());
                it = inst->pendingRequests_.erase(it);
            } else {
                ++it;
            }
        }
    }

    for(auto& [cb, result] : completed) {
        cb(result.first, result.second);
    }
}

std::string HttpManager::RequestSync(
    const std::string& method, const std::string& url,
    const std::string& body, bool* outSuccess,
    const std::string& token
) {
    std::string response;
    bool success = false;

    CURL* curl = curl_easy_init();
    if(curl) {
        curl_slist* headers = nullptr;

        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, CurlWriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
        curl_easy_setopt(curl, CURLOPT_TIMEOUT, 10L);
        curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);

        headers = curl_slist_append(headers, "Content-Type: application/json");
        if(!token.empty()){
            std::string authHeader = "Authorization: Bearer " + token;
            headers = curl_slist_append(headers, authHeader.c_str());
        }
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

        if(!body.empty()) {
            curl_easy_setopt(curl, CURLOPT_POSTFIELDS, body.c_str());
            curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, static_cast<long>(body.size()));    
        }

        if(method != "GET") {
            curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, method.c_str());
        }

        CURLcode res = curl_easy_perform(curl);
        success = (res == CURLE_OK);

        if(headers) { curl_slist_free_all(headers); }
        curl_easy_cleanup(curl);
    }

    if(outSuccess) { *outSuccess = success; }
    return response;
}

void HttpManager::RequestAsync(
    const std::string& method, const std::string& url,
    const std::string& body, Callback callback,
    const std::string& token
) {
    auto* inst = GetInstance();
    auto future = std::async(
        std::launch::async,
        [method, url, body, token]() -> std::pair<bool, std::string> {
            bool success = false;
            std::string resp = HttpManager::RequestSync(method, url, body, &success, token);  
            return { success, std::move(resp) };
        }
    );

    std::lock_guard<std::mutex> lock(inst->mutex_);
    inst->pendingRequests_.push_back({ std::move(future), std::move(callback) });
}

void HttpManager::GetAsync(const std::string& url, Callback callback, const std::string& token) {
    RequestAsync("GET", url, "", std::move(callback), token);
}

std::string HttpManager::GetSync(const std::string& url, bool* outSuccess, const std::string& token) {
    return RequestSync("GET", url, "", outSuccess, token);
}

} // namespace SEED
