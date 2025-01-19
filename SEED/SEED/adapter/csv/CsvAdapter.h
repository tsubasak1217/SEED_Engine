#pragma once
#include <string>
#include <vector>

class CsvAdapter{
public:// singleton
    static CsvAdapter* GetInstance();

    CsvAdapter(const CsvAdapter&) = delete;
    CsvAdapter& operator=(const CsvAdapter&) = delete;

public: // function
    void SaveCsv(const std::string& fileName,
                 const std::vector<std::vector<std::string>>& data);

    std::vector<std::vector<std::string>> LoadCsv(const std::string& fileName);

private: // constructor / destructor
    CsvAdapter() = default;
    ~CsvAdapter() = default;

private:
    std::string directoryPath_ = "resources/csv/";
};
