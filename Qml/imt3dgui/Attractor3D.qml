/**
 * \brief Web stub — QtQuick3D Attractor3D replacement for JQML.
 *
 * Property-only stub; on the desktop the real QtQuick3D type is used.
 */
import QtQuick 2.15

Node3D {
    property var system: undefined
    property var particles: undefined
    property vector3d position3D: Qt.vector3d(0, 0, 0)
    property real duration: -1
    property real strength: 50
    property bool enabled: true
    property bool hideAtEnd: false
    property int useCachedPositions: 0
}
