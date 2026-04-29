import QtQuick 2.15

Node3D {
    property var system: undefined
    property var particles: undefined
    property vector3d direction: Qt.vector3d(0, -100, 0)
    property real magnitude: 100
    property bool enabled: true
}
