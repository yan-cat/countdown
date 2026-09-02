#include <QCoreApplication>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QUrl>
#include <QJsonDocument>
#include <QJsonObject>
#include <QDebug>
#include <QStandardPaths>
#include <QDir>
#include <QFileInfo>
#include <KTar>
#include <KArchive>
#include <KArchiveDirectory>
#include <QProcess>
#include <QDesktopServices>
#include "updater.h"
#include "debug.h"

#ifdef Q_OS_WIN
#include <windows.h>
#include <shellapi.h>
#endif

QUrl downloadUrl;
QString os;

CountdownUpdater::CountdownUpdater(QObject *parent) : QObject(parent) { }

// 获取更新
void CountdownUpdater::getReleaseInfo()
{
    qCDebug(CountdownLog) << "[ Debug ]" << "开始检查更新";

    QUrl url("https://api.github.com/repos/yan-cat/countdown/releases/latest");
    QNetworkRequest request(url);
    request.setRawHeader("Accept", "application/vnd.github+json");
    request.setRawHeader("X-GitHub-Api-Version", "2026-03-10");

    QNetworkReply *reply = updater.get(request);

    connect(reply, &QNetworkReply::finished, this, [this, reply]() {
        if (reply->error() != QNetworkReply::NoError) {
            qWarning() << "网络请求错误:" << reply->errorString();
            reply->deleteLater();
            return;
        }

        QByteArray data = reply->readAll();
        QJsonDocument jsonDoc = QJsonDocument::fromJson(data);
        QJsonObject jsonObj = jsonDoc.object();

        // 要的信息
        QString latestVersion = jsonObj["tag_name"].toString(); // 最新版本
        QString latestVersionLog = jsonObj["body"].toString(); // 更新日志

        downloadUrl = "https://github.com/yan-cat/countdown/releases/download/" + latestVersion;
        qCDebug(CountdownLog) << "[ Debug ]" << "获取到的下载地址：" << downloadUrl;

        QString currentVersion = "v" APP_VERSION;

        qCDebug(CountdownLog) << "[ Debug ]" << "当前版本:" << currentVersion;
        qCDebug(CountdownLog) << "[ Debug ]" << "最新版本:" << latestVersion;

        bool haveNewVersion = latestVersion > currentVersion;
        if (getDebugOn("forceDownloadLatest"))
        {
            haveNewVersion = true;
            qCDebug(CountdownLog) << "[ Debug ]" << "强制下载最新版本";
        }
        if (haveNewVersion) {
            qCDebug(CountdownLog) << "[ Debug ]" << "发现新版本" << latestVersion;
            emit newVersion(true, latestVersion, latestVersionLog);
        } else {
            qCDebug(CountdownLog) << "[ Debug ]" << "已是最新版本" << latestVersion;
            emit newVersion(false, latestVersion, latestVersionLog);
        }

        reply->deleteLater();
    });
}

// 下载更新 这个函数我看不懂，出bug找AI
void CountdownUpdater::downloadNewVersion()
{
    os = QSysInfo::kernelType();
    qCDebug(CountdownLog) << "[ Debug ]" << "当前系统为：" << os;
    QString filename;

    if (os == "linux")
    {
        qCDebug(CountdownLog) << "[ Debug ]" << "准备下载linux版本";
        filename = "Countdown-linux-x86_64.tar.gz";
    }
    else if (os == "winnt")
    {
        qCDebug(CountdownLog) << "[ Debug ]" << "准备下载windows版本";
        filename = "Countdown-windows-x86_64.exe";
    }
    else qCDebug(CountdownLog) << "[ Debug ]" << "未知系统";

    QString savePath = QStandardPaths::writableLocation(QStandardPaths::CacheLocation) + "/" + filename; // 下载路径
    QDir().mkpath(QFileInfo(savePath).absolutePath());

    // 无法写入文件
    downloadFile.setFileName(savePath);
    if (!downloadFile.open(QIODevice::WriteOnly | QIODevice::Truncate))
    {
        qCDebug(CountdownLog) << "[ Debug ]" << "无法写入文件：" << savePath;
        emit downloadError(tr("无法写入文件：").arg(savePath));
        return;
    }

    // 开始下载
    QUrl fileUrl(downloadUrl.toString() + "/" + filename);
    QNetworkRequest request(fileUrl);
    request.setHeader(QNetworkRequest::UserAgentHeader, "Countdown Updater/1.0");

    // 跟随 GitHub 的 302 跳转
    request.setAttribute(QNetworkRequest::RedirectPolicyAttribute,
                         QNetworkRequest::NoLessSafeRedirectPolicy);
    request.setRawHeader("User-Agent", "Countdown Updater/1.0");

    reply = networkManager.get(request);

    // 下载进度
    connect(reply, &QNetworkReply::downloadProgress,
            this, &CountdownUpdater::downloadProgress);

    // 下载完成
    connect(reply, &QNetworkReply::finished, this, [this, savePath]() {
        downloadFile.flush();
        downloadFile.close();
        if (reply->error() != QNetworkReply::NoError)
            emit downloadError(reply->errorString());
        else
        {
            emit downloadFinished();
            CountdownUpdater::installNewVersion(savePath);
        }
        reply->deleteLater();
        reply = nullptr;
        networkManager.clearConnectionCache();
    });

    // 下载错误
    connect(reply, &QNetworkReply::errorOccurred,
            this, [this](QNetworkReply::NetworkError code) {
                Q_UNUSED(code);
                QString errorString = reply->errorString();
                emit downloadError(errorString);
            });

    // 写入数据到文件
    connect(reply, &QNetworkReply::readyRead, this, [this]() {
        if (reply->error() != QNetworkReply::NoError)
            return;
        if (downloadFile.isOpen())
            downloadFile.write(reply->readAll());
    });
}

// 安装更新
void CountdownUpdater::installNewVersion(QString path)
{
    qCDebug(CountdownLog) << "[ Debug ]" << "下载完成：" << path;
    if (os == "linux")
    {
        qCDebug(CountdownLog) << "[ Debug ]" << "进入linux安装流程";

        QString extractDir = QStandardPaths::writableLocation(QStandardPaths::TempLocation)
                             + "/countdown-update";
        QDir().mkpath(extractDir);

        KTar archive(path);
        if (!archive.open(QIODevice::ReadOnly)) {
            qWarning() << "无法打开更新包:" << path;
            emit downloadError(tr("无法打开更新包：%1").arg(path));
            return;
        }
        archive.directory()->copyTo(extractDir, true);
        archive.close();

        QString exePath = QFileInfo(QStringLiteral("/proc/self/exe")).canonicalFilePath();
        qCDebug(CountdownLog) << "[ Debug ]" << "当前程序路径:" << exePath;

        // 3. 确认解压出了新程序
        QString newExe = extractDir + "/Countdown";
        if (!QFile::exists(newExe)) {
            emit downloadError(tr("更新包内容不完整"));
            return;
        }

        QFile::remove(exePath);
        if (!QFile::rename(newExe, exePath)) {
            emit downloadError(tr("替换可执行文件失败"));
            return;
        }
        else emit installSuccess();

    }
    else if (os == "winnt")
    {
#ifdef Q_OS_WIN
        // 先按"双击"语义打开（自动触发 UAC）；SE_ERR_ACCESSDENIED 表示被拒，再强制 runas
        QString nativePath = QDir::toNativeSeparators(path);
        HINSTANCE ret = ShellExecuteW(nullptr, L"open",
                                      (LPCWSTR)nativePath.utf16(),
                                      nullptr, nullptr, SW_SHOWNORMAL);
        if ((intptr_t)ret == SE_ERR_ACCESSDENIED) {
            ret = ShellExecuteW(nullptr, L"runas",
                                (LPCWSTR)nativePath.utf16(),
                                nullptr, nullptr, SW_SHOWNORMAL);
        }
        if ((intptr_t)ret <= 32) {
            qWarning() << "启动安装包失败，ShellExecute 返回:" << (intptr_t)ret
                       << "路径:" << nativePath;
            emit downloadError(tr("启动安装包失败（错误码 %1）").arg((intptr_t)ret));
            return;
        }
#else
        if (!QProcess::startDetached(path, {})) {
            qWarning() << "启动安装包失败:" << path;
            emit downloadError(tr("启动安装包失败：%1").arg(path));
            return;
        }
#endif
        QCoreApplication::quit();
        return;
    }
    else qCDebug(CountdownLog) << "[ Debug ]" << "未知系统";
}