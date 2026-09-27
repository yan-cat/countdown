#include <QCoreApplication>
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
#include <QVersionNumber>
#include <QTextDocument>
#include "updater.hpp"
#include "debug.hpp"
#include "manager.hpp"
#include "main.hpp"

#ifdef Q_OS_WIN
#include <windows.h>
#include <shellapi.h>
#elif defined(Q_OS_ANDROID)
#include <QJniObject>
#endif


QUrl downloadUrl;
QUrl fastUrl("https://v4.gh-proxy.org/");

CountdownUpdater::CountdownUpdater(QObject *parent) : QObject(parent) { }

CountdownUpdater &updater()
{
    static CountdownUpdater instance(nullptr);
    return instance;
}
CountdownUpdater *CountdownUpdater::create(QQmlEngine *, QJSEngine *)
{
    CountdownUpdater *m = &updater();                          // 复用同一个实例
    QJSEngine::setObjectOwnership(m, QJSEngine::CppOwnership);  // 别让引擎删它
    return m;
}

// 获取更新
void CountdownUpdater::getReleaseInfo()
{
    if (reply && reply->isRunning()) {
        qWarning() << "已有检查在进行中，忽略本次请求";
        return;
    }
    qCDebug(CountdownLog) << "开始检查更新";
    // 构造请求
    QUrl url("https://api.github.com/repos/yan-cat/countdown/releases/latest");
    QNetworkRequest request(url);
    request.setRawHeader("Accept", "application/vnd.github+json");
    request.setRawHeader("X-GitHub-Api-Version", "2026-03-10");

    // 发送请求
    QNetworkReply *reply = m_updater.get(request);

    // 处理请求
    connect(reply, &QNetworkReply::finished, this, [this, reply]() {
        // 读返回
        QByteArray data = reply->readAll();
        QJsonDocument jsonDoc = QJsonDocument::fromJson(data);
        QJsonObject jsonObj = jsonDoc.object();

        // 网络错误
        if (reply->error() != QNetworkReply::NoError) {
            qWarning() << "网络请求错误:" << reply->errorString();
            emit newVersionError(reply->errorString());
            reply->close();
            reply->deleteLater();
            return;
        }

        // 要的信息
        QString latestVersion = jsonObj["tag_name"].toString(); // 最新版本
        QString latestVersionLog = jsonObj["body"].toString(); // 更新日志

        // md转html
        QTextDocument doc;
        doc.setMarkdown(latestVersionLog);
        latestVersionLog = doc.toHtml();

        QUrl fsUrl;
        if (manager().setting("fastDownload", 0)) fsUrl = fastUrl;
        downloadUrl = fsUrl.toString() + "https://github.com/yan-cat/countdown/releases/download/" + latestVersion;
        qCDebug(CountdownLog) << "获取到的下载地址：" << downloadUrl;

        QString currentVersion = "v" APP_VERSION;

        qCDebug(CountdownLog) << "当前版本:" << currentVersion;
        qCDebug(CountdownLog) << "最新版本:" << latestVersion;

        // 返回信号
        auto stripV = [](QString v) {
            return v.startsWith(QLatin1Char('v')) ? v.mid(1) : v;
        };
        if (QVersionNumber::fromString(stripV(latestVersion)) <  QVersionNumber::fromString(stripV(currentVersion))) qCDebug(CountdownLog) << "当前为 Beta 版";
        bool haveNewVersion =  QVersionNumber::fromString(stripV(latestVersion)) >  QVersionNumber::fromString(stripV(currentVersion));
        if (debug().getDebugOn("forceDownloadLatest"))
        {
            haveNewVersion = true;
            qCDebug(CountdownLog) << "强制下载最新版本";
        }
        if (haveNewVersion) {
            qCDebug(CountdownLog) << "发现新版本" << latestVersion;
            emit newVersion(true, latestVersion, latestVersionLog);
        } else {
            qCDebug(CountdownLog) << "已是最新版本" << latestVersion;
            emit newVersion(false, latestVersion, latestVersionLog);
        }

        reply->close();
        reply->deleteLater();
    });
}

// 下载更新 这个函数我看不懂，出bug找AI
void CountdownUpdater::downloadNewVersion()
{
    qCDebug(CountdownLog) << "当前系统为：" << os;
    QString filename;

    if (os == "linux")
    {
        qCDebug(CountdownLog) << "准备下载linux版本";
        filename = "Countdown-linux-x86_64.tar.gz";
    }
    else if (os == "win")
    {
        qCDebug(CountdownLog) << "准备下载windows版本";
        filename = "Countdown-windows-x86_64.exe";
    }
    else if (os == "android")
    {
        qCDebug(CountdownLog) << "准备下载android版本";
        filename = "Countdown-android-arm64-v8a.apk";
    }
    else {
        qCDebug(CountdownLog) << "未知系统";
        return;
    }

    // 设置下载目录
    QString savePath;
    #ifdef Q_OS_ANDROID
    savePath = externalAppDataPath + "/download/" + filename;
    #else
    savePath = QStandardPaths::writableLocation(QStandardPaths::CacheLocation) + "/" + filename; // 下载路径
    #endif

    QDir().mkpath(QFileInfo(savePath).absolutePath());

    // 无法写入文件
    downloadFile.setFileName(savePath);
    if (!downloadFile.open(QIODevice::WriteOnly | QIODevice::Truncate))
    {
        qCritical() << "无法写入文件：" << savePath;
        emit downloadError(tr("无法写入文件：%1").arg(savePath));
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
    qCDebug(CountdownLog) << "下载完成：" << path;
    if (os == "linux")
    {
        qCDebug(CountdownLog) << "进入linux安装流程";

        QString extractDir = QStandardPaths::writableLocation(QStandardPaths::TempLocation)
                             + "/countdown-update";
        QDir().mkpath(extractDir);

        KTar archive(path);
        if (!archive.open(QIODevice::ReadOnly)) {
            qCritical() << "无法打开更新包:" << path;
            emit downloadError(tr("无法打开更新包：%1").arg(path));
            return;
        }
        archive.directory()->copyTo(extractDir, true);
        archive.close();

        QString exePath = QFileInfo(QStringLiteral("/proc/self/exe")).canonicalFilePath();
        qCDebug(CountdownLog) << "当前程序路径:" << exePath;

        // 3. 确认解压出了新程序
        QString newExe = extractDir + "/Countdown";
        if (!QFile::exists(newExe)) {
            qCritical() << "更新包内容不完整";
            emit downloadError(tr("更新包内容不完整"));
            return;
        }

        QFile::remove(exePath);
        if (!QFile::rename(newExe, exePath)) {
            qCritical() << "替换可执行文件失败";
            emit downloadError(tr("替换可执行文件失败"));
            return;
        }
        else emit installSuccess();

    }
    else if (os == "win")
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
            qCritical() << "启动安装包失败，ShellExecute 返回:" << (intptr_t)ret
                       << "路径:" << nativePath;
            emit downloadError(tr("启动安装包失败（错误码 %1）").arg((intptr_t)ret));
            return;
        }
        #else
        // 我没绷住
        qFatal() << "什么叫你在winnt内核下原生运行了其他平台的程序";
        qFatal() << "兄弟兄弟你的编译工具链好像爆了";
        #endif
        QCoreApplication::quit();
        return;
    }
    else if (os == "android") {
        #ifdef Q_OS_ANDROID
        QJniObject ctx = QNativeInterface::QAndroidApplication::context();
        if (!ctx.isValid()) {
            qCritical() << "无法获取 Android context";
            emit downloadError(tr("无法获取 Android 上下文，更新功能失效"));
            return;
        }

        QJniObject jniPath = QJniObject::fromString(path);
        QJniObject::callStaticMethod<void>(
            "com/countdown/Installer",
            "installApk",
            "(Landroid/content/Context;Ljava/lang/String;)V",
            ctx.object<jobject>(),
            jniPath.object<jstring>()
            );
        qCDebug(CountdownLog) << "已调用系统安装器：" << path;
        #endif
    }
    else qWarning() << "未知系统";
}
