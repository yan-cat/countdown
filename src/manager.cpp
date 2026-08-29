#include <QJsonObject>
#include <QDir>
#include <QStandardPaths>
#include <QSettings>
#include <QCoreApplication>
#include "manager.h"
#include "countdowndata.h"
#include "reminder.h"
#include "debug.h"

// 初始化
CountdownManager::CountdownManager(QObject *parent) : QObject(parent)
{
    m_filePath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) // 设定倒数日json
                 + "/countdowns.json";
    QDir().mkpath(QFileInfo(m_filePath).absolutePath());
    loadCountdowns(); // 首次加载数据

    push_reminder(); // 检查提醒
}

// 保存倒数日
void CountdownManager::saveCountdowns()
{
    QFile file(m_filePath);
    if (!file.open(QIODevice::WriteOnly)) { // 只读报错
        qWarning() << "无法写入：" << m_filePath;
        return;
    }
    QJsonObject rootobj{
        {"version", APP_VERSION},
        {"data", m_countdowns}
    };
    QJsonDocument root(rootobj);
    file.write(root.toJson(QJsonDocument::Indented));
}

// 加载倒数日
void CountdownManager::loadCountdowns()
{
    qCDebug(CountdownLog) << "[ Debug ]" << "开始加载数据文件";
    QFile file(m_filePath);

    if (!file.open(QIODevice::ReadOnly)) { // 找不到数据文件
        qWarning() << "打开失败，首次可忽略：" << m_filePath;
        return;
    }

    QJsonObject root = QJsonDocument::fromJson(file.readAll()).object(); // 读取全文成json

    QString fileVersion = root.value("version").toString();
    qCDebug(CountdownLog) << "[ Debug ]" << "文件版本" << fileVersion;
    qCDebug(CountdownLog) << "[ Debug ]" << "软件版本" << APP_VERSION;
    if (fileVersion != APP_VERSION)
    {
        file.close();
        updateOlddata();
        loadCountdowns();
        return;
    }
    else qCDebug(CountdownLog) << "[ Debug ]" << "无需升级数据";

    // 是json写入m_countdowns
    if (root.contains("data") && root.value("data").isArray()) {
        m_countdowns = root.value("data").toArray();
    }
    else
    {
        qWarning() << "数据结构损坏：" << m_filePath;
    }
}

// 旧版本数据转移
void CountdownManager::updateOlddata()
{
    qCDebug(CountdownLog) << "[ Debug ]" << "旧数据转移启动";

    QFile file(m_filePath);
    if (!file.open(QIODevice::ReadOnly)) {
        qCDebug(CountdownLog) << "找不到数据文件" << m_filePath;
        qApp->quit();
        return;
    }

    QJsonObject root = QJsonDocument::fromJson(file.readAll()).object(); // 读取全文成json

    file.close();

    //是不是新的数据结构
    if (!root.contains("data"))
    {
        qCDebug(CountdownLog) << "[ Debug ]" << "数据结构为旧结构";

        if (!file.open(QIODevice::ReadOnly)) {
            qCDebug(CountdownLog) << "找不到数据文件" << m_filePath;
            qApp->quit();
            return;
        }
        QJsonArray data = QJsonDocument::fromJson(file.readAll()).array();
        file.close();

        if (!file.open(QIODevice::WriteOnly)) {
            qCDebug(CountdownLog) << "数据文件不可写" << m_filePath;
            qApp->quit();
            return;
        }

        QJsonObject rootobj{
            {"data", data}
        };
        QJsonDocument newroot(rootobj);
        file.write(newroot.toJson(QJsonDocument::Indented));

        qCDebug(CountdownLog) << "[ Debug ]" << "已转移成新结构";
    }
    else
    {
        qCDebug(CountdownLog) << "[ Debug ]" << "数据结构为新结构";

        // 读数据
        if (!file.open(QIODevice::ReadOnly)) {
            qCDebug(CountdownLog) << "找不到数据文件" << m_filePath;
            qApp->quit();
            return;
        }
        QJsonArray data = QJsonDocument::fromJson(file.readAll()).object().value("data").toArray();
        file.close();

        // 处理缺失项
        for (int i = 0; i < data.size(); ++i) {
            QJsonObject obj = data[i].toObject();

            if (!obj.contains("notificationdays")) obj["notificationdays"] = -1;
            if (!obj.contains("repeat")) obj["repeat"] = 0;

            data[i] = obj;
        }

        // 放好版本存回去
        if (!file.open(QIODevice::WriteOnly)) {
            qCDebug(CountdownLog) << "数据文件不可写" << m_filePath;
            qApp->quit();
            return;
        }
        QJsonObject rootobj{
            {"version", APP_VERSION},
            {"data", data}
        };
        QJsonDocument root(rootobj);
        file.write(root.toJson(QJsonDocument::Indented));

        qCDebug(CountdownLog) << "[ Debug ]" << "已添加缺失项";
    }
}

// 数据丢qml
QVariantList CountdownManager::countdowns() const
{
    return buildCountdownViewData(m_countdowns);
}

// 添加或编辑倒数日
void CountdownManager::editCountdown(const QString &dateString)
{
    qCDebug(CountdownLog) << "[ Debug ]" << "收到数据：" << dateString;

    QJsonObject obj = QJsonDocument::fromJson(dateString.toUtf8()).object();
    int id = obj.value("id").toInteger();

    if (id >= 0) //编辑
    {
        for (int i = 0; i < m_countdowns.size(); ++i) {
            if (m_countdowns.at(i).toObject().value("id").toInteger() == id) {
                m_countdowns.removeAt(i);
                m_countdowns.insert(i, obj);
                saveCountdowns();
                emit refreshCountdowns();
                qCDebug(CountdownLog) << "[ Debug ]" << "已编辑，id：" << id;
                return;
            }
        }
        qWarning() << "编辑失败，找不到 id：" << id;
    }
    else // 新建
    {
        int newId = 0;
        while (true) {
            bool found = false;
            for (const QJsonValue &v : std::as_const(m_countdowns)) {
                if (v.toObject().value("id").toInteger() == newId) { found = true; break; }
            }
            if (!found) break;
            newId++;
        }
        obj.insert("id", newId);
        m_countdowns.append(obj);
        saveCountdowns();
        emit refreshCountdowns();
        qCDebug(CountdownLog) << "[ Debug ]" << "新增，id：" << newId;
    }
}

// 删除倒数日
void CountdownManager::removeCountdown(int id)
{
    for (int i = 0; i < m_countdowns.size(); ++i) {
        if (m_countdowns.at(i).toObject().value("id").toInteger() == id) {
            m_countdowns.removeAt(i);
            saveCountdowns();
            emit refreshCountdowns();
            qCDebug(CountdownLog) << "[ Debug ]" << "已删除，id：" << id;
            return;
        }
    }
    qWarning() << "删除失败，id：" << id;
}

// 读设置
int CountdownManager::setting(const QString &key, int def) const
{
    QSettings s;
    qCDebug(CountdownLog) << "[ Debug ]" << "查询设置：" << key;
    return s.value(key, def).toInt();
}

// 写设置
void CountdownManager::setSetting(const QString &key, int value)
{
    QSettings s;
    s.setValue(key, value);
    emit refreshCountdowns();
    qCDebug(CountdownLog) << "[ Debug ]" << "修改设置键：" << key << "值：" << value;
}

// 按id查
QJsonObject CountdownManager::getCountdownJson(int id, QString key) const
{
    return ::getCountdownJson(m_countdowns, id, key);
}

// 确认满足发送条件发通知
void CountdownManager::run_reminder(int id)
{
    QString data = getCountdownJson(id, "name").value("name").toString();
    qCDebug(CountdownLog) << "[ Debug ]" << "查询数据返回：" << data;

    QDate today = QDate::currentDate();
    QDate nextDue = getNextDue(getCountdownJson(id, "none"), today);
    qint64 days = today.daysTo(nextDue);
    qCDebug(CountdownLog) << "[ Debug ]" << "距今：" << days << "天";

    qint64 setdays = getCountdownJson(id, "notificationdays").value("notificationdays").toInteger();
    if (days > setdays)
    {
        qCDebug(CountdownLog) << "[ Debug ]" << "超过设定天数" << setdays << "天，驳回";
        return;
    }

    QString out;
    if (days == 0) out = "今天";
    else out = QString("还剩 %1 天").arg(days);

    reminder(QString(out + data));
}

// 首次查需要提醒的日子
void CountdownManager::push_reminder()
{
    QSettings s;
    qCDebug(CountdownLog) << "[ Debug ]" << "查询需提醒倒数日";
    for (const QJsonValue &v : std::as_const(m_countdowns))
    {
        QJsonObject obj = v.toObject();
        if (getCountdownJson(obj.value("id").toInteger(), "notificationdays").value("notificationdays").toInteger() >= 0)
        {
            run_reminder(obj.value("id").toInteger());
        }
    }
}

// 获取debug设置
bool CountdownManager::getDebugOn(const QString &key)
{
    QSettings s;
    if (s.value("debugmode", false).toBool()){
        return s.value(key, false).toBool();
    }
    else return false;
}