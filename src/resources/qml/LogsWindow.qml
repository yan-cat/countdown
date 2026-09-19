import com.countdown
import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import QtQuick.Window 2.15
import org.kde.kirigami as Kirigami

Window {
    id: logsWindow
    title: qsTr("软件日志")
    width: 640
    height: 480
    visible: false
    color: Kirigami.Theme.backgroundColor

    property bool pause: false

    Button {
        id: pauseButton
        text: pause ? qsTr("继续刷新日志") : qsTr("暂停刷新日志")

        onClicked: pause = !pause

        z: 100
        anchors.left: parent.left
        anchors.top: parent.top
        anchors.leftMargin: 10
        anchors.topMargin: 10
    }

    Button {
        id: clearButton
        text: qsTr("清空日志")

        onClicked: CountdownDebug.clearLogs()

        z: 100
        anchors.right: parent.right
        anchors.top: parent.top
        anchors.leftMargin: 10
        anchors.topMargin: 10
    }

    ScrollView {
        anchors.fill: parent
        anchors.margins: 10

        contentWidth: availableWidth

        ScrollBar.vertical.policy: ScrollBar.AlwaysOff
        ScrollBar.horizontal.policy: ScrollBar.AlwaysOff

        TextArea {
            id: logText
            readOnly: true
            wrapMode: TextArea.Wrap
            font.family: "monospace"
            textFormat: TextArea.PlainText
        }
    }

    Timer {
        interval: 500
        running: logsWindow.visible && !pause
        repeat: true
        onTriggered: {
            let content = CountdownDebug.getLogs()
            if (content !== logText.text) logText.text = content
            logText.cursorPosition = logText.length
        }
    }
}
