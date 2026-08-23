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
    title: "倒数日"

    Label { //空项目引导
        visible: manager.countdowns.length === 0
        text: "还没有倒数日\n右键空白处新建倒数日"
        anchors.centerIn: parent
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
        enabled: false
        z: 1
    }

    globalDrawer: Kirigami.GlobalDrawer {
        title: "菜单"
        isMenu: true
        actions: [
            Kirigami.Action {
                text: "新建倒数日"
                icon.name: "document-new"
                onTriggered: {
                    adddata.open()
                }
            },
            Kirigami.Action {
                text: "设置"
                icon.name: "settings-configure"
                onTriggered: settingsWindow.show()
            },
            Kirigami.Action {
                text: "关于"
                icon.name: "help-about"
                onTriggered: {
                    if (root.pageStack.currentItem?.objectName !== "aboutPage") {
                        root.pageStack.push(aboutPageComponent)   // 推入整页
                    }
                }
            }
        ]
    }
//==========================================================================菜单
    Kirigami.Dialog {
        id: adddata
        title: "新建倒数日"
        modal: true
        height: root.height
        standardButtons: Kirigami.Dialog.Ok | Kirigami.Dialog.Cancel

        ColumnLayout {
            anchors.fill: parent
            spacing: 10

            Label {
                text: "名称："
                Layout.alignment: Qt.AlignCenter
            }
            TextField {
                id: nameField
                placeholderText: "例如：生日、纪念日"
                Layout.fillWidth: true
                Layout.alignment: Qt.AlignCenter
            }

            Label {
                text: "重复："
                Layout.alignment: Qt.AlignCenter
            }
            ComboBox {
                id: repeatField
                currentIndex: 0
                model: ["无", "月重复", "年重复"]   // 三个选项
                Layout.alignment: Qt.AlignCenter
            }

            Label {
                text: "请选择目标日期："
                Layout.alignment: Qt.AlignCenter
            }
            KA.DatePicker {
                id: dateField
                Layout.alignment: Qt.AlignCenter
            }
        }
        onOpened: {
            nameField.text = ""
            repeatField.currentIndex = 0
            dateField.selectedDate = new Date()
        }
        onAccepted: {
            var date = dateField.selectedDate
            var dateStr = date.getFullYear() + "-" +
                          String(date.getMonth() + 1).padStart(2, '0') + "-" +
                          String(date.getDate()).padStart(2, '0')

            var payload = {
                name: nameField.text,
                repeat: repeatField.currentIndex,
                date: dateStr
            }
            manager.addCountdown(JSON.stringify(payload))
        }
    }
//==========================================================================卡片主界面
    pageStack.initialPage: Kirigami.ScrollablePage {
        title: "倒数日"
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
                    text: "新建倒数日"
                    icon.name: "document-new"
                    onTriggered: adddata.open()
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
                            // MenuItem {
                            //     text: "编辑"
                            //     icon.name: "document-edit"
                            //     onTriggered: console.log("点了编辑，id =", modelData.id)
                            // }
                            MenuItem {
                                text: "删除"
                                icon.name: "edit-delete"
                                onTriggered: manager.removeCountdown(modelData.id)
                            }
                        }
                        //主卡片
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
                            Label { text: modelData.daysText }
                        }
                        //鼠标右键
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
        id: settingsWindow        // 类型名按文件名，这里是隐藏的窗口实例
    }
    Component {
        id: aboutPageComponent
        AboutPage {}
    }
}