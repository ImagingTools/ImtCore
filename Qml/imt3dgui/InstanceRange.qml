/**
 * \brief Web stub — QtQuick3D InstanceRange replacement for JQML.
 *
 * Property-only stub; on the desktop the real QtQuick3D type is used.
 */
import QtQuick 2.15

QtObject {
    property int from: 0
    property int instanceCount: -1
    property vector3d positionMinimum: Qt.vector3d(0, 0, 0)
    property vector3d positionMaximum: Qt.vector3d(0, 0, 0)
    property vector3d scaleMinimum: Qt.vector3d(1, 1, 1)
    property vector3d scaleMaximum: Qt.vector3d(1, 1, 1)
    property vector3d eulerRotationMinimum: Qt.vector3d(0, 0, 0)
    property vector3d eulerRotationMaximum: Qt.vector3d(0, 0, 0)
    property color colorMinimum: "#ffffff"
    property color colorMaximum: "#ffffff"
    property bool randomSeed: false
}
