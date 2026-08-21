import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import org.kde.kirigami 2.19 as Kirigami
import org.kde.kirigamiaddons.dateandtime 1.0 as KA

Kirigami.ApplicationWindow {
    id: root
    width: 800
    height: 600
    visible: true
    title: "倒数日"

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
            }
        ]
    }
//==========================================================================菜单
    Kirigami.Dialog {
        id: adddata
        title: "选择倒数日"
        modal: true
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
            }

            Label {
                text: "重复："
                Layout.alignment: Qt.AlignCenter
            }
            ComboBox {
                id: repeatField
                currentIndex: 0
                model: ["无", "月重复", "年重复"]   // 三个选项
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
        onAccepted: {
            var date = dateField.selectedDate
            var dateStr = date.getFullYear() + "-" +
                          String(date.getMonth() + 1).padStart(2, '0') + "-" +
                          String(date.getDate()).padStart(2, '0')

            var payload = {
                name: nameField.text,
                repeat: repeatField.currentText,
                date: dateStr
            }
            manager.addCountdown(JSON.stringify(payload))
        }
    }
//==========================================================================页面
    pageStack.initialPage: Kirigami.Page {
        title: "倒数日"
        Label {
            anchors.centerIn: parent
            text: "滚木"
        }
    }

}