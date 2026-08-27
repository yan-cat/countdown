#include <QJsonObject>
#include <QDir>
#include <QStandardPaths>
#include <QSettings>
#include "manager.h"
#include "countdowndata.h"
#include "reminder.h"

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
    return buildCountdownViewData(m_countdowns);
}

void CountdownManager::editCountdown(const QString &dateString) //添加或编辑倒数日
{
    QJsonObject obj = QJsonDocument::fromJson(dateString.toUtf8()).object();
    int id = obj.value("id").toInt();

    if (id >= 0) { //编辑
        for (int i = 0; i < m_countdowns.size(); ++i) {
            if (m_countdowns.at(i).toObject().value("id").toInt() == id) {
                m_countdowns.removeAt(i);
                m_countdowns.insert(i, obj);
                saveCountdowns();
                emit refreshCountdowns();
                qDebug() << "已编辑，id：" << id;
                return;
            }
        }
        qWarning() << "编辑失败，找不到 id：" << id;
    } else { //新建
        int newId = 0;
        while (true) {
            bool found = false;
            for (const QJsonValue &v : std::as_const(m_countdowns)) {
                if (v.toObject().value("id").toInt() == newId) { found = true; break; }
            }
            if (!found) break;
            newId++;
        }
        obj.insert("id", newId);
        m_countdowns.append(obj);
        saveCountdowns();
        emit refreshCountdowns();
        qDebug() << "新增，id：" << newId;
    }
}

void CountdownManager::removeCountdown(int id) //删除倒数日
{
    for (int i = 0; i < m_countdowns.size(); ++i) {
        if (m_countdowns.at(i).toObject().value("id").toInt() == id) {
            m_countdowns.removeAt(i);
            saveCountdowns();
            emit refreshCountdowns();
            qDebug() << "已删除，id：" << id;
            return;
        }
    }
    qWarning() << "删除失败，id：" << id;
}

int CountdownManager::setting(const QString &key, int def) const //读设置
{
    QSettings s;
    return s.value(key, def).toInt();
}

void CountdownManager::setSetting(const QString &key, int value) //写设置
{
    QSettings s;
    s.setValue(key, value);
    emit refreshCountdowns();
    qDebug() << "修改设置键：" << key << "值：" << value;
}

QJsonObject CountdownManager::getCountdownJson(int id, QString key) const //按id查
{
    return ::getCountdownJson(m_countdowns, id, key);
}

void CountdownManager::run_reminder(int id) //发通知
{
    QString data = getCountdownJson(id, "name").value("name").toString();
    qDebug() << "查询数据返回：" << data;

    QDate today = QDate::currentDate();
    QDate nextDue = getNextDue(getCountdownJson(id, "none"), today);
    qint64 days = today.daysTo(nextDue);
    qDebug() << "距今：" << days << "天";

    QString out;
    if (days == 0) out = "就是今天";
    else out = QString("还有 %1 天").arg(days);

    reminder(QString(data + out));
}