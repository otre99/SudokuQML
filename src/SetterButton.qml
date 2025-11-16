import QtQuick
import QtQuick.Controls
import QtQuick.Window 2.2

Button {
    id: root
    property int number: -1
    property bool selected: false
    property alias pixelSize: textCmp.font.pixelSize
    text: number.toString()
    checked: selected
    visible: number > 0
    background: Rectangle {
        radius: 8
        color: root.selected ? "#2980b9" : "#ecf0f1"
        border.color: root.selected ? "#1c5980" : "#bdc3c7"
        border.width: 2
    }

    contentItem: Text {
        id: textCmp
        text: root.text
        font.bold: true
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
        color: root.selected ? "white" : "lightGray"
        anchors.centerIn: parent
    }
}
