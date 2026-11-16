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

    if (!QFileInfo::exists(filePath)) {
        qWarning() << "选择的文件不存在";
        emit debug().showErrorDialog(tr("选择的文件不存在"));
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
    else qWarning() << "无旧数据，无需备份";

    if (QFile::copy(filePath, manager().getCountdownsPath())) {
        qCDebug(CountdownLog) << "导入成功";
        emit debug().showErrorDialog(tr("导入成功，旧数据已备份至 %1").arg(backupPath));
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
    }

    // 重载
    manager().loadCountdowns();
    emit manager().refreshCountdowns();
}
