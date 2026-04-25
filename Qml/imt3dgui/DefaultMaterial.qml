// DefaultMaterial - classic Phong-style material.
import QtQuick 2.15

QtObject {
    property color diffuseColor: "#ffffff"
    property color specularTint: "#ffffff"
    property real shininess: 50
    property real opacity: 1
    property color emissiveColor: "#000000"
    property var diffuseMap
    property var normalMap
}
