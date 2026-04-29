import QtQuick 2.15

QtObject {
    property color diffuseColor: "#ffffffff"
    property color specularTint: "#ffffffff"
    property real shininess: 50
    property real opacity: 1
    property color emissiveColor: "#000000ff"
    property var diffuseMap: undefined
    property var normalMap: undefined
}
