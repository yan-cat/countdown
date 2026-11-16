#include <QFileDialog>
#include <QStandardPaths>
#include "replication.hpp"
#include "debug.hpp"
#include "manager.hpp"

// 初始化函数与统一实例
CountdownReplication *CountdownReplication::create(QQmlEngine *, QJSEngine *) {
    CountdownReplication *m = &replication();                      // 复用同一个实例
    QJSEngine::setObjectOwnership(m, QJSEngine::CppOwnership);  // 别让引擎删它
    return m;
}
CountdownReplication &replication() {
    static CountdownReplication instance(nullptr);
    return instance;
}
CountdownReplication::CountdownReplication(QObject *parent) : QObject(parent) { }

// 导出倒数日数据
void CountdownReplication::exportCountdownData() {
    QString defaultDir = QStandardPaths::writableLocation(QStandardPaths::DownloadLocation);

    QString filePath = QFileDialog::getSaveFileName(
        nullptr,
        tr("选择保存位置"),
        defaultDir + "/countdown_backup.json",
        tr("JSON 文件 (*.json);;所有文件 (*)")
        );

    if (filePath.isEmpty()) {
        qCDebug(CountdownLog) << "用户取消导出";
        return;
    }

    if (QFile::exists(filePath)) {
        QFile::remove(filePath);
    }

    if (QFile::copy(manager().getCountdownsPath(), filePath)) {
        qCDebug(CountdownLog) << "文件复制成功";
    } else {
        qWarning() << "文件复制失败";
        emit debug().showErrorDialog(tr("文件复制失败"));
    }
}

// 导入倒数日数据
void CountdownReplication::importCountdownData() {
    QString defaultDir = QStandardPaths::writableLocation(QStandardPaths::DownloadLocation);

    QString filePath = QFileDialog::getOpenFileName(
        nullptr,
        tr("选择要导入的备份文件"),
        defaultDir,
        tr("JSON 文件 (*.json);;所有文件 (*)")
        );

    if (filePath.isEmpty()) {
        qCDebug(CountdownLog) << "用户取消导入";
        return;
    }

    if (QFile f(filePath); f.open(QIODevice::ReadOnly)) {
        QJsonObject obj = QJsonDocument::fromJson(f.readAll()).object();

        if (obj.value("app") != "yan-cat/countdown") {
            qWarning() << "文件格式不正确";
            emit debug().showErrorDialog(tr("文件格式不正确，请检查文件来源"));
            return;
        }
    }
    else {
        qWarning() << "文件打开失败";
        emit debug().showErrorDialog(tr("文件打开失败"));
        return;
    }

    // 找到空着的序号
    qint64 reNameId = 0;
    while (QFile::exists(manager().getCountdownsPath() + ".bak" + QString::number(reNameId))) {
        reNameId++;
    }

    // 备份
    QString backupPath = manager().getCountdownsPath() + ".bak" + QString::number(reNameId);
    if (QFile::exists(manager().getCountdownsPath())) {
        if (QFile::rename(manager().getCountdownsPath(), backupPath)) {
            qCDebug(CountdownLog) << "旧数据已成功备份为：" << backupPath;
        } else {
            qWarning() << "备份旧数据失败";
            emit debug().showErrorDialog(tr("备份旧数据失败"));
            return;
        }
    }
    else qCDebug(CountdownLog) << "无旧数据，无需备份";

    if (QFile::copy(filePath, manager().getCountdownsPath())) {
        qCDebug(CountdownLog) << "导入成功";
    } else {
        qWarning() << "复制数据失败";
        if (QFile::rename(backupPath, manager().getCountdownsPath())) {
            emit debug().showErrorDialog(tr("复制数据失败") + " " + tr("回滚成功"));
            qWarning() << "回滚成功";
        }
        else {
            emit debug().showErrorDialog(tr("复制数据失败") + " " + tr("回滚失败"));
            qWarning() << "回滚失败";
        }
        return;
    }

    // 重载
    if (manager().reloadCountdownData()) {
        qCDebug(CountdownLog) << "重载成功";
        emit debug().showErrorDialog(tr("导入成功，旧数据已备份至 %1").arg(backupPath));
    }
    else {
        qWarning() << "重载失败";
        QFile::remove(manager().getCountdownsPath());
        if (QFile::copy(backupPath, manager().getCountdownsPath())) {
            qCDebug(CountdownLog) << "恢复旧文件成功";
            emit debug().showErrorDialog(tr("导入失败，已恢复旧数据，请检查文件来源"));
        }
        else {
            qWarning() << "恢复旧文件失败";
            emit debug().showErrorDialog(tr("导入失败，恢复旧数据失败，请检查您的硬盘与权限"));
        }
    }
}
