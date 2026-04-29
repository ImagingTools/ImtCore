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
    default property list<QtObject> childNodes
}
