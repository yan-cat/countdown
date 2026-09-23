import com.countdown
import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import QtQuick.Window 2.15
import org.kde.kirigami as Kirigami

Window {
    id: settingsWindow
    title: qsTr("设置")
    width: 640
    height: 480
    visible: false
    color: Kirigami.Theme.backgroundColor

    ScrollView {
        anchors.fill: parent
        ScrollBar.vertical.policy: ScrollBar.AlwaysOn
        contentWidth: availableWidth

        ColumnLayout {
            id: layout
            width: parent.width

//=====================================显示

            Label {
                text: qsTr("显示")
                Layout.alignment: Qt.AlignHCenter
                font.pointSize: Kirigami.Theme.defaultFont.pointSize * 1.5
            }

            // 日期显示格式
            Label {
                text: qsTr("日期显示格式：")
                Layout.alignment: Qt.AlignHCenter
            }
            ComboBox {
                id: dayShow
                currentIndex: CountdownManager.setting("dayShow", 0)
                onActivated: CountdownManager.setSetting("dayShow", currentIndex)
                model: [qsTr("按天显示"), qsTr("按年月日显示")]
                Layout.alignment: Qt.AlignHCenter
            }
            Label {
                text: dayShow.currentIndex === 0
                    ? qsTr("预览：还有 456 天 / 已经过了 456 天")
                    : qsTr("预览：还有 1 年 3 个月 1 天 / 已经过了 1 年 3 个月 1 天")

                Layout.alignment: Qt.AlignHCenter
            }

            // 描边
            CheckBox {
                id: upcoming
                text: qsTr("临近日期卡片红色描边")
                checked: CountdownManager.setting("upcoming", false)
                onClicked: {
                    CountdownManager.setSetting("upcoming", checked)
                }
                Layout.alignment: Qt.AlignHCenter
            }
            RowLayout {
                spacing: 10
                visible: upcoming.checked
                Layout.alignment: Qt.AlignCenter


                Label { text: qsTr("距离") }
                TextField {
                    text: CountdownManager.setting("upcomingDays", 3)
                    placeholderText: "3"
                    Layout.preferredWidth: 100
                    inputMethodHints: Qt.ImhDigitsOnly

                    maximumLength: 4

                    validator: RegularExpressionValidator {
                        regularExpression: /^\d+$/
                    }

                    onTextChanged: {
                        CountdownManager.setSetting("upcomingDays", text)
                    }
                }
                Label { text: qsTr("天标红") }
            }

            Item {
                Layout.preferredHeight: 20
                Layout.fillWidth: true
            }

//=====================================提醒设置

            Label {
                text: qsTr("提醒")
                Layout.alignment: Qt.AlignHCenter
                font.pointSize: Kirigami.Theme.defaultFont.pointSize * 1.5

                visible: Qt.platform.os !== "android"
            }

            Label {
                text: qsTr("倒数日提醒时间：")
                Layout.alignment: Qt.AlignHCenter

                visible: Qt.platform.os !== "android"
            }

            property int reminderHour: CountdownManager.setting("reminderHour", 06)
            property int reminderMinute: CountdownManager.setting("reminderMinute", 30)

            Button {
                icon.name: "clock"
                text: String(layout.reminderHour).padStart(2, '0') + ":" +
                      String(layout.reminderMinute).padStart(2, '0')
                Layout.alignment: Qt.AlignHCenter

                onClicked: timeDialog.open()

                visible: Qt.platform.os !== "android"
            }
            // 弹出时间选择框
            Kirigami.Dialog {
                id: timeDialog
                parent: settingsWindow.contentItem
                title: qsTr("选择提醒时间")
                preferredWidth: Kirigami.Units.gridUnit * 18
                standardButtons: Kirigami.Dialog.Ok | Kirigami.Dialog.Cancel

                // 临时变量，确认后才写回
                property int tempHour: layout.reminderHour
                property int tempMinute: layout.reminderMinute

                onOpened: {
                    // 改按钮文字
                    let okButton = standardButton(Kirigami.Dialog.Ok)
                    if (okButton) okButton.text = qsTr("保存")

                    let cancelButton = standardButton(Kirigami.Dialog.Cancel)
                    if (cancelButton) cancelButton.text = qsTr("放弃")

                    tempHour = layout.reminderHour
                    tempMinute = layout.reminderMinute
                    hourTumbler.currentIndex = tempHour
                    minuteTumbler.currentIndex = tempMinute
                }

                onAccepted: {
                    layout.reminderHour = tempHour
                    layout.reminderMinute = tempMinute
                    CountdownManager.setSetting("reminderHour", tempHour)
                    CountdownManager.setSetting("reminderMinute", tempMinute)
                }

                contentItem: Item {
                    RowLayout {
                        spacing: Kirigami.Units.largeSpacing
                        Layout.alignment: Qt.AlignHCenter
                        anchors.centerIn: parent

                        Tumbler {
                            id: hourTumbler
                            model: 24
                            visibleItemCount: 3
                            Layout.preferredWidth: Kirigami.Units.gridUnit * 3

                            delegate: Text {
                                text: String(modelData).padStart(2, '0')
                                color: Kirigami.Theme.textColor
                                font.pointSize: Kirigami.Theme.defaultFont.pointSize * 1.5
                                horizontalAlignment: Text.AlignHCenter
                                verticalAlignment: Text.AlignVCenter
                                opacity: 1.0 - Math.abs(Tumbler.displacement) / (Tumbler.tumbler.visibleItemCount / 2)
                            }
                            onCurrentIndexChanged: timeDialog.tempHour = currentIndex
                        }
                        Label {
                            text: ":"
                            font.pointSize: Kirigami.Theme.defaultFont.pointSize * 1.5
                            color: Kirigami.Theme.textColor
                            Layout.alignment: Qt.AlignVCenter
                        }
                        Tumbler {
                            id: minuteTumbler
                            model: 60
                            visibleItemCount: 3
                            Layout.preferredWidth: Kirigami.Units.gridUnit * 3

                            delegate: Text {
                                text: String(modelData).padStart(2, '0')
                                color: Kirigami.Theme.textColor
                                font.pointSize: Kirigami.Theme.defaultFont.pointSize * 1.5
                                horizontalAlignment: Text.AlignHCenter
                                verticalAlignment: Text.AlignVCenter
                                opacity: 1.0 - Math.abs(Tumbler.displacement) / (Tumbler.tumbler.visibleItemCount / 2)
                            }
                            onCurrentIndexChanged: timeDialog.tempMinute = currentIndex
                        }
                    }
                }
            }

            Label {
                text: qsTr("（在每天的几点提醒当天的倒数日）")
                Layout.alignment: Qt.AlignHCenter

                visible: Qt.platform.os !== "android"
            }

            Button {
                text: qsTr("测试通知")
                Layout.alignment: Qt.AlignHCenter

                onClicked: CountdownReminder.pushReminder(qsTr("倒数日提醒"), qsTr("测试通知"))

                visible: Qt.platform.os !== "android"
            }

            Item {
                Layout.preferredHeight: 20
                Layout.fillWidth: true

                visible: Qt.platform.os !== "android"
            }

//=====================================更新设置

            Label {
                text: qsTr("更新")
                Layout.alignment: Qt.AlignHCenter
                font.pointSize: Kirigami.Theme.defaultFont.pointSize * 1.5
            }

            CheckBox {
                id: autoGetNewVersion
                text: qsTr("有新版本时弹出更新窗口")
                checked: CountdownManager.setting("autoGetNewVersion", false)
                onClicked: {
                    CountdownManager.setSetting("autoGetNewVersion", checked)
                }
                Layout.alignment: Qt.AlignHCenter
            }
            CheckBox {
                id: fastDownload
                text: qsTr("使用加速源快速下载")
                checked: CountdownManager.setting("fastDownload", false)
                onClicked: {
                    CountdownManager.setSetting("fastDownload", checked)
                }
                Layout.alignment: Qt.AlignHCenter
            }

            Item {
                Layout.preferredHeight: 20
                Layout.fillWidth: true
            }

//=====================================软件设置

            Label {
                text: qsTr("其他")
                Layout.alignment: Qt.AlignHCenter
                font.pointSize: Kirigami.Theme.defaultFont.pointSize * 1.5
            }

            // 开机自启
            CheckBox {
                id: autoStart
                text: qsTr("开机自启")
                checked: CountdownAutoStart.getAutoStart()
                onClicked: {
                    CountdownAutoStart.setAutoStart(checked)
                }
                Layout.alignment: Qt.AlignHCenter
            }

//=====================================调试

            // 调试
            CheckBox {
                id: debugMode
                text: qsTr("调试模式")
                checked: CountdownManager.setting("debugMode", false)
                onClicked: {
                    CountdownManager.setSetting("debugMode", checked)
                }
                Layout.alignment: Qt.AlignHCenter
            }
            Label {
                text: qsTr("调试选项需要重启才能生效！")
                visible: debugMode.checked
                Layout.alignment: Qt.AlignHCenter
            }

            Item {
                Layout.preferredHeight: 20
                Layout.fillWidth: true
            }

            Label {
                text: qsTr("调试选项")
                visible: debugMode.checked
                Layout.alignment: Qt.AlignHCenter
                font.pointSize: Kirigami.Theme.defaultFont.pointSize * 1.8
            }
//=====================================日志

            Label {
                text: qsTr("日志")
                visible: debugMode.checked
                Layout.alignment: Qt.AlignHCenter
                font.pointSize: Kirigami.Theme.defaultFont.pointSize * 1.5
            }

            ComboBox {
                id: outputDebuglog
                visible: debugMode.checked
                currentIndex: CountdownManager.setting("outputDebuglog", 0)
                model: [qsTr("关闭调试日志"), qsTr("仅APP调试日志"), qsTr("调试日志全开（此设置会拖慢软件速度！！！）")]
                onActivated: CountdownManager.setSetting("outputDebuglog", currentIndex)
                Layout.alignment: Qt.AlignHCenter
            }
            CheckBox {
                id: showLogSource
                text: qsTr("显示日志来源")
                visible: debugMode.checked
                checked: CountdownManager.setting("showLogSource", false)
                onClicked: {
                    CountdownManager.setSetting("showLogSource", checked)
                }
                Layout.alignment: Qt.AlignHCenter
            }
            CheckBox {
                id: showStartupDuration
                text: qsTr("显示启动耗时")
                visible: debugMode.checked
                checked: CountdownManager.setting("showStartupDuration", false)
                onClicked: {
                    CountdownManager.setSetting("showStartupDuration", checked)
                }
                Layout.alignment: Qt.AlignHCenter
            }
            CheckBox {
                id: disableQmlWarn
                text: qsTr("禁用 QML 引擎警告")
                visible: debugMode.checked
                checked: CountdownManager.setting("disableQmlWarn", false)
                onClicked: {
                    CountdownManager.setSetting("disableQmlWarn", checked)
                }
                Layout.alignment: Qt.AlignHCenter
            }
            CheckBox {
                id: outputLogFile
                text: qsTr("启用日志文件")
                visible: debugMode.checked
                checked: CountdownManager.setting("outputLogFile", false)
                onClicked: {
                    CountdownManager.setSetting("outputLogFile", checked)
                }
                Layout.alignment: Qt.AlignHCenter
            }
            Button {
                text: qsTr("查看软件日志")
                visible: debugMode.checked && outputLogFile.checked
                onClicked: {
                    if (!logsLoader.active) {
                        logsLoader.active = true
                    }
                    logsLoader.item.show()
                }

                Layout.alignment: Qt.AlignHCenter
            }

            Item {
                Layout.preferredHeight: 20
                Layout.fillWidth: true
            }

//=====================================其他

            Label {
                text: qsTr("其他")
                visible: debugMode.checked
                Layout.alignment: Qt.AlignHCenter
                font.pointSize: Kirigami.Theme.defaultFont.pointSize * 1.5
            }

            CheckBox {
                id: useWindowsReminderType
                text: qsTr("强制使用windows通知窗口而非当前系统默认")
                visible: debugMode.checked
                checked: CountdownManager.setting("useWindowsReminderType", false)
                onClicked: {
                    CountdownManager.setSetting("useWindowsReminderType", checked)
                }
                Layout.alignment: Qt.AlignHCenter
            }

            CheckBox {
                id: useEnLang
                text: qsTr("强制软件语言为英语")
                visible: debugMode.checked
                checked: CountdownManager.setting("useEnLang", false)
                onClicked: {
                    CountdownManager.setSetting("useEnLang", checked)
                }
                Layout.alignment: Qt.AlignHCenter
            }

            CheckBox {
                id: forceDownloadLatest
                text: qsTr("强制下载最新版本即使当前版本为最新")
                visible: debugMode.checked
                checked: CountdownManager.setting("forceDownloadLatest", false)
                onClicked: {
                    CountdownManager.setSetting("forceDownloadLatest", checked)
                }
                Layout.alignment: Qt.AlignHCenter
            }

            CheckBox {
                id: showDebugDetails
                text: qsTr("在倒数日详情显示调试信息")
                visible: debugMode.checked
                checked: CountdownManager.setting("showDebugDetails", false)
                onClicked: {
                    CountdownManager.setSetting("showDebugDetails", checked)
                }
                Layout.alignment: Qt.AlignHCenter
            }

            // 占满不让分配
            Item {
                Layout.fillHeight: true
                Layout.fillWidth: true
            }
        }
    }

    Loader {
        id: logsLoader
        active: false
        sourceComponent: Component { LogsWindow { } }
    }
}
