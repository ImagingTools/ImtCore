import QtQuick 2.15

Node3D {
    readonly property int Point: 0
    readonly property int Line: 1
    readonly property int Area: 2

    property var system: undefined
    property var particle: undefined
    property int shape: 0
    property real emitRate: 10
    property int lifeSpan: 1000
    property int lifeSpanVariation: 0
    property vector3d particleEndScale: Qt.vector3d(-1, -1, -1)
    property vector3d particleRotation: Qt.vector3d(0, 0, 0)
    property vector3d particleRotationVariation: Qt.vector3d(0, 0, 0)
    property vector3d particleRotationVelocity: Qt.vector3d(0, 0, 0)
    property vector3d particleRotationVelocityVariation: Qt.vector3d(0, 0, 0)
    property vector3d velocity: Qt.vector3d(0, 100, 0)
    property vector3d velocityVariation: Qt.vector3d(0, 0, 0)
    property bool enabled: true
}
