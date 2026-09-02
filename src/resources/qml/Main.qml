import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import org.kde.kirigami as Kirigami
import org.kde.kirigamiaddons.dateandtime 1.0 as KA

Kirigami.ApplicationWindow {
    id: root
    width: 800
    height: 600
    visible: true
    title: qsTr("倒数日")

    // 空项目引导
    Label {
        visible: manager.countdowns.length === 0 && root.pageStack.depth === 1
        text: qsTr("还没有倒数日\n右键空白处新建倒数日")
        anchors.centerIn: parent
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
        enabled: false
        z: 1
    }

    // 左上角菜单
    globalDrawer: Kirigami.GlobalDrawer {
        title: qsTr("菜单")
        isMenu: true
        actions: [
            Kirigami.Action {
                text: qsTr("设置")
                icon.name: "settings-configure"
                onTriggered: settingsWindow.show()
            },
            Kirigami.Action {
                text: qsTr("检查更新")
                icon.name: "update-none"
                onTriggered: updaterWindow.show()
            },
            Kirigami.Action {
                text: qsTr("关于")
                icon.name: "help-about"
                onTriggered: aboutPageWindow.show()
            }
        ]
    }
//==========================================================================菜单
    // 新建&编辑倒数日
    Kirigami.Dialog {
        id: adddate
        title: editingId >= 0 ? qsTr("编辑倒数日") : qsTr("新建倒数日")
        modal: true
        height: root.height
        standardButtons: Kirigami.Dialog.Ok | Kirigami.Dialog.Cancel

        property int editingId: -1
        property var editingData: ({})

        ColumnLayout {
            anchors.fill: parent
            spacing: 10

            Label {
                text: qsTr("名称：")
                Layout.alignment: Qt.AlignCenter
            }
            TextField {
                id: nameField
                placeholderText: qsTr("例如：生日、纪念日")
                Layout.fillWidth: true
                Layout.alignment: Qt.AlignCenter
            }

            Label {
                text: qsTr("重复：")
                Layout.alignment: Qt.AlignCenter
            }
            ComboBox {
                id: repeatField
                currentIndex: 0
                model: [qsTr("无"), qsTr("月重复"), qsTr("年重复")]
                Layout.alignment: Qt.AlignCenter
            }

            CheckBox {
                id: notificationField
                checked: false
                text: qsTr("到设定日期时提醒")
                Layout.alignment: Qt.AlignCenter
            }
            ComboBox {
                id: notificationtypeField
                currentIndex: 0
                model: [qsTr("当天提醒"), qsTr("前一天提醒"), qsTr("自定义")]
                visible: notificationField.checked
                Layout.alignment: Qt.AlignCenter
            }
            RowLayout {
                spacing: 10
                visible: notificationtypeField.currentIndex === 2 && notificationField.checked
                Layout.alignment: Qt.AlignCenter

                TextField {
                    id: notificationdaysField
                    placeholderText: "7"
                    Layout.preferredWidth: 100
                    inputMethodHints: Qt.ImhDigitsOnly

                        validator: RegularExpressionValidator {
                            regularExpression: /^\d+$/
                        }
                }
                Label {
                    id: rightText
                    text: qsTr("天")
                    Layout.preferredWidth: 15
                }
            }

            Label {
                text: qsTr("请选择目标日期：")
                Layout.alignment: Qt.AlignCenter
            }
            KA.DatePicker {
                id: dateField
                Layout.alignment: Qt.AlignCenter
            }
        }
        onOpened: {
            if (editingId >= 0) {
                nameField.text = editingData.name
                repeatField.currentIndex = editingData.repeatIndex
                dateField.selectedDate = new Date(editingData.date)
            }
            else
            {
                nameField.text = ""
                repeatField.currentIndex = 0
                notificationField.checked = false
                notificationtypeField.currentIndex = 0
                notificationdaysField.text = ""
                dateField.selectedDate = new Date()
            }
        }
        onAccepted: {
            var date = dateField.selectedDate
            var dateStr = date.getFullYear() + "-" +
                          String(date.getMonth() + 1).padStart(2, '0') + "-" +
                          String(date.getDate()).padStart(2, '0')

            var name = nameField.text;
            if (name === "") name = qsTr("未命名");

            var days = 0
            if (!notificationField.checked) days = -1
            else if (notificationtypeField.currentIndex === 0) days = 0
            else if (notificationtypeField.currentIndex === 1) days = 1
            else if (notificationtypeField.currentIndex === 2 && notificationdaysField.text !== "") days = parseInt(notificationdaysField.text, 10)
            else
            {
                days = -1
                inlineMessage.text = qsTr("提醒天数为空，已禁用提醒")
                inlineMessage.type = Kirigami.MessageType.Warning
                inlineMessage.visible = true
            }

            var payload = {
                id: editingId, // id
                name: name, // 名字
                repeat: repeatField.currentIndex, // 重复
                notificationdays: days, // 提醒
                date: dateStr, // 日期
            }
            manager.editCountdown(JSON.stringify(payload))

            inlineMessage.text = qsTr("保存成功")
            inlineMessage.type = Kirigami.MessageType.Positive
            inlineMessage.visible = true
        }
    }

    // 卡片主界面
    pageStack.initialPage: Kirigami.ScrollablePage {
        title: qsTr("倒数日")
        MouseArea {
                anchors.fill: parent
                z: 0
                acceptedButtons: Qt.RightButton
                onClicked: (mouse) => {
                    if (mouse.button === Qt.RightButton) {
                        blankMenu.popup(mouse.x, mouse.y)
                    }
                }
                onPressAndHold: (mouse) => {
                    blankMenu.popup(mouse.x, mouse.y)
                }
            }

            Menu {
                id: blankMenu
                MenuItem {
                    text: qsTr("新建倒数日")
                    icon.name: "document-new"
                    onTriggered: {
                        adddate.editingId = -1
                        adddate.open()
                    }
                }
            }
        ColumnLayout {
            Kirigami.CardsLayout {
                Repeater {
                    model: manager.countdowns
                    delegate: Kirigami.AbstractCard {
                        Layout.preferredWidth: Kirigami.Units.gridUnit * 20
                        //菜单
                        Menu {
                            id: cardMenu

                            property var inlineMessage1: inlineMessage

                            MenuItem {
                                text: qsTr("编辑")
                                icon.name: "document-edit"
                                onTriggered: {
                                    adddate.editingId = modelData.id
                                    adddate.editingData = {
                                        name: modelData.name,
                                        repeatIndex: modelData.repeatIndex,
                                        date: modelData.date
                                    }

                                    var days = modelData.notificationdays

                                    notificationField.checked = false
                                    notificationtypeField.currentIndex = 0
                                    notificationdaysField.text = ""
                                    if (days === -1) {
                                        notificationField.checked = false
                                    } else {
                                        notificationField.checked = true
                                        if (days === 0) {
                                            notificationtypeField.currentIndex = 0
                                        } else if (days === 1) {
                                            notificationtypeField.currentIndex = 1
                                        } else {
                                            notificationtypeField.currentIndex = 2
                                            notificationdaysField.text = days.toString()
                                        }
                                    }

                                    adddate.open()
                                }
                            }
                            MenuItem {
                                text: qsTr("删除")
                                icon.name: "edit-delete"
                                onTriggered: {
                                    manager.removeCountdown(modelData.id)

                                    cardMenu.inlineMessage1.text = qsTr("删除成功")
                                    cardMenu.inlineMessage1.type = Kirigami.MessageType.Positive
                                    cardMenu.inlineMessage1.visible = true
                                }
                            }
                        }
                        // 主卡片
                        contentItem: ColumnLayout {
                            anchors.margins: Kirigami.Units.largeSpacing

                            Label {
                                text: modelData.name
                                font.bold: true
                                Layout.fillWidth: true
                                wrapMode: Text.Wrap
                                maximumLineCount: 2
                                elide: Text.ElideRight
                            }
                            Label { text: modelData.repeat }
                            Label { text: modelData.date }
                            Label { text: modelData.notificationdaystext }
                            Label { text: modelData.daysText }
                        }
                        // 鼠标右键
                        MouseArea {
                            anchors.fill: parent
                            acceptedButtons: Qt.LeftButton | Qt.RightButton
                            onClicked: (mouse) => {
                                if (mouse.button === Qt.RightButton) {
                                    cardMenu.popup(mouse.x, mouse.y)
                                }
                            }
                            onPressAndHold: (mouse) => {
                                cardMenu.popup(mouse.x, mouse.y)
                            }
                        }
                    }
                }
            }
        }
    }


    SettingsWindow {
        id: settingsWindow // 类型名按文件名，这里是隐藏的窗口实例
    }
    UpdaterWindow {
        id: updaterWindow // 类型名按文件名，这里是隐藏的窗口实例
    }
    AboutPageWindow {
        id: aboutPageWindow
    }

    //提醒土司
    Kirigami.InlineMessage {
        id: inlineMessage
        anchors.top: parent.top
        anchors.left: parent.left
        anchors.right: parent.right
        visible: false
        showCloseButton: true
        onVisibleChanged: {
            if (visible) {
                hideMessageTimer.restart()
            }
        }
    }

    // 两秒自动关闭
    Timer {
        id: hideMessageTimer
        interval: 2000
        onTriggered: {
            inlineMessage.visible = false
        }
    }

}