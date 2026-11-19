import com.countdown
import QtQuick
import org.kde.kirigami as Kirigami

Window {
    id: aboutPageWindow
    title: qsTr("关于")
    width: 420
    height: 480
    visible: false
    color: Kirigami.Theme.backgroundColor

    Kirigami.AboutPage {
        anchors.fill: parent
        loadAvatars: true
        aboutData: {
            "productName": "Countdown",
            "componentName": "Countdown",
            "displayName": "倒数日",
            "desktopFileName": "com.countdown",
            "shortDescription": "一个记录重要日子的倒计时小应用",
            "homepage": "https://github.com/yan-cat/countdown",
            "bugAddress": "https://github.com/yan-cat/countdown/issues",
            "version": Qt.application.version,
            "copyrightStatement": "© 2026 yancat",
            "authors": [
                {
                    "name": "yancat",
                    "task": "开发",
                    "ocsUsername": "yan-cat",
                    "avatarUrl": "https://github.com/yan-cat.png"
                },
                { "name": "DeepSeek", "task": "辅助开发" }
            ],
            "licenses": [
                { "name": "GPL v3", "spdx": "GPL-3.0", "text": "https://www.gnu.org/licenses/gpl-3.0" }
            ]
        }
    }
}
