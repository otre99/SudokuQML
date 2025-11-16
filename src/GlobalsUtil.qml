
import QtQuick 2.0
import QtQuick.Window

QtObject {
    property int baseSize: Math.min(Screen.width, Screen.height)
    readonly property int normal: baseSize / 15
    readonly property int horizontalBarHeight: baseSize / 9
}
