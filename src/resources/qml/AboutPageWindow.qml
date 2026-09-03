import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import org.kde.kirigami as Kirigami
import org.kde.kirigamiaddons.dateandtime 1.0 as KA

Window {
    id: aboutPageWindow
    title: qsTr("关于")
    width: 420
    height: 480
    visible: false
    color: Kirigami.Theme.backgroundColor

    Kirigami.AboutPage {
        anchors.fill: parent
        aboutData: {
            "productName": "countdown",
            "componentName": "countdown",
            "displayName": "倒数日",
            "shortDescription": "一个记录重要日子的倒计时小应用",
            "homepage": "https://github.com/yan-cat/countdown",
            "bugAddress": "https://github.com/yan-cat/countdown/issues",
            "version": Qt.application.version,
            "copyrightStatement": "© 2026 yancat",
            "authors": [
                { "name": "yancat", "task": "开发" },
                { "name": "DeepSeek", "task": "辅助开发" }
            ],
            "licenses": [
                { "name": "GPL v3", "spdx": "GPL-3.0", "text": "https://www.gnu.org/licenses/gpl-3.0" }
            ]
        }
    }
}