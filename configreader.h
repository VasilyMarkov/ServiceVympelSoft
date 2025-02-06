#ifndef CONFIGREADER_H
#define CONFIGREADER_H


#include <QCoreApplication>
#include <QDir>
#include <QFile>
#include <QTextStream>
#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonArray>
#include <QDateTime>
#include <QHash>
#include <iostream>
#include <filesystem>
#include <exception>

namespace fs = std::filesystem;

class ConfigReader final
{
public:
    ConfigReader(const ConfigReader&) = delete;
    ConfigReader& operator=(const ConfigReader&) = delete;

    static ConfigReader& getInstance()
    {
        static ConfigReader instance;
        return instance;
    }

    QVariant get(QString globalKey, QString key) const {
        return (*config_store_[globalKey])[key];
    }
private:
    ConfigReader() {
        auto configFilePath = fs::current_path().parent_path().parent_path();
        configFilePath /= localPath_;

        QFile file(QString::fromStdString(configFilePath.string()));
        if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            throw std::runtime_error("Could not open config file.");
        }

        QByteArray jsonData = file.readAll();
        file.close();

        QJsonParseError parseError;
        QJsonDocument jsonDoc = QJsonDocument::fromJson(jsonData, &parseError);
        if (parseError.error != QJsonParseError::NoError) {
            throw std::runtime_error("JSON parse error.");
        }

        auto hash = jsonDoc.toVariant().toHash();

        for(auto it = hash.begin(); it != hash.end(); ++it) {
            config_store_.insert(it.key(), std::make_shared<QVariantHash>(it.value().toHash()));
        }
    }
    ~ConfigReader(){}

    const std::string localPath_ = "prod/conf/config.json";
    QHash<QString, std::shared_ptr<QVariantHash>> config_store_;
};

inline auto&& configReader = ConfigReader::getInstance();

#endif // CONFIGREADER_H
