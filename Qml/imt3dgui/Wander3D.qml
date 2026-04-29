import QtQuick 2.15

Node3D {
    property var system: undefined
    property var particles: undefined
    property vector3d globalAmount: Qt.vector3d(0, 0, 0)
    property vector3d globalPace: Qt.vector3d(0, 0, 0)
    property vector3d globalPaceStart: Qt.vector3d(0, 0, 0)
    property vector3d uniqueAmount: Qt.vector3d(0, 0, 0)
    property vector3d uniquePace: Qt.vector3d(0, 0, 0)
    property real uniqueAmountVariation: 0
    property real uniquePaceVariation: 0
    property bool enabled: true
}
