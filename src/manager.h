#ifndef MANAGER_H
#define MANAGER_H

#include <QObject>
#include <QString>
#include <qjsonarray.h>
#include <QVariant>

class CountdownManager : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QVariantList countdowns READ countdowns NOTIFY countdownsChanged)

public:
    explicit CountdownManager(QObject *parent = nullptr);
    QVariantList countdowns() const;

signals:
    void countdownsChanged();

public slots:
    void addCountdown(const QString &dateString);
    void removeCountdown(int id);

private:
    void saveCountdowns();
    void loadCountdowns();

    QJsonArray m_countdowns;
    QString m_filePath;
};
#endif