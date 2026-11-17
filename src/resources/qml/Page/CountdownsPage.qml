import com.countdown
import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Dialogs
import org.kde.kirigami as Kirigami
import org.kde.kirigamiaddons.dateandtime as DateAndTime

Kirigami.ScrollablePage {
    id: countdownsPage
    title: qsTr("倒数日")

    // 新建/编辑菜单
    Kirigami.Dialog {
        id: adddate
        title: editingId >= 0 ? qsTr("编辑倒数日") : qsTr("新建倒数日")
        modal: true
        height: countdownsPage.height
        preferredWidth: Qt.platform.os === "android"
                        ? Math.min(countdownsPage.width, Kirigami.Units.gridUnit * 40)
                        : implicitWidth   // 桌面平台用默认宽度

        standardButtons: Kirigami.Dialog.Ok | Kirigami.Dialog.Cancel

        property int editingId: -1
        property var editingData: ({})

        ColumnLayout {
            width: adddate.availableWidth
            spacing: 10

            // 名字
            Label {
                text: qsTr("名称：")
                Layout.alignment: Qt.AlignCenter
            }
            TextField {
                id: nameField
                placeholderText: qsTr("例如：生日、纪念日")
                Layout.preferredWidth: Qt.platform.os === "android"
                                       ? Math.min(adddate.width, 350)
                                       : implicitWidth   // 桌面平台用默认宽度
                Layout.alignment: Qt.AlignCenter
                maximumLength: 20
            }

            // 重复
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

            // 到日期提醒
            CheckBox {
                id: notificationField
                checked: false
                text: qsTr("到设定日期时提醒")
                Layout.alignment: Qt.AlignCenter

                visible: Qt.platform.os !== "android"
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

                    maximumLength: 9

                        validator: RegularExpressionValidator {
                            regularExpression: /^\d+$/
                        }
                }
                Label {
                    text: qsTr("天")
                    Layout.preferredWidth: 15
                }
            }

            // 日期
            Label {
                text: qsTr("请选择目标日期：")
                Layout.alignment: Qt.AlignCenter
            }
            DateAndTime.DatePicker {
                id: dateField
                Layout.preferredWidth: Qt.platform.os === "android"
                                       ? Math.min(adddate.width, 350)
                                       : implicitWidth   // 桌面平台用默认宽度
                Layout.alignment: Qt.AlignCenter
            }
        }
        onOpened: {
            // 改按钮文字
            let okButton = standardButton(Kirigami.Dialog.Ok)
            if (okButton) okButton.text = qsTr("保存")

            let cancelButton = standardButton(Kirigami.Dialog.Cancel)
            if (cancelButton) cancelButton.text = qsTr("放弃")


            if (editingId >= 0) { // 编辑
                nameField.text = editingData.name
                repeatField.currentIndex = editingData.repeatIndex
                dateField.selectedDate = new Date(editingData.date)
            }
            else { // 新建
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
            else {
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
            CountdownManager.editCountdown(JSON.stringify(payload))

            inlineMessage.text = qsTr("保存成功")
            inlineMessage.type = Kirigami.MessageType.Positive
            inlineMessage.visible = true
        }
    }

    // 空项目引导
    Label {
        visible: cardsLayout.count === 0
        text: Qt.platform.os === "android"
            ? qsTr("还没有倒数日\n长按空白处新建倒数日")
            : qsTr("还没有倒数日\n右键空白处新建倒数日")
        anchors.centerIn: parent
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
        enabled: false
        z: 1
    }

    // 如果是测试版就改标题
    Connections {
        target: CountdownUpdater

        function onNewVersion(latestVersion, version , updateLog) { // latestVersion 为 true 是软件版本小于 latest
            // 如果现在版本不小于云的版本也不等于云的版本就改标题
            if (!latestVersion && version !== "v" + Qt.application.version) countdownsPage.title = qsTr("倒数日 Beta")
        }
    }

    // 提醒土司
    Kirigami.InlineMessage {
        id: inlineMessage
        anchors.top: parent.top
        anchors.left: parent.left
        anchors.right: parent.right
        visible: false
        showCloseButton: true
        z: 999

        Timer {
            interval: 2000
            running: inlineMessage.visible
            onTriggered: inlineMessage.visible = false
        }
    }

    // 确认删除
    Kirigami.PromptDialog {
        property var name
        property var cardid
        id: deleteCountdown
        title: qsTr("删除")
        subtitle: qsTr("确认删除 %1 吗？").arg(name)
        standardButtons: Kirigami.Dialog.Yes | Kirigami.Dialog.No
        onOpened: {
            // 改按钮文字
            let yesButton = standardButton(Kirigami.Dialog.Yes)
            if (yesButton) yesButton.text = qsTr("确定")

            let noButton = standardButton(Kirigami.Dialog.No)
            if (noButton) noButton.text = qsTr("取消")
        }
        onAccepted: {
            CountdownManager.removeCountdown(cardid)
            inlineMessage.text = qsTr("删除成功")
            inlineMessage.type = Kirigami.MessageType.Positive
            inlineMessage.visible = true
        }
        onRejected: {
        }
    }

    // 卡片的乱七八糟代码
    property bool isSortingMode: false
    actions: [
        // 排序按钮
        Kirigami.Action {
            id: sortAction
            icon.name: "view-sort-ascending"
            text: qsTr("调整顺序")

            checkable: true
            checked: countdownsPage.isSortingMode

            onTriggered: countdownsPage.isSortingMode = !countdownsPage.isSortingMode
        }
    ]

    MouseArea {
        anchors.fill: parent
        z: 0
        acceptedButtons: Qt.LeftButton | Qt.RightButton
        pressAndHoldInterval: 300 // 300ms触发长按
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
            visible: !countdownsPage.isSortingMode
            maximumColumns: 8
            Repeater {
                id: cardsLayout
                model: CountdownManager.countdowns
                delegate: Kirigami.AbstractCard {
                    id: cardAbs
                    Layout.preferredWidth: Kirigami.Units.gridUnit * 20

                    property int depressedId: -1
                    property bool depressed: modelData.id === depressedId

                    property real hoverScale: depressed ? 0.95 : 1.0
                    property real pulseScale: 1.0

                    scale: hoverScale * pulseScale

                    Behavior on hoverScale {
                        NumberAnimation {
                            duration: 180
                            easing.type: Easing.OutCubic
                        }
                    }

                    //菜单
                    Menu {
                        id: cardMenu
                        MenuItem {
                            text: qsTr("编辑")
                            icon.name: "document-edit"
                            onTriggered: {
                                adddate.editingId = modelData.id
                                adddate.editingData = {
                                    name: modelData.name,
                                    repeatIndex: modelData.repeat,
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
                                deleteCountdown.cardid = modelData.id
                                deleteCountdown.name = modelData.name
                                deleteCountdown.open()
                            }
                        }
                    }

                    // 主卡片
                    contentItem: Rectangle {
                        id: cardRect
                        property int cardMargins: 5
                        // 卡片容器大小
                        implicitWidth: cardLayout.implicitWidth + (cardMargins * 2)
                        implicitHeight: cardLayout.implicitHeight + (cardMargins * 2)
                        color: "transparent"
                        ColumnLayout {
                            id: cardLayout
                            // 卡片边距
                            anchors.fill: parent
                            anchors.margins: cardRect.cardMargins

                            //卡片内容
                            Label {
                                text: modelData.name
                                font.bold: true
                                Layout.fillWidth: true
                                wrapMode: Text.Wrap
                                maximumLineCount: 2
                                elide: Text.ElideRight
                            }
                            Label { text: modelData.repeatText }
                            Label { text: modelData.date }
                            Label { text: modelData.daysText }
                        }
                    }
                    // 临近日红色描边
                    Rectangle {
                        anchors.fill: parent
                        color: "transparent"
                        border.color: "red"
                        border.width: 1
                        radius: 8
                        enabled: false
                        visible: modelData.upcoming
                    }

                    // 卡片动画
                    SequentialAnimation {
                        id: pulseAnim

                        NumberAnimation {
                            target: cardAbs
                            property: "pulseScale"
                            to: 0.95
                            duration: 120
                            easing.type: Easing.OutCubic
                        }
                        NumberAnimation {
                            target: cardAbs
                            property: "pulseScale"
                            to: 1.0
                            duration: 120
                            easing.type: Easing.OutCubic
                        }
                    }

                    // 鼠标
                    MouseArea {
                        anchors.fill: parent
                        acceptedButtons: Qt.LeftButton | Qt.RightButton
                        hoverEnabled: true
                        pressAndHoldInterval: 300 // 300ms触发长按
                        onClicked: (mouse) => {
                            if (mouse.button === Qt.LeftButton) {
                                pulseAnim.start()
                                if (!detailsLoader.active) {
                                    detailsLoader.active = true
                                }
                                if (detailsLoader.item) {
                                    detailsLoader.item.modelData = modelData
                                    detailsLoader.item.show()
                                }
                            }
                            if (mouse.button === Qt.RightButton) {
                                cardMenu.popup(mouse.x, mouse.y)
                            }
                        }
                        onPressAndHold: (mouse) => {
                            cardMenu.popup(mouse.x, mouse.y)
                        }

                        onEntered: { depressedId = modelData.id }
                        onExited:  { depressedId = -1 }
                    }
                }
            }
        }

        // 排序
        ListView {
            id: swapList
            visible: countdownsPage.isSortingMode
            Layout.fillWidth: true
            Layout.fillHeight: true
            Layout.preferredHeight: contentHeight
            interactive: false
            model: CountdownManager.countdowns
            clip: true

            // 躲开动画
            moveDisplaced: Transition {
                YAnimator {
                    duration: Kirigami.Units.longDuration
                    easing.type: Easing.InOutQuad
                }
            }

            delegate: Item {
                id: delegateRoot
                width: ListView.view.width
                height: dragItem.implicitHeight

                ItemDelegate {
                    id: dragItem
                    width: parent.width

                    contentItem: RowLayout {
                        spacing: Kirigami.Units.smallSpacing

                        Kirigami.ListItemDragHandle {
                            listItem: dragItem
                            listView: swapList
                            incrementalMoves: false
                            onDropped: (oldIndex, newIndex) => {
                                CountdownManager.moveCountdown(oldIndex, newIndex);
                            }
                        }

                        Label {
                            Layout.fillWidth: true
                            text: modelData.name ?? ""
                            elide: Text.ElideRight
                        }

                        Label {
                            text: modelData.daysText ?? ""
                            opacity: 0.6
                        }
                    }
                }
            }
        }
    }
}
