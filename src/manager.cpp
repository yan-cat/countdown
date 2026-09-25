#include <QJsonObject>
#include <QDir>
#include <QStandardPaths>
#include <QSettings>
#include <QCoreApplication>
#include <QJSEngine>
#include <QQmlEngine>
#include <QSaveFile>
#include "manager.hpp"
#include "countdowndata.hpp"
#include "reminder.hpp"
#include "debug.hpp"

// 初始化函数与统一实例
CountdownManager *CountdownManager::create(QQmlEngine *, QJSEngine *) {
    CountdownManager *m = &manager();                      // 复用同一个实例
    QJSEngine::setObjectOwnership(m, QJSEngine::CppOwnership);  // 别让引擎删它
    return m;
}
CountdownManager &manager() {
    static CountdownManager instance(nullptr);
    return instance;
}
CountdownManager::CountdownManager(QObject *parent) : QObject(parent) {
    m_filePath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) // 设定倒数日json
                 + "/countdowns.json";
    QDir().mkpath(QFileInfo(m_filePath).absolutePath());
    loadCountdowns(); // 首次加载数据

    push_reminder(); // 检查提醒

    // 启动定时提醒
    m_reminderTimer.setInterval(60 * 1000); // 1 分钟
    connect(&m_reminderTimer, &QTimer::timeout, this, &CountdownManager::push_reminder);
    m_reminderTimer.start();
    qCDebug(CountdownLog) << "启动提醒检查";
}

// 保存倒数日
void CountdownManager::saveCountdowns() {
    QSaveFile saveFile(m_filePath);
    if (!saveFile.open(QIODevice::WriteOnly)) { // 只读报错
        qCritical() << "无法写入：" << m_filePath;
        return;
    }
    QJsonObject rootobj{ // 重新存入版本信息
        {"version", APP_VERSION},
        {"data", m_countdowns},
        {"lastModified", QDateTime::currentDateTime().toString(Qt::ISODate)}
    };
    QJsonDocument root(rootobj);
    saveFile.write(root.toJson(QJsonDocument::Indented));
    if (!saveFile.commit()) qCritical() << "提交文件失败：" << saveFile.errorString();
}

// 加载倒数日
void CountdownManager::loadCountdowns() {
    qCDebug(CountdownLog) << "开始加载数据文件";
    QFile file(m_filePath);

    if (!file.open(QIODevice::ReadOnly)) { // 找不到数据文件
        qWarning() << "打开失败，首次可忽略：" << m_filePath;
        return;
    }

    QJsonObject root = QJsonDocument::fromJson(file.readAll()).object(); // 读取全文成json

    QString fileVersion = root.value("version").toString();
    qCDebug(CountdownLog) << "文件版本" << fileVersion;
    qCDebug(CountdownLog) << "软件版本" << APP_VERSION;
    if (fileVersion != APP_VERSION) { // 版本不对更新版本
        file.close();
        updateOlddata();
        loadCountdowns();
        return;
    }
    else qCDebug(CountdownLog) << "无需升级数据";

    // 是json写入m_countdowns
    if (root.contains("data") && root.value("data").isArray()) {
        m_countdowns = root.value("data").toArray();
    }
    else {
        qCritical() << "数据结构损坏：" << m_filePath;
    }
}

// 旧版本数据转移
void CountdownManager::updateOlddata() {
    qCDebug(CountdownLog) << "旧数据转移启动";

    QFile file(m_filePath);
    QSaveFile saveFile(m_filePath);

    if (!file.open(QIODevice::ReadOnly)) {
        qCritical() << "找不到数据文件" << m_filePath;
        qApp->quit();
        return;
    }

    QJsonObject root = QJsonDocument::fromJson(file.readAll()).object(); // 读取全文成json

    file.close();

    //是不是新的数据结构
    if (!root.contains("data")) {
        qCDebug(CountdownLog) << "数据结构为旧结构";

        if (!file.open(QIODevice::ReadOnly)) {
            qCritical() << "找不到数据文件" << m_filePath;
            qApp->quit();
            return;
        }
        QJsonArray data = QJsonDocument::fromJson(file.readAll()).array();
        file.close();

        if (!saveFile.open(QIODevice::WriteOnly)) {
            qCritical() << "数据文件不可写" << m_filePath;
            qApp->quit();
            return;
        }

        QJsonObject rootobj{
            {"data", data}
        };
        QJsonDocument newroot(rootobj);
        saveFile.write(newroot.toJson(QJsonDocument::Indented));

        if (!saveFile.commit()) {
            qCritical() << "提交文件失败：" << saveFile.errorString();
            return;
        }

        qCDebug(CountdownLog) << "已转移成新结构";
    }
    else {
        qCDebug(CountdownLog) << "数据结构为新结构";

        // 读数据
        if (!file.open(QIODevice::ReadOnly)) {
            qCritical() << "找不到数据文件" << m_filePath;
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
        if (!saveFile.open(QIODevice::WriteOnly)) {
            qCritical() << "数据文件不可写" << m_filePath;
            qApp->quit();
            return;
        }
        QJsonObject rootobj{
            {"version", APP_VERSION},
            {"data", data}
        };
        QJsonDocument root(rootobj);
        saveFile.write(root.toJson(QJsonDocument::Indented));

        if (!saveFile.commit()) {
            qCritical() << "提交文件失败：" << saveFile.errorString();
            return;
        }

        qCDebug(CountdownLog) << "已添加缺失项";
    }
}

// 数据丢qml
QVariantList CountdownManager::countdowns() const {
    return CountdownData::buildCountdownViewData(m_countdowns);
}

// 添加或编辑倒数日
void CountdownManager::editCountdown(const QString &dateString) {
    qCDebug(CountdownLog) << "收到数据：" << dateString;

    QJsonObject obj = QJsonDocument::fromJson(dateString.toUtf8()).object();
    obj.insert("lastModified", QDateTime::currentDateTime().toString(Qt::ISODate));

    int id = obj.value("id").toInteger();

    if (id >= 0) { //编辑
        for (int i = 0; i < m_countdowns.size(); ++i) {
            if (m_countdowns.at(i).toObject().value("id").toInteger() == id) {
                m_countdowns.removeAt(i);
                m_countdowns.insert(i, obj);
                saveCountdowns();
                emit refreshCountdowns();
                qCDebug(CountdownLog) << "已编辑，id：" << id;
                return;
            }
        }
        qWarning() << "编辑失败，未知 id：" << id;
    }
    else { // 新建
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
        qCDebug(CountdownLog) << "新增，id：" << newId;
    }
}

// 删除倒数日
void CountdownManager::removeCountdown(int id) {
    for (int i = 0; i < m_countdowns.size(); ++i) {
        if (m_countdowns.at(i).toObject().value("id").toInteger() == id) {
            m_countdowns.removeAt(i);
            saveCountdowns();
            emit refreshCountdowns();
            qCDebug(CountdownLog) << "已删除，id：" << id;
            return;
        }
    }
    qCritical() << "删除失败，id：" << id;
}

// 读设置
int CountdownManager::setting(const QString &key, int def) const {
    QSettings s;
    // qCDebug(CountdownLog) << "查询设置：" << key;
    return s.value(key, def).toInt();
}

// 写设置
void CountdownManager::setSetting(const QString &key, int value) {
    QSettings s;
    if (s.value(key, "") != value) {
        s.setValue(key, value);
        qCDebug(CountdownLog) << "修改设置键：" << key << "值：" << value;
        emit refreshCountdowns();
    }
    else qCDebug(CountdownLog) << "设置值未变动，拒绝修改：" << key;
}

// 设置存在吗
bool CountdownManager::hasSetting(const QString &key) {
    QSettings s;
    return s.contains(key);
}

// 按id查
QJsonObject CountdownManager::getCountdownJson(int id, QString key) const {
    return CountdownData::getCountdownJson(m_countdowns, id, key);
}

// 确认满足发送条件发通知
void CountdownManager::run_reminder(int id) {
    QString data = getCountdownJson(id, "name").value("name").toString();
    qCDebug(CountdownLog) << "查询数据返回：" << data;

    QDate today = QDate::currentDate();
    QDate nextDue = CountdownData::getNextDue(getCountdownJson(id, "none"), today);
    qint64 days = today.daysTo(nextDue);
    qCDebug(CountdownLog) << "距今：" << days << "天";

    qint64 setdays = getCountdownJson(id, "notificationdays").value("notificationdays").toInteger();
    if (days > setdays) {
        qCDebug(CountdownLog) << "超过设定天数" << setdays << "天，驳回";
        return;
    }

    QString out;
    if (days == 0) out = tr("今天是");
    else out = tr("还剩 %1 天").arg(days);

    reminder().pushReminder(tr("倒数日提醒"), QString(out + data));
}

// 首次查需要提醒的日子
void CountdownManager::push_reminder() {
    QDate today = QDate::currentDate();
    QTime now = QTime::currentTime();

    QSettings s; // 不算设置所以放这里
    s.beginGroup("Data");
    qint64 diffDay = s.value("lastReminder", "1970-01-01").toDate().daysTo(today);
    qCDebug(CountdownLog) << "上一次提醒在" << diffDay << "天前";

    if (diffDay <= 0) {
        qCDebug(CountdownLog) << "今日已提醒";
        return;
    }

    qint64 reminderHour = setting("reminderHour", 6);
    qint64 reminderMinute = setting("reminderMinute", 30);
    QTime reminderTime(reminderHour, reminderMinute);

    if (now < reminderTime) {
        qCDebug(CountdownLog) << "提醒时间未到";
        return;
    }

    qCDebug(CountdownLog) << "开始提醒";
    qCDebug(CountdownLog) << "查询需提醒倒数日";
    for (const QJsonValue &v : std::as_const(m_countdowns)) {
        QJsonObject obj = v.toObject();
        if (getCountdownJson(obj.value("id").toInteger(), "notificationdays").value("notificationdays").toInteger() >= 0) {
            run_reminder(obj.value("id").toInteger());
        }
    }
    s.setValue("lastReminder", today.toString(Qt::ISODate));
}
