import QtQuick 2.15

QtObject {
    property vector3d position: Qt.vector3d(0, 0, 0)
    property vector3d eulerRotation: Qt.vector3d(0, 0, 0)
    property vector3d scale: Qt.vector3d(1, 1, 1)
    property color color: "#ffffff"
    property var customData: undefined
}
