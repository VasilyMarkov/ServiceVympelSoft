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
#include <exception>

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
        QDir configDir = QDir::current();
//        configDir.cdUp(); // Move to the parent directory
        QString configFilePath = configDir.filePath(localPath_);

        QFile file(configFilePath);

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
#ifdef _WIN32
    QString localPath_ = "release/config.json";
#elif __linux__
    QString localPath_ = "config.json";
#endif
    QHash<QString, std::shared_ptr<QVariantHash>> config_store_;
};

#endif // CONFIGREADER_H
