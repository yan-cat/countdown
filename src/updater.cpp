#include <QCoreApplication>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QUrl>
#include <QJsonDocument>
#include <QJsonObject>
#include <QDebug>
#include "updater.h"
#include "debug.h"

CountdownUpdater::CountdownUpdater(QObject *parent) : QObject(parent) { }

void CountdownUpdater::getReleaseInfo()
{
    qCDebug(CountdownLog) << "[ Debug ]" << "开始检查更新";

    QUrl url("https://api.github.com/repos/yan-cat/countdown/releases/latest");
    QNetworkRequest request(url);
    request.setRawHeader("Accept", "application/vnd.github+json");
    request.setRawHeader("X-GitHub-Api-Version", "2026-03-10");

    QNetworkReply *reply = updater.get(request);

    connect(reply, &QNetworkReply::finished, this, [this, reply]() {
        if (reply->error() != QNetworkReply::NoError) {
            qWarning() << "网络请求错误:" << reply->errorString();
            reply->deleteLater();
            return;
        }

        QByteArray data = reply->readAll();
        QJsonDocument jsonDoc = QJsonDocument::fromJson(data);
        QJsonObject jsonObj = jsonDoc.object();

        // 要的信息
        QString latestVersion = jsonObj["tag_name"].toString(); // 最新版本
        QString latestVersionLog = jsonObj["body"].toString(); // 更新日志

        // QString currentVersion = "v1.1.0"; // 硬编码版本测试
        QString currentVersion = "v" APP_VERSION; // 当前软件版本

        qCDebug(CountdownLog) << "[ Debug ]" << "当前版本:" << currentVersion;
        qCDebug(CountdownLog) << "[ Debug ]" << "最新版本:" << latestVersion;

        if (latestVersion > currentVersion) {
            qCDebug(CountdownLog) << "[ Debug ]" << "发现新版本" << latestVersion;
            emit newVersion(true, latestVersion, latestVersionLog);
        } else {
            qCDebug(CountdownLog) << "[ Debug ]" << "已是最新版本" << latestVersion;
            emit newVersion(false, latestVersion, latestVersionLog);
        }

        reply->deleteLater();
    });
}