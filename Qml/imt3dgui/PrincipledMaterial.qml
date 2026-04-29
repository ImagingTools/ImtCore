import QtQuick 2.15

QtObject {
    property color baseColor: "#ffffffff"
    property real metalness: 0
    property real roughness: 1
    property real opacity: 1
    property color emissiveColor: "#000000ff"
    property var baseColorMap: undefined
    property var metalnessMap: undefined
    property var roughnessMap: undefined
    property var normalMap: undefined
    property var emissiveMap: undefined
}
