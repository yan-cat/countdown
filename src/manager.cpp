#include "manager.h"
#include <QDebug>
#include <QDateTime>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QFile>
#include <QFileInfo>
#include <QDir>
#include <QStandardPaths>

CountdownManager::CountdownManager(QObject *parent) : QObject(parent) //初始化
{
    m_filePath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) //设定倒数日json
                 + "/countdowns.json";
    QDir().mkpath(QFileInfo(m_filePath).absolutePath());
    loadCountdowns(); //首次加载数据
}

void CountdownManager::saveCountdowns() //保存倒数日
{
    QFile file(m_filePath);
    if (!file.open(QIODevice::WriteOnly)) { //只读报错
        qWarning() << "无法写入：" << m_filePath;
        return;
    }
    file.write(QJsonDocument(m_countdowns).toJson(QJsonDocument::Indented));
}
void CountdownManager::loadCountdowns() //加载倒数日
{
    qDebug() << "开始加载数据文件";
    QFile file(m_filePath);

    if (!file.open(QIODevice::ReadOnly)) { //找不到数据文件
        qWarning() << "打开失败，首次可忽略：" << m_filePath;
        return;
    }

    QJsonDocument doc = QJsonDocument::fromJson(file.readAll()); //读取全文成json

    if (doc.isArray()) { //是json写入m_countdowns
        m_countdowns = doc.array();
    }
    else
    {
        qWarning() << "数据结构损坏：" << m_filePath;
    }
}

QVariantList CountdownManager::countdowns() const //数据丢qml
{
    qDebug() << "查询数据";
    QVariantList list;
    for (const QJsonValue &v : m_countdowns) {
        QJsonObject obj = v.toObject(); //拿出来

        QString repeat = obj.value("repeat").toString(); //不重复文案
        if (repeat == "无") repeat = "不重复";
        obj.insert("repeat", repeat);

        QString date = obj.value("date").toString(); //日期

        QDateTime target = QDateTime::fromString(date, "yyyy-MM-dd"); //天数
        QDate today = QDate::currentDate();
        QDate nextDue;
        if (repeat == "年重复") { //重复吗
            int m = target.date().month();
            int d = target.date().day();
            nextDue = QDate(today.year(), m, d);
            if (nextDue < today) {
                nextDue = QDate(today.year() + 1, m, d);
            }
        } else if (repeat == "月重复") {
            int d = target.date().day();
            nextDue = QDate(today.year(), today.month(), d);
            if (nextDue < today) {
                nextDue = QDate(today.addMonths(1).year(),
                                today.addMonths(1).month(), d);
            }
        } else { //不重复或数据异常
            nextDue = target.date();
        }
        qint64 days = today.daysTo(nextDue);
        QString daysText; //文案
        if (days > 0) {
            daysText = QString("还有 %1 天").arg(days);
        } else if (days < 0) {
            daysText = QString("已经过了 %1 天").arg(-days);
        } else {
            daysText = QStringLiteral("今天");
        }
        obj.insert("daysText", daysText);

        list.append(obj.toVariantMap()); //丢回去
    }
    return list;
}

void CountdownManager::addCountdown(const QString &dateString) //添加倒数日
{
    QJsonObject obj = QJsonDocument::fromJson(dateString.toUtf8()).object();

    int id = 0; //找未占用id
    while (true) {
        bool found = false;
        for (const QJsonValue &v : std::as_const(m_countdowns)) {
            if (v.toObject().value("id").toInt() == id) {
                found = true;
                break;
            }
        }
        if (!found) break;
        id++;
    }
    qDebug() <<"新增倒数日，id："<< id;
    obj.insert("id", id);

    m_countdowns.append(obj);
    saveCountdowns();
    emit countdownsChanged(); //添加后刷新列表
}

void CountdownManager::removeCountdown(int id) //删除倒数日
{
    for (int i = 0; i < m_countdowns.size(); ++i) {
        if (m_countdowns.at(i).toObject().value("id").toInt() == id) {
            m_countdowns.removeAt(i);
            saveCountdowns();
            emit countdownsChanged();
            qDebug() << "已删除，id：" << id;
            return;
        }
    }
    qWarning() << "删除失败，id：" << id;
}