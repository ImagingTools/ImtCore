/**
 * \brief Web stub — QtQuick3D PerspectiveCamera replacement for JQML.
 *
 * Property-only stub; on the desktop the real QtQuick3D type is used.
 */
import QtQuick 2.15

Node3D {
    property real fieldOfView: 60
    property real clipNear: 10
    property real clipFar: 10000
    property bool frustumCullingEnabled: false
    property var lookAtNode: undefined
}
