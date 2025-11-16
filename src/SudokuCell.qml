import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Item {
    id: root
    width: 50
    height: 50
    property alias cellColor: cellRect.color
    property alias textColor: valueText.color

    // Public properties
    property int value: 0
    property list<bool> candidates: []
    property bool isSelected: false
    property bool isFrozen: false

    GlobalsUtil {
        id: globalUtils
    }

    function conflictAnimate() {
        root.z = 99
        inConflictAnimation.start()
    }

    function invalidAnimate(txt) {
        tmpTxt.scale = 0.0
        tmpTxt.visible = true
        tmpTxt.z = 99
        tmpTxt.text = txt
        invalidValueOrCandidate.start()
    }

    Text {
        id: tmpTxt
        color: "red"
        font.bold: true
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
        anchors.centerIn: parent
        font.pixelSize: parent.width * 0.9
        width: parent.width
        height: parent.height
    }

    SequentialAnimation {
        id: inConflictAnimation
        ParallelAnimation {
            NumberAnimation {
                target: cellRect
                from: 1
                to: 1.2
                property: "scale"
                duration: 200
                easing.type: Easing.InOutQuad
            }
            ColorAnimation {
                target: cellRect
                property: "color"
                to: "red"
                duration: 150
            }
        }
        ParallelAnimation {
            NumberAnimation {
                target: cellRect
                from: 1.2
                to: 1.0
                property: "scale"
                duration: 200
                easing.type: Easing.InOutQuad
            }
            ColorAnimation {
                target: cellRect
                property: "color"
                to: cellColor
                duration: 150
            }
        }
        onFinished: {
            root.z = 0
        }
    }

    SequentialAnimation {
        id: invalidValueOrCandidate
        ParallelAnimation {
            NumberAnimation {
                target: tmpTxt
                from: 0.0
                to: 1.0
                property: "scale"
                duration: 200
                easing.type: Easing.InOutQuad
            }
            NumberAnimation {
                target: cellRect
                from: 1.0
                to: 0.0
                property: "opacity"
                duration: 200
                easing.type: Easing.InOutQuad
            }
        }
        ParallelAnimation {
            NumberAnimation {
                target: tmpTxt
                from: 1.0
                to: 0.0
                property: "scale"
                duration: 200
                easing.type: Easing.InOutQuad
            }
            NumberAnimation {
                target: cellRect
                from: 0.0
                to: 1.0
                property: "opacity"
                duration: 200
                easing.type: Easing.InOutQuad
            }
        }
        onFinished: {
            tmpTxt.visible = false
            tmpTxt.z = true
        }
    }

    Rectangle {
        id: cellRect
        anchors.fill: parent
        border.color: isSelected ? "blue" : "black"
        border.width: isSelected ? 2 : 1
        radius: isSelected ? 4 : 1
        color: "white"

        // Show value if set
        Text {
            id: valueText
            anchors.centerIn: parent
            text: value !== 0 ? value.toString() : ""
            font.pixelSize: cellRect.width * 0.9
            visible: value !== 0
        }

        // Show candidates if value is 0
        Grid {
            id: candidateGrid
            anchors.fill: parent
            anchors.margins: 2
            columns: 3
            spacing: 1
            visible: value === 0

            Repeater {
                model: 9
                delegate: Text {
                    text: (candidates[index + 1] === true) ? (index + 1).toString(
                                                                 ) : ""
                    font.pixelSize: width * 0.8
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                    width: parent.width / 3
                    height: parent.height / 3
                }
            }
        }
    }
}
