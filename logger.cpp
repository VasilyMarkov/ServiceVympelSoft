#include "logger.h"
#include <sstream>
#include <filesystem>
#include <fstream>
#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonArray>

namespace fs = std::filesystem;

Logger &Logger::getInstance()
{
    static Logger instance;
    return instance;
}

void Logger::createLog()
{
    auto logsDirPath = fs::current_path().parent_path() / "logs";

    if(!fs::exists(logsDirPath)) {
        fs::create_directory(logsDirPath);
    }

    auto now = std::chrono::system_clock::now();
    std::time_t now_time = std::chrono::system_clock::to_time_t(now);
    std::tm *now_tm = std::localtime(&now_time);

    std::stringstream sstream;
    sstream << std::put_time(now_tm, "%Y-%m-%d_%H:%M:%S.json");
    auto logPath = logsDirPath / sstream.str();
    logfile_ = std::make_unique<std::ofstream>(logPath);
}

void Logger::log(const std::vector<double>& data)
{
    if(!logfile_->is_open()) {
        throw std::runtime_error("Log file doesn't exist");
    }

    if(data.empty()) {
        throw std::runtime_error("Error log");
    }

    QJsonArray jsonArray;

    for (auto&& el : data) {
        QJsonObject jsonObject;
        jsonObject["brightness"] = el;
        jsonArray.append(jsonObject);
    }
    QJsonDocument jsonDoc(jsonArray);

    QByteArray jsonByteArray = jsonDoc.toJson();

    logfile_->write(jsonByteArray.data(), jsonByteArray.size());
}

void Logger::closeLog()
{

}

Logger::Logger()
{

}

Logger::~Logger()
{

}

Logger::Logger(const Logger &)
{

}

Logger &Logger::operator=(const Logger &)
{

}
