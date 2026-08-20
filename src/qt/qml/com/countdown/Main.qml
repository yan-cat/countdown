import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import org.kde.kirigami 2.19 as Kirigami
import org.kde.kirigamiaddons.dateandtime 1.0 as KA

Kirigami.ApplicationWindow {
    id: root
    width: 600
    height: 400
    visible: true
    title: "菜单测试"

    globalDrawer: Kirigami.GlobalDrawer {
        title: "菜单"
        isMenu: true
        actions: [
                Kirigami.Action {
                text: "新建倒数日"
                icon.name: "document-new"
                onTriggered: {
                    datePickerDialog.open()
                }
            }
        ]
    }

    pageStack.initialPage: Kirigami.Page {
        title: "倒数日"
        Label {
            anchors.centerIn: parent
            text: "滚木"
        }
    }

    Kirigami.Dialog {
        id: datePickerDialog
        title: "选择倒数日"
        modal: true
        standardButtons: Kirigami.Dialog.Ok | Kirigami.Dialog.Cancel

        ColumnLayout {
            anchors.fill: parent
            spacing: 10

            Label {
                text: "请选择目标日期："
                Layout.alignment: Qt.AlignCenter
            }

            KA.DatePicker {
                id: datePicker
                Layout.alignment: Qt.AlignCenter
                selectedDate: new Date()
            }
        }

        onAccepted: {
            var date = datePicker.selectedDate
            var dateStr = date.getFullYear() + "-" +
                          String(date.getMonth() + 1).padStart(2, '0') + "-" +
                          String(date.getDate()).padStart(2, '0')
            manager.addCountdown(dateStr)
        }
    }
}