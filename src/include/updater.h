#ifndef UPDATER_H
#define UPDATER_H

#include <QJsonObject>
#include <QNetworkAccessManager>
#include <QFile>

class CountdownUpdater : public QObject
{
    Q_OBJECT

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

#endif // UPDATER_H
