#ifndef MANAGER_H
#define MANAGER_H

#include <QObject>
#include <QString>

class CountdownManager : public QObject
{
    Q_OBJECT
public:
    explicit CountdownManager(QObject *parent = nullptr);

public slots:
    // 供 QML 调用的函数，参数是日期字符串（格式 yyyy-MM-dd）
    void addCountdown(const QString &dateString);
};

#endif // MANAGER_H
