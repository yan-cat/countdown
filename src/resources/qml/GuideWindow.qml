import com.countdown
import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import QtQuick.Window 2.15
import org.kde.kirigami as Kirigami

Window {
    id: guideWindow
    title: qsTr("使用引导")
    width: 640
    height: 480
    visible: false
    color: Kirigami.Theme.backgroundColor

    ScrollView {
        anchors.fill: parent
        contentWidth: availableWidth

        TextArea {
            text: CountdownManager.getGuideMarkdown()
            textFormat: TextArea.MarkdownText
            readOnly: true
            wrapMode: TextArea.Wrap
            color: Kirigami.Theme.textColor
            background: null   // 去掉默认背景
        }
    }
}
