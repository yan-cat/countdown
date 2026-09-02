#ifndef UPDATER_H
#define UPDATER_H

#include <QJsonObject>
#include <QNetworkAccessManager>

class CountdownUpdater : public QObject
{
    Q_OBJECT

public:
    explicit CountdownUpdater(QObject *parent = nullptr);
    Q_INVOKABLE void getReleaseInfo();

private:
    QNetworkAccessManager updater;

signals:
    bool newVersion(bool latestVersion,QString version ,QString updateLog);
};

#endif // UPDATER_H
