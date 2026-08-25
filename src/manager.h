#ifndef MANAGER_H
#define MANAGER_H

#include <QJsonArray>

class CountdownManager : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QVariantList countdowns READ countdowns NOTIFY refreshCountdowns)

public:
    explicit CountdownManager(QObject *parent = nullptr);
    QVariantList countdowns() const;

    Q_INVOKABLE int setting(const QString &key, int def = 0) const;
    Q_INVOKABLE void setSetting(const QString &key, int value);

signals:
    void refreshCountdowns();

public slots:
    void editCountdown(const QString &dateString);
    void removeCountdown(int id);

private:
    void saveCountdowns();
    void loadCountdowns();

    QJsonArray m_countdowns;
    QString m_filePath;
};
#endif