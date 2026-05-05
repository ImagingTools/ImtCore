/**
 * \brief Web stub — base type for all 3-D scene-graph nodes.
 *
 * Mirrors the spatial properties of QtQuick3D's Node (position,
 * rotation, scale, …).  On the desktop the real QtQuick3D types are
 * used; this stub exists so the JQML web compiler can resolve the type.
 */
import QtQuick 2.15

QtObject {
    property vector3d position: Qt.vector3d(0, 0, 0)
    property vector3d eulerRotation: Qt.vector3d(0, 0, 0)
    property quaternion rotation: Qt.quaternion(1, 0, 0, 0)
    property vector3d scale: Qt.vector3d(1, 1, 1)
    property vector3d pivot: Qt.vector3d(0, 0, 0)
    property bool visible3D: true
    property real opacity3D: 1
    property string objectName3D: ""
    property int staticFlags: 0
    readonly property vector3d forward: Qt.vector3d(0, 0, -1)
    readonly property vector3d up: Qt.vector3d(0, 1, 0)
    readonly property vector3d right: Qt.vector3d(1, 0, 0)
    default property list<QtObject> childNodes

    function mapPositionToScene(localPos) { return Qt.vector3d(0, 0, 0) }
    function mapPositionFromScene(scenePos) { return Qt.vector3d(0, 0, 0) }
    function mapDirectionToScene(localDir) { return Qt.vector3d(0, 0, 0) }
    function mapDirectionFromScene(sceneDir) { return Qt.vector3d(0, 0, 0) }
}
