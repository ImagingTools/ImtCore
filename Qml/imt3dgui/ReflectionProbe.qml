import QtQuick 2.15

Node3D {
    readonly property int Infinite: 0
    readonly property int Box: 1

    property bool parallaxCorrection: false
    property int boxSize: 0
    property vector3d boxOffset: Qt.vector3d(0, 0, 0)
    property int quality: 0
    property real refreshInterval: -1
    property bool timeSlicing: false
}
