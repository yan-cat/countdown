#ifndef UPDATER_H
#define UPDATER_H

#include <QJsonObject>
#include <QNetworkAccessManager>
#include <QFile>
#include <QtQml/qqmlregistration.h>

class CountdownUpdater : public QObject
{
    Q_OBJECT
    QML_ELEMENT          // 注册进 QML 模块
    QML_SINGLETON        // 声明为 QML 单例

public:
    explicit CountdownUpdater(QObject *parent = nullptr);
    Q_INVOKABLE void getReleaseInfo();
    Q_INVOKABLE void downloadNewVersion();

private:
    QNetworkAccessManager updater;
    QNetworkAccessManager networkManager;
    QFile downloadFile;
    QNetworkReply *reply = nullptr;

signals:
    void newVersion(bool latestVersion,QString version ,QString updateLog);
    void downloadProgress(qint64 bytesReceived, qint64 bytesTotal);
    void downloadFinished();
    void downloadError(const QString &errorString);
    void installSuccess();

private:
    void installNewVersion(QString path);
};
CountdownUpdater &updater();
#endif // UPDATER_H
