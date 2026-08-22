import QtQuick 2.15
import org.kde.kirigami as Kirigami

Kirigami.AboutPage {
    objectName: "aboutPage"                 // 用来防止重复推入

    actions: [
        Kirigami.Action {
            text: "关闭"
            icon.name: "window-close"
            onTriggered: applicationWindow().pageStack.pop()   // 页栈里弹自己
        }
    ]

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
