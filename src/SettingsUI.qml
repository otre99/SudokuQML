import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Rectangle {
    id: settings
    property alias difficulty: difficultyCBox.currentIndex
    property alias showPrecalculatedAnns: cBoxPrecalculateAnns.checked
    property alias maxAllowedHints: spBoxHints.value
    property real pixelSize: 18
    radius: 8

    signal save

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 8
        RowLayout {
            Layout.fillHeight: true
            Layout.fillWidth: true
            spacing: 8
            Text {
                Layout.fillHeight: true
                Layout.fillWidth: true
                text: "Difficulty:"
                verticalAlignment: Text.AlignVCenter
                font.pixelSize: settings.pixelSize
            }
            ComboBox {
                id: difficultyCBox
                model: ["Easy", "Medium", "Hard"]
                font.pixelSize: settings.pixelSize
                Layout.fillHeight: true
            }
        }
        RowLayout {
            Layout.fillHeight: true
            Layout.fillWidth: true
            spacing: 8
            Text {
                Layout.fillHeight: true
                Layout.fillWidth: true
                text: "Max. allowed hints:"
                verticalAlignment: Text.AlignVCenter
                font.pixelSize: settings.pixelSize
            }
            SpinBox {
                id: spBoxHints
                from: 0
                to: 10
                value: 3
                Layout.fillHeight: true
                font.pixelSize: settings.pixelSize
            }
        }
        CheckBox {
            Layout.fillHeight: true
            Layout.fillWidth: true
            id: cBoxPrecalculateAnns
            checked: true
            text: "Precalculated Annotations"
            font.pixelSize: settings.pixelSize
        }

        Button {
            id: saveBt
            text: "Save"
            font.pixelSize: settings.pixelSize
            Layout.fillHeight: true
            Layout.fillWidth: true
            onClicked: settings.save()
        }
    }
}
