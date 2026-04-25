/*
 * Node3D - base type for everything in a 3D scene.
 *
 * Pure-QML property bag that matches the JQML web runtime's Node3D API
 * (Tools/JQML/v2/components/Node3D.js).  On the desktop the existing C++
 * render backend (CRhiWidget / COpenGLWidget) handles actual rendering;
 * these QML types serve as a declarative scene-description layer.
 */
import QtQuick 2.15

QtObject {
    property vector3d position: Qt.vector3d(0, 0, 0)
    property vector3d eulerRotation: Qt.vector3d(0, 0, 0)
    property quaternion rotation: Qt.quaternion(1, 0, 0, 0)
    property vector3d scale: Qt.vector3d(1, 1, 1)
    property vector3d pivot: Qt.vector3d(0, 0, 0)
    property bool visible: true
    property real opacity: 1.0
}
