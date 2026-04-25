// PrincipledMaterial - PBR material (metalness/roughness workflow).
import QtQuick 2.15

QtObject {
    property color baseColor: "#ffffff"
    property real metalness: 0
    property real roughness: 1
    property real opacity: 1
    property color emissiveColor: "#000000"
    property var baseColorMap
    property var metalnessMap
    property var roughnessMap
    property var normalMap
    property var emissiveMap
}
