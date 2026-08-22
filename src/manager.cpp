#include <QJsonObject>
#include <QDir>
#include <QStandardPaths>
#include <QSettings>
#include "manager.h"
#include "countdowndata.h"

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
    emit refreshCountdowns(); //添加后刷新列表
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